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

#include <fmt/format.h>

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
    for (auto&& target_info : recording_targets_)
    {
        target_info.target->set_size(target_info.source_texture->get_x_size(), target_info.source_texture->get_y_size());
    }
}

void RecordingStage::set_show_through_texture(Texture* tex)
{
}

rpcore::RenderTarget* RecordingStage::make_recording_target(const std::string& target_name, Texture* source_texture,
    GraphicsOutput::RenderTextureMode rtmode, const Filename& fragment_shader_path)
{
    const auto source_z_size = source_texture->get_z_size();
    const auto tex_type = source_texture->get_texture_type();
    if (source_z_size > 2)
    {
        error("Cannot make recording target using Texture with 2 more z-size.");
        return nullptr;
    }
    else if (tex_type != Texture::TextureType::TT_2d_texture && tex_type != Texture::TextureType::TT_2d_texture_array)
    {
        error("Can make recording target using only Texture2D or Texture2DArray.");
        return nullptr;
    }
    else if (source_z_size == 1 && tex_type == Texture::TextureType::TT_2d_texture_array)
    {
        error("Cannot make recording target using Texture2DArray with 1 z-size.");
        return nullptr;
    }

    auto target = setup_recording_target(target_name, source_texture, rtmode, fragment_shader_path);

    if (source_z_size == 2)
        target->set_layers(2);

    target->prepare_buffer();
    reload_recording_target_shader(recording_targets_.size() - 1);

    return target;
}

rpcore::RenderTarget* RecordingStage::make_recording_target_as_mono(const std::string& target_name, Texture* source_texture,
    int layer, GraphicsOutput::RenderTextureMode rtmode, const Filename& fragment_shader_path)
{
    if (source_texture->get_texture_type() == Texture::TextureType::TT_2d_texture)
        return make_recording_target(target_name, source_texture, rtmode, fragment_shader_path);

    auto target = setup_recording_target(target_name, source_texture, rtmode, fragment_shader_path);
    recording_targets_.back().layer = layer;

    target->prepare_buffer();
    reload_recording_target_shader(recording_targets_.size() - 1);

    return target;
}

std::string RecordingStage::get_plugin_id(void) const
{
    return RPPLUGINS_ID_STRING;
}

rpcore::RenderTarget* RecordingStage::setup_recording_target(
    const std::string& target_name,
    Texture* source_texture,
    GraphicsOutput::RenderTextureMode rtmode,
    const Filename& fragment_shader_path)
{
    auto target = create_target(target_name);

    target->set_sort(*show_through_target_->get_sort() - 1);
    target->set_size(source_texture->get_x_size(), source_texture->get_y_size());
    target->set_render_texture_mode(rtmode);

    const auto num_components = source_texture->get_num_components();
    const int component_bit = 8;

    switch (num_components)
    {
    case 1:
        target->add_color_attachment(LVecBase3i(component_bit, 0, 0));
        break;

    case 2:
        target->add_color_attachment(LVecBase3i(component_bit, component_bit, 0));
        break;

    case 3:
        target->add_color_attachment(LVecBase3i(component_bit));
        break;

    case 4:
        target->add_color_attachment(LVecBase4i(component_bit));
        break;

    default:
        break;
    }

    TargetInfo info;
    info.target = target;
    info.source_texture = source_texture;
    info.shader_path = fragment_shader_path;

    recording_targets_.push_back(std::move(info));

    return target;
}

void RecordingStage::reload_recording_target_shader(size_t index)
{
    auto& target_info = recording_targets_[index];

    const bool stereo_mode = target_info.source_texture->get_z_size() == 2;

    if (target_info.shader_path.empty())
    {
        std::string shader_path = "recording.frag.glsl";
        if (target_info.layer)
        {
            auto tex_type = target_info.source_texture->get_texture_type();

            switch (tex_type)
            {
            case Texture::TextureType::TT_2d_texture_array:
                shader_path = "recording_as_mono_2darray.frag.glsl";
                break;
            case Texture::TextureType::TT_3d_texture:
                shader_path = "recording_as_mono_3d.frag.glsl";
                break;
            default:
                error(fmt::format("RecordingStage::reload_recording_target_shader: invalid texture type ({})", Texture::format_texture_type(tex_type)));
                return;
            }
        }
        else
        {
            if (target_info.source_texture->get_z_size() == 2)
                shader_path = "recording_stereo.frag.glsl";
        }

        target_info.target->set_shader(load_plugin_shader({ shader_path }, stereo_mode));
        if (target_info.layer)
            target_info.target->set_shader_input(ShaderInput("layer", LVecBase4i(*target_info.layer, 0, 0, 0)));
    }
    else
    {
        target_info.target->set_shader(get_shader_handle(Filename(), { target_info.shader_path }, stereo_mode));
    }

    target_info.target->set_shader_input(ShaderInput("source_texture", target_info.source_texture));
}

}
