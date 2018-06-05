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

#include "scenegraph_window.hpp"

#include <material.h>
#include <geomNode.h>
#include <paramNodePath.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>

#include "material_window.hpp"

namespace rpplugins {

static constexpr const char* SHOW_MATERIAL_WINDOW_TEXT = "Show Material Window";

ScenegraphWindow::ScenegraphWindow(NodePath axis_model) : WindowInterface("Scenegraph", "###Scenegraph"), axis_model_(axis_model)
{
    root_ = rpcore::Globals::render.attach_new_node("imgui-ScenegraphWindow-root");
}

void ScenegraphWindow::draw()
{
    if (!is_open_ && selected_np_)
    {
        selected_np_.clear();
        if (axis_model_.get_parent() == root_)
            axis_model_.detach_node();
    }

    WindowInterface::draw();
}

void ScenegraphWindow::draw_contents()
{
    draw_nodepath(rpcore::Globals::base->get_render());

    if (selected_np_)
    {
        axis_model_.set_mat(selected_np_.get_mat(rpcore::Globals::render));
        axis_model_.set_scale(0.25f);
    }
}

void ScenegraphWindow::draw_nodepath(NodePath np)
{
    if (!np || np == root_)
        return;

    bool node_open = false;
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        (selected_np_ == np ? ImGuiTreeNodeFlags_Selected : 0);

    // leaf or not.
    if (np.get_num_children() == 0 && !np.node()->is_geom_node())
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(np.node(), flags, np.get_name().c_str());
    }
    else
    {
        node_open = ImGui::TreeNodeEx(np.node(), flags, np.get_name().c_str());
    }

    if (ImGui::IsItemClicked())
        change_selected_nodepath(np);

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::Selectable("Show NodePath Window"))
        {
            send_show_event("###NodePath");
            throw_event(NODE_SELECTED_EVENT_NAME, EventParameter(new ParamNodePath(np)));
        }

        if (np.has_material())
        {
            if (ImGui::Selectable(SHOW_MATERIAL_WINDOW_TEXT))
            {
                send_show_event("###Material");
                throw_event(MaterialWindow::MATERIAL_SELECTED_EVENT_NAME, EventParameter(np.get_material()));
            }
        }
        else
        {
            ImGui::TextDisabled(SHOW_MATERIAL_WINDOW_TEXT);
        }
        ImGui::EndPopup();
    }

    if (node_open)
    {
        for (int k = 0, k_end = np.get_num_children(); k < k_end; ++k)
            draw_nodepath(np.get_child(k));

        if (np.node()->is_geom_node())
            draw_geomnode(DCAST(GeomNode, np.node()));

        ImGui::TreePop();
    }
}

void ScenegraphWindow::draw_geomnode(GeomNode* node)
{
    rpcore::RPGeomNode gn(node);
    for (int k = 0, k_end = gn.get_num_geoms(); k < k_end; ++k)
    {
        const Geom* geom = gn->get_geom(k);
        const auto& state = gn.get_state(k);

        ImGuiTreeNodeFlags flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_OpenOnDoubleClick |
            (selected_geom_ == geom ? ImGuiTreeNodeFlags_Selected : 0);

        bool node_open = ImGui::TreeNodeEx(geom, flags, "Geom %d", k);

        if (ImGui::IsItemClicked())
            selected_geom_ = geom;

        if (ImGui::BeginPopupContextItem())
        {
            if (state.has_material())
            {
                if (ImGui::Selectable(SHOW_MATERIAL_WINDOW_TEXT))
                {
                    send_show_event("###Material");
                    throw_event(MaterialWindow::MATERIAL_SELECTED_EVENT_NAME, EventParameter(state.get_material().get_material()));
                }
            }
            else
            {
                ImGui::TextDisabled(SHOW_MATERIAL_WINDOW_TEXT);
            }
            ImGui::EndPopup();
        }

        if (node_open)
        {
            ImGui::TreePop();
        }
    }
}

void ScenegraphWindow::change_selected_nodepath(NodePath np)
{
    selected_np_ = np;

    axis_model_.reparent_to(root_);

    throw_event(NODE_SELECTED_EVENT_NAME, EventParameter(new ParamNodePath(selected_np_)));
}

}
