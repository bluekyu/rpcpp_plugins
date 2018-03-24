/**
 * MIT License
 * 
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence
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

#include "openvr_render_stage.hpp"

#include <graphicsWindow.h>
#include <textureContext.h>
#include <callbackNode.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/util/post_process_region.hpp>

namespace rpplugins {

class SubmitCallback : public CallbackObject
{
public:
    SubmitCallback(rpcore::RenderTarget* left, rpcore::RenderTarget* right): left_(left), right_(right)
    {
        gsg_ = rpcore::Globals::base->get_win()->get_gsg();
    }

    virtual void do_callback(CallbackData* cbdata)
    {
        if (cbdata)
            cbdata->upcall();

        const auto left_id = left_->get_color_tex()->prepare_now(
            gsg_->get_current_tex_view_offset(), gsg_->get_prepared_objects(), gsg_)->get_native_id();

        const auto right_id = right_->get_color_tex()->prepare_now(
            gsg_->get_current_tex_view_offset(), gsg_->get_prepared_objects(), gsg_)->get_native_id();

        auto compositor = vr::VRCompositor();

        vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)(left_id), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        compositor->Submit(vr::Eye_Left, &leftEyeTexture);

        vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)(right_id), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        compositor->Submit(vr::Eye_Right, &rightEyeTexture);

        compositor->PostPresentHandoff();
    }

    ALLOC_DELETED_CHAIN(SubmitCallback);

private:
    GraphicsStateGuardian* gsg_;
    const rpcore::RenderTarget* left_;
    const rpcore::RenderTarget* right_;
};

OpenVRRenderStage::RequireType OpenVRRenderStage::required_inputs_;
OpenVRRenderStage::RequireType OpenVRRenderStage::required_pipes_ = { "ShadedScene" };

void OpenVRRenderStage::create()
{
    // without glTextureView
    target_left_ = create_target("left_distortion");
    target_left_->add_color_attachment(8, true);
    target_left_->set_size(render_target_size_);
    target_left_->prepare_buffer();
    target_left_->set_shader_input(ShaderInput("vr_eye", LVecBase4i(0, 0, 0, 0)));

    target_right_ = create_target("right_distortion");
    target_right_->add_color_attachment(8, true);
    target_right_->set_size(render_target_size_);
    target_right_->prepare_buffer();
    target_right_->set_shader_input(ShaderInput("vr_eye", LVecBase4i(1, 0, 0, 0)));

    PT(CallbackNode) submit_node = new CallbackNode("OpenVRSubmitNode");
    submit_node->set_draw_callback(new SubmitCallback(target_left_, target_right_));

    auto submit_np = target_right_->get_postprocess_region()->get_node().attach_new_node(submit_node);
    submit_np.set_depth_test(false);
    submit_np.set_depth_write(false);
    submit_np.set_bin("unsorted", 10);
}

void OpenVRRenderStage::reload_shaders()
{
    target_left_->set_shader(load_plugin_shader({"openvr_render.frag.glsl"}));
    target_right_->set_shader(load_plugin_shader({"openvr_render.frag.glsl"}));
}

std::string OpenVRRenderStage::get_plugin_id() const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
