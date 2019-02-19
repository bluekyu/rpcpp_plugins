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

#include "rpplugins/ar_render/plugin.hpp"

#include <boost/dll/alias.hpp>

#include <fmt/ostream.h>

#include <render_pipeline/rpcore/loader.hpp>

#include "rpplugins/ar_render/ar_composite_stage.hpp"

#include "depthmap_render_stage.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::ARRenderPlugin)

namespace rpplugins {

rpcore::BasePlugin::RequrieType ARRenderPlugin::require_plugins_;

ARRenderPlugin::ARRenderPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGINS_ID_STRING)
{
}

rpcore::BasePlugin::RequrieType& ARRenderPlugin::get_required_plugins(void) const
{
    return require_plugins_;
}

void ARRenderPlugin::on_stage_setup(void)
{
    auto depthmap_render_stage = std::make_unique<DepthmapRenderStage>(pipeline_);
    depthmap_render_stage_ = depthmap_render_stage.get();
    add_stage(std::move(depthmap_render_stage));

    auto ar_composite_stage = std::make_unique<ARCompositeStage>(pipeline_);
    ar_composite_stage_ = ar_composite_stage.get();
    add_stage(std::move(ar_composite_stage));
}

NodePath ARRenderPlugin::get_real_depth_camera(void) const
{
    return depthmap_render_stage_->get_camera();
}

void ARRenderPlugin::set_ar_camera_color_texture(Texture* tex, bool is_bgr, bool flip_vertical)
{
    ar_composite_stage_->set_ar_camera_color_texture(tex, is_bgr, flip_vertical);
}

void ARRenderPlugin::set_ar_camera(const NodePath& cam)
{
    if (!cam.node()->is_of_type(Camera::get_class_type()))
    {
        error(fmt::format("The NodePath is not type of Camera: {}", cam));
        return;
    }

    depthmap_render_stage_->set_ar_camera(cam);
    ar_composite_stage_->set_ar_camera(cam);
}

void ARRenderPlugin::set_ar_camera(const NodePath& left_cam, const NodePath& right_cam)
{
    if (!left_cam.node()->is_of_type(Camera::get_class_type()))
    {
        error(fmt::format("The NodePath is not type of Camera: {}", left_cam));
        return;
    }

    if (!right_cam.node()->is_of_type(Camera::get_class_type()))
    {
        error(fmt::format("The NodePath is not type of Camera: {}", right_cam));
        return;
    }

    depthmap_render_stage_->set_ar_camera(left_cam);
    ar_composite_stage_->set_ar_camera(left_cam, right_cam);
}

}
