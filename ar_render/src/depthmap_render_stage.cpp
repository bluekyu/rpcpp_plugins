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

#include "depthmap_render_stage.hpp"

#include <camera.h>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

namespace rpplugins {

DepthmapRenderStage::RequireType DepthmapRenderStage::required_inputs;
DepthmapRenderStage::RequireType DepthmapRenderStage::required_pipes;

DepthmapRenderStage::ProduceType DepthmapRenderStage::get_produced_pipes(void) const
{
    return {
        ShaderInput("RealDepthMap", target_->get_depth_tex()),
    };
}

DepthmapRenderStage::~DepthmapRenderStage()
{
    real_depth_cam_np_.remove_node();
}

void DepthmapRenderStage::create(void)
{
    stereo_mode_ = pipeline_.is_stereo_mode();

    real_depth_cam_ = new Camera("real_depth_camera");
    real_depth_cam_->set_lens(rpcore::Globals::base->get_cam_lens());
    real_depth_cam_np_ = rpcore::Globals::base->get_cam().attach_new_node(real_depth_cam_);

    target_ = create_target("RealDepthMap");
    target_->add_depth_attachment(32);
    if (stereo_mode_)
        target_->set_layers(2);
    target_->prepare_render(real_depth_cam_np_);
}

void DepthmapRenderStage::set_shader_input(const ShaderInput& inp)
{
    rpcore::Globals::render.set_shader_input(inp);
}

NodePath DepthmapRenderStage::get_camera(void) const
{
    return real_depth_cam_np_;
}

void DepthmapRenderStage::set_ar_camera(const NodePath& cam)
{
    Camera* camera_node = DCAST(Camera, cam.node());
    real_depth_cam_->set_camera_mask(camera_node->get_camera_mask());
}

std::string DepthmapRenderStage::get_plugin_id(void) const
{
    return RPPLUGIN_ID_STRING;
}

}
