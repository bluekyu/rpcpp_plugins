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

#include "rpplugins/recording/recording_stage.hpp"

#include <camera.h>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

namespace rpplugins {

RecordingStage::RequireType RecordingStage::required_inputs_;
RecordingStage::RequireType RecordingStage::required_pipes_ = { "ShadedScene" };

RecordingStage::~RecordingStage() = default;

void RecordingStage::create(void)
{
    stereo_mode_ = pipeline_.is_stereo_mode();

    show_through_target_ = create_target("ShowThrough");
    show_through_target_->add_color_attachment(16);
    if (stereo_mode_)
        show_through_target_->set_layers(2);
    show_through_target_->prepare_buffer();
}

void RecordingStage::reload_shaders()
{
    show_through_target_->set_shader(load_plugin_shader({"show_through.frag.glsl"}, stereo_mode_));
    for (size_t k = 0, k_end = recording_targets_.size(); k < k_end; ++k)
        reload_recording_target_shader(k);
}

void RecordingStage::set_dimensions()
{
    for (auto&& target_tex : recording_targets_)
    {
        const auto source_texture = target_tex.second;
        target_tex.first->set_size(source_texture->get_x_size(), source_texture->get_y_size());
    }
}

void RecordingStage::set_show_through_texture(Texture* tex)
{
}

rpcore::RenderTarget* RecordingStage::make_recording_target(const std::string& target_name, Texture* source_texture, GraphicsOutput::RenderTextureMode rtmode)
{
    const auto source_z_size = source_texture->get_z_size();
    if (source_z_size > 2)
    {
        error("Cannot make basic recording target using Texture with 2 more z-size.");
        return nullptr;
    }

    auto target = create_target(target_name);

    target->set_sort(*show_through_target_->get_sort() - 1);
    target->set_size(source_texture->get_x_size(), source_texture->get_y_size());
    if (source_z_size == 2)
        target->set_layers(2);

    target->set_render_texture_mode(rtmode);
    target->set_size(source_texture->get_x_size(), source_texture->get_y_size());

    const auto num_components = source_texture->get_num_components();
    const int component_bit = 8;

    if (num_components == 1)
    {
        target->add_color_attachment(LVecBase3i(component_bit, 0, 0));
    }
    else if (num_components == 2)
    {
        target->add_color_attachment(LVecBase3i(component_bit, component_bit, 0));
    }
    else if (num_components == 3)
    {
        target->add_color_attachment(LVecBase3i(component_bit));
    }
    else if (num_components == 4)
    {
        target->add_color_attachment(LVecBase4i(component_bit));
    }

    target->prepare_buffer();

    recording_targets_.push_back({target, source_texture});

    reload_recording_target_shader(recording_targets_.size() - 1);

    return target;
}

std::string RecordingStage::get_plugin_id(void) const
{
    return RPPLUGINS_ID_STRING;
}

void RecordingStage::reload_recording_target_shader(size_t index)
{
    auto& target_tex = recording_targets_[index];
    target_tex.first->set_shader(load_plugin_shader({ "recording.frag.glsl" }, target_tex.second->get_z_size() == 2));
    target_tex.first->set_shader_input(ShaderInput("source_texture", target_tex.second));
}

}
