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

#include "nodepath_window.hpp"

#include <fmt/ostream.h>

#include <paramNodePath.h>
#include <showBoundsEffect.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>

#include "rpplugins/rpstat/plugin.hpp"
#include "scenegraph_window.hpp"

namespace rpplugins {

NodePathWindow::NodePathWindow() : WindowInterface("NodePath: None", "###NodePath")
{
    RPStatPlugin::get_global_instance()->accept(
        ScenegraphWindow::NODE_SELECTED_EVENT_NAME,
        [this](const Event* ev) { np_ = DCAST(ParamNodePath, ev->get_parameter(0).get_ptr())->get_value(); }
    );
}

void NodePathWindow::draw()
{
    if (np_)
        title_ = fmt::format("NodePath: {}", np_.get_name());
    else
        title_ = "NodePath: None";

    WindowInterface::draw();
}

void NodePathWindow::draw_contents()
{
    if (!np_)
        return;

    if (ImGui::CollapsingHeader("Transforms", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
    {
        LVecBase3f pos = np_.get_pos();
        if (ImGui::InputFloat3("Position", &pos[0]))
            np_.set_pos(pos);

        LVecBase3f hpr = np_.get_hpr();
        if (ImGui::InputFloat3("HPR", &hpr[0]))
            np_.set_hpr(hpr);

        LVecBase3f scale = np_.get_scale();
        if (ImGui::InputFloat3("Scale", &scale[0]))
            np_.set_scale(scale);
    }

    if (ImGui::CollapsingHeader("Render Mode"))
    {
        static const char* render_modes[] = { "unchanged", "filled", "wireframe", "point", "filled flat", "filled wireframe" };
        int render_mode_index = np_.get_render_mode();
        bool changed = ImGui::ListBox("Render Mode\n(single select)", &render_mode_index, render_modes, std::extent<decltype(render_modes)>::value, 5);

        switch (render_mode_index)
        {
            case RenderModeAttrib::Mode::M_unchanged:
            {
                np_.clear_render_mode();
                break;
            }

            case RenderModeAttrib::Mode::M_filled:
            {
                if (changed)
                    np_.set_render_mode_filled();
                break;
            }

            case RenderModeAttrib::Mode::M_wireframe:
            {
                if (changed)
                    np_.set_render_mode_wireframe();
                break;
            }

            case RenderModeAttrib::Mode::M_point:
            {
                float thickness = np_.get_render_mode_thickness();
                changed |= ImGui::InputFloat("Thickness", &thickness);

                if (changed)
                    np_.set_render_mode(RenderModeAttrib::Mode::M_point, thickness);
                break;
            }

            case RenderModeAttrib::Mode::M_filled_flat:
            {
                float thickness = np_.get_render_mode_thickness();
                changed |= ImGui::InputFloat("Thickness", &thickness);

                if (changed)
                    np_.set_render_mode(RenderModeAttrib::Mode::M_filled_flat, thickness);
                break;
            }

            case RenderModeAttrib::Mode::M_filled_wireframe:
            {
                LColor wireframe_color(1);
                if (auto attrib = np_.get_attrib(RenderModeAttrib::get_class_type()))
                    wireframe_color = DCAST(RenderModeAttrib, attrib)->get_wireframe_color();
                changed |= ImGui::ColorEdit4("Wireframe Color", &wireframe_color[0]);

                if (changed)
                    np_.set_render_mode_filled_wireframe(wireframe_color);
                break;
            }
        }
    }

    ImGui::Separator();

    bool visible = !np_.is_hidden();
    if (ImGui::Checkbox("Visible", &visible))
    {
        if (visible)
            np_.show();
        else
            np_.hide();
    }

    bool bounds_visible = np_.get_effect(ShowBoundsEffect::get_class_type()) != nullptr;
    if (ImGui::Checkbox("Show Tight Bounds", &bounds_visible))
    {
        if (bounds_visible)
            np_.show_tight_bounds();
        else
            np_.hide_bounds();
    }

    ImGui::Separator();

    ImGui::BeginGroup();
    ImGui::Text("Flatten:");
    flatten("Light...");
    ImGui::SameLine();
    flatten("Medium...");
    ImGui::SameLine();
    flatten("Strong...");
    ImGui::EndGroup();
}

void NodePathWindow::flatten(const char* popup_id)
{
    if (ImGui::Button(popup_id))
        ImGui::OpenPopup(popup_id);
    if (ImGui::BeginPopupModal(popup_id, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Selected NodePath will be flatten\nThis operation cannot be undone!\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            std::string name(popup_id);
            if (name.find("Light") != std::string::npos)
                np_.flatten_light();
            else if (name.find("Medium") != std::string::npos)
                np_.flatten_medium();
            else if (name.find("Strong") != std::string::npos)
                np_.flatten_strong();

            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
}

}
