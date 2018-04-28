/**
 * MIT License
 *
 * Copyright (c) 2018 Center of Human-centered Interaction for Coexistence
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "rpplugins/ar_render/ar_composite_stage.hpp"

#include <camera.h>
#include <virtualFileSystem.h>

#include <spdlog/fmt/ostr.h>

#include <render_pipeline/rpcore/native/tag_state_manager.h>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>

namespace rpplugins {

struct ARCompositeStage::Impl
{
    void create(ARCompositeStage* self);

    bool stereo_mode_ = false;

    bool disable_composite_ = false;
    bool arcam_tex_use_bgr_ = false;
    bool arcam_tex_flip_vertical_ = false;
    bool render_only_valid_ar_depth_ = false;
    bool remove_occlusion_ = false;

    PT(Texture) ar_camera_tex_;
    rpcore::RenderTarget* target_;
    std::vector<rpcore::RenderTarget*> ar_cam_depthmap_targets_;
    std::vector<Camera*> ar_cameras_;
};

void ARCompositeStage::Impl::create(ARCompositeStage* self)
{
    stereo_mode_ = self->pipeline_.is_stereo_mode();

    target_ = self->create_target("ARComposite");
    target_->add_color_attachment(16);
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_buffer();
}

// ************************************************************************************************

ARCompositeStage::RequireType ARCompositeStage::required_inputs_;
ARCompositeStage::RequireType ARCompositeStage::required_pipes_ = { "GBuffer", "ShadedScene", "RealDepthMap" };

ARCompositeStage::RequireType& ARCompositeStage::get_required_inputs() const
{
    return required_inputs_;
}

ARCompositeStage::RequireType& ARCompositeStage::get_required_pipes() const
{
    return required_pipes_;
}

ARCompositeStage::ProduceType ARCompositeStage::get_produced_pipes() const
{
    return {
        { ShaderInput("ShadedScene", impl_->target_->get_color_tex()) },
    };
}

ARCompositeStage::ARCompositeStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "ARCompositeStage"), impl_(std::make_unique<Impl>())
{
}

void ARCompositeStage::create()
{
    impl_->create(this);
}

bool ARCompositeStage::composite_disabled() const
{
    return impl_->disable_composite_;
}

void ARCompositeStage::disable_composite(bool enable)
{
    impl_->disable_composite_ = enable;
    reload_shaders();
}

void ARCompositeStage::render_only_valid_ar_depth(bool enable)
{
    impl_->render_only_valid_ar_depth_ = enable;
    reload_shaders();
}

void ARCompositeStage::remove_occlusion(bool enable)
{
    impl_->remove_occlusion_ = enable;
    reload_shaders();
}

void ARCompositeStage::set_ar_camera_color_texture(Texture* tex, bool is_bgr, bool flip_vertical)
{
    if (!tex)
    {
        error("AR camera texture is null.");
        return;
    }

    impl_->target_->set_shader_input(ShaderInput("ARCamScene", impl_->ar_camera_tex_ = tex));
    impl_->arcam_tex_use_bgr_ = is_bgr;
    impl_->arcam_tex_flip_vertical_ = flip_vertical;

    reload_shaders();
}

void ARCompositeStage::set_ar_camera(const NodePath& cam)
{
    Camera* camera_node = DCAST(Camera, cam.node());

    for (auto&& old_target : impl_->ar_cam_depthmap_targets_)
        remove_target(old_target);
    impl_->ar_cam_depthmap_targets_.clear();

    auto tag_mgr = pipeline_.get_tag_mgr();
    for (auto cam : impl_->ar_cameras_)
        tag_mgr->unregister_camera("shadow", cam);
    impl_->ar_cameras_.clear();

    auto target = create_target("ARCamDepthMap");
    target->add_depth_attachment(32);
    target->set_sort(impl_->target_->get_sort().get() - 1);
    if (impl_->ar_camera_tex_)
        target->set_size(impl_->ar_camera_tex_->get_x_size(), impl_->ar_camera_tex_->get_y_size());

    target->prepare_render(cam);
    impl_->target_->set_shader_input(ShaderInput("ARCamDepthMap", target->get_depth_tex()));
    impl_->ar_cam_depthmap_targets_.push_back(target);

    auto cam_mask = camera_node->get_camera_mask();
    tag_mgr->register_camera("shadow", camera_node);
    impl_->ar_cameras_.push_back(camera_node);
    camera_node->set_camera_mask(cam_mask);

    impl_->target_->set_shader_input(ShaderInput("arCamera", cam));
    impl_->target_->set_shader_input(ShaderInput("ar_camera_proj_mat", camera_node->get_lens()->get_projection_mat()));

    pipeline_.reload_shaders();
}

void ARCompositeStage::set_ar_camera(const NodePath& left_cam, const NodePath& right_cam)
{
    Camera* left_camera_node = DCAST(Camera, left_cam.node());
    Camera* right_camera_node = DCAST(Camera, right_cam.node());

    for (auto&& old_target : impl_->ar_cam_depthmap_targets_)
        remove_target(old_target);
    impl_->ar_cam_depthmap_targets_.clear();

    auto tag_mgr = pipeline_.get_tag_mgr();
    for (auto cam : impl_->ar_cameras_)
        tag_mgr->unregister_camera("shadow", cam);
    impl_->ar_cameras_.clear();

    auto target = create_target("ARCamDepthMap_left");
    target->add_depth_attachment(32);
    target->set_sort(impl_->target_->get_sort().get() - 2);
    if (impl_->ar_camera_tex_)
        target->set_size(impl_->ar_camera_tex_->get_x_size(), impl_->ar_camera_tex_->get_y_size());
    target->prepare_render(left_cam);
    impl_->target_->set_shader_input(ShaderInput("ARCamDepthMap_left", target->get_depth_tex()));
    impl_->ar_cam_depthmap_targets_.push_back(target);

    auto cam_mask = left_camera_node->get_camera_mask();
    tag_mgr->register_camera("shadow", left_camera_node);
    impl_->ar_cameras_.push_back(left_camera_node);
    left_camera_node->set_camera_mask(cam_mask);

    target = create_target("ARCamDepthMap_right");
    target->add_depth_attachment(32);
    target->set_sort(impl_->target_->get_sort().get() - 1);
    if (impl_->ar_camera_tex_)
        target->set_size(impl_->ar_camera_tex_->get_x_size(), impl_->ar_camera_tex_->get_y_size());
    target->prepare_render(right_cam);
    impl_->target_->set_shader_input(ShaderInput("ARCamDepthMap_right", target->get_depth_tex()));
    impl_->ar_cam_depthmap_targets_.push_back(target);

    cam_mask = right_camera_node->get_camera_mask();
    tag_mgr->register_camera("shadow", right_camera_node);
    impl_->ar_cameras_.push_back(right_camera_node);
    right_camera_node->set_camera_mask(cam_mask);

    impl_->target_->set_shader_input(ShaderInput("arCameraLeft", left_cam));
    impl_->target_->set_shader_input(ShaderInput("arCameraRight", right_cam));

    impl_->target_->set_shader_input(ShaderInput("ar_camera_proj_mat_left", left_camera_node->get_lens()->get_projection_mat()));
    impl_->target_->set_shader_input(ShaderInput("ar_camera_proj_mat_right", right_camera_node->get_lens()->get_projection_mat()));

    pipeline_.reload_shaders();
}

void ARCompositeStage::reload_shaders()
{
    // Generate autoconfig as string
    std::string output = "#pragma once\n\n";
    output += "// Autogenerated by the 'ar_render' plugin\n";
    output += "// Do not edit! Your changes will be lost.\n\n";

    output += fmt::format(
        "#define DISABLE_COMPOSITE {}\n"
        "#define ARCAM_TEX_USE_BGR {}\n"
        "#define ARCAM_TEX_FLIP_VERTICAL {}\n"
        "#define REMOVE_OCCLUSION {}\n"
        "#define RENDER_ONLY_VALID_AR_DEPTH {}\n",

        impl_->disable_composite_ ? "1" : "0",
        impl_->arcam_tex_use_bgr_ ? "1" : "0",
        impl_->arcam_tex_flip_vertical_ ? "1" : "0",
        impl_->remove_occlusion_ ? "1" : "0",
        impl_->render_only_valid_ar_depth_ ? "1" : "0"
    );

    VirtualFileSystem* vfs = VirtualFileSystem::get_global_ptr();
    try
    {
        std::ostream* file = vfs->open_write_file("/$$rptemp/$$rpplugins_ar_render.inc.glsl", false, true);
        *file << output;
        vfs->close_write_file(file);
    }
    catch (const std::exception& err)
    {
        error(fmt::format("Error writing shader autoconfig: {}", err.what()));
    }

    if (!impl_->ar_camera_tex_)
        impl_->target_->set_shader(load_plugin_shader({ "empty_ar_composite.frag.glsl" }, impl_->stereo_mode_));
    else if (impl_->ar_camera_tex_->get_texture_type() == Texture::TextureType::TT_2d_texture)
        impl_->target_->set_shader(load_plugin_shader({ "mono_ar_composite.frag.glsl" }, impl_->stereo_mode_));
    else if (impl_->ar_camera_tex_->get_texture_type() == Texture::TextureType::TT_2d_texture_array)
        impl_->target_->set_shader(load_plugin_shader({ "stereo_ar_composite.frag.glsl" }, impl_->stereo_mode_));
    else
        error("Invalid texture type!");
}

std::string ARCompositeStage::get_plugin_id() const
{
    return RPPLUGIN_ID_STRING;
}

}
