/**
 * MIT License
 *
 * Copyright (c) 2018 Younguk Kim (bluekyu)
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

#pragma once

#include "texture_window.hpp"

#include <fmt/ostream.h>

#include <paramNodePath.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>

#include "rpplugins/rpstat/plugin.hpp"
#include "scenegraph_window.hpp"

namespace rpplugins {

TextureWindow::TextureWindow(RPStatPlugin& plugin) : WindowInterface(plugin, "Texture Window", "###Texture")
{
    plugin.accept(
        ScenegraphWindow::NODE_SELECTED_EVENT_NAME,
        [this](const Event* ev) { set_nodepath(DCAST(ParamNodePath, ev->get_parameter(0).get_ptr())->get_value()); }
    );
}

void TextureWindow::draw_contents()
{
    if (!np_)
        return;

    if (tex_collection_.get_num_textures() == 0)
        return;

    static const char* empty_name = "(no-name)";

    texture_names_.clear();
    for (int k = 0, k_end = tex_collection_.get_num_textures(); k < k_end; ++k)
    {
        auto tex = tex_collection_.get_texture(k);
        if (tex->has_name())
            texture_names_.push_back(tex_collection_.get_texture(k)->get_name().c_str());
        else
            texture_names_.push_back(empty_name);
    }

    ImGui::Combo("Textures", &current_item_, texture_names_.data(), texture_names_.size());

    Texture* tex = tex_collection_.get_texture(current_item_);

    ImGui::Text("Preview");
    ImGui::Image(
        tex,
        ImVec2(256, 256),
        ImVec2(0, 0),
        ImVec2(1, 1),
        ImColor(255, 255, 255, 255),
        ImColor(255, 255, 255, 128));

    ui_texture_type(tex);
    ui_component_type(tex);

    ImGui::LabelText("Size", "%d x %d x %d", tex->get_x_size(), tex->get_y_size(), tex->get_z_size());
}

void TextureWindow::set_nodepath(NodePath np)
{
    np_ = np;

    if (is_open_)
        tex_collection_ = np_.find_all_textures();
}

void TextureWindow::show()
{
    tex_collection_ = np_.find_all_textures();
    WindowInterface::show();
}

void TextureWindow::ui_texture_type(Texture* tex)
{
    std::string texture_type = "";
    switch (tex->get_texture_type())
    {
    case Texture::TextureType::TT_1d_texture:
        texture_type = "1D Texture";
        break;

    case Texture::TextureType::TT_2d_texture:
        texture_type = "2D Texture";
        break;

    case Texture::TextureType::TT_3d_texture:
        texture_type = "3D Texture";
        break;

    case Texture::TextureType::TT_2d_texture_array:
        texture_type = "2D Texture Array";
        break;

    case Texture::TextureType::TT_cube_map:
        texture_type = "Cube Map";
        break;

    case Texture::TextureType::TT_buffer_texture:
        texture_type = "Buffer Texture";
        break;

    case Texture::TextureType::TT_cube_map_array:
        texture_type = "Cube Map Array";
        break;

    case Texture::TextureType::TT_1d_texture_array:
        texture_type = "1D Texture Array";
        break;
    };
    ImGui::LabelText("Texture Type", texture_type.c_str());
}

void TextureWindow::ui_component_type(Texture* tex)
{
    std::string component_type;
    switch (tex->get_component_type())
    {
    case Texture::ComponentType::T_unsigned_byte:
        component_type = "Unsigned Byte";
        break;

    case Texture::ComponentType::T_unsigned_short:
        component_type = "Unsigned Short";
        break;

    case Texture::ComponentType::T_float:
        component_type = "Float";
        break;

    case Texture::ComponentType::T_unsigned_int_24_8:
        component_type = "Unsigned Int 24 & 8";
        break;

    case Texture::ComponentType::T_int:
        component_type = "Int";
        break;

    case Texture::ComponentType::T_byte:
        component_type = "Byte";
        break;

    case Texture::ComponentType::T_short:
        component_type = "Short";
        break;

    case Texture::ComponentType::T_half_float:
        component_type = "Half Float";
        break;

    case Texture::ComponentType::T_unsigned_int:
        component_type = "Unsigned Int";
        break;
    }
    ImGui::LabelText("Component Type", component_type.c_str());
}

void TextureWindow::ui_texture_format(Texture* tex)
{
    std::string format;
    switch (tex->get_format())
    {
    case Texture::Format::F_depth_stencil:
        format = "Depth Stencil";
        break;

    case Texture::Format::F_color_index:
        format = "Color Index";
        break;

    case Texture::Format::F_red:
        format = "Red";
        break;

    case Texture::Format::F_green:
        format = "Green";
        break;

    case Texture::Format::F_blue:
        format = "Blue";
        break;

    case Texture::Format::F_alpha:
        format = "Alpha";
        break;

    case Texture::Format::F_rgb:
        format = "RGB";
        break;
    }
    ImGui::LabelText("Format", format.c_str());
}

}
