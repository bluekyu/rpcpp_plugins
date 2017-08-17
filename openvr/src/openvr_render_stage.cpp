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

#include <glgsg.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

namespace rpplugins {

OpenvrRenderStage::RequireType OpenvrRenderStage::required_inputs_;
OpenvrRenderStage::RequireType OpenvrRenderStage::required_pipes_ = { "ShadedScene" };

void OpenvrRenderStage::create()
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
}

void OpenvrRenderStage::reload_shaders()
{
    target_left_->set_shader(load_plugin_shader({"openvr_render.frag.glsl"}));
    target_right_->set_shader(load_plugin_shader({"openvr_render.frag.glsl"}));
}

unsigned int OpenvrRenderStage::get_eye_texture(vr::EVREye vr_eye)
{
    GLGraphicsStateGuardian* glgsg = reinterpret_cast<GLGraphicsStateGuardian*>(rpcore::Globals::base->get_win()->get_gsg());
    if (!glgsg)
        return 0;

    const std::shared_ptr<rpcore::RenderTarget> target = vr_eye == vr::EVREye::Eye_Left ? target_left_ : target_right_;
    return reinterpret_cast<GLTextureContext*>(target->get_color_tex()->prepare_now(
        glgsg->get_current_tex_view_offset(), glgsg->get_prepared_objects(), glgsg))->_index;
}

std::string OpenvrRenderStage::get_plugin_id() const
{
    return RPPLUGIN_ID_STRING;
}

}    // namespace rpplugins
