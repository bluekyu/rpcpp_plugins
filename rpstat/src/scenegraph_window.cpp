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

#include <lens.h>
#include <material.h>
#include <geomNode.h>
#include <paramNodePath.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/actor/actor.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>

#include "ImGuizmo/ImGuizmo.h"

#include "rpplugins/rpstat/plugin.hpp"
#include "material_window.hpp"
#include "texture_window.hpp"

namespace rpplugins {

static constexpr const char* SHOW_MATERIAL_WINDOW_TEXT = "Show Material Window";
static constexpr const char* SHOW_TEXTURE_WINDOW_TEXT = "Show Texture Window";

ScenegraphWindow::ScenegraphWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline) : WindowInterface(plugin, pipeline, "Scenegraph", "###Scenegraph")
{
    root_ = rpcore::Globals::render.attach_new_node("imgui-ScenegraphWindow-root");

    accept(
        CHANGE_SELECTED_NODE_EVENT_NAME,
        [this](const Event* ev) { change_selected_nodepath(DCAST(ParamNodePath, ev->get_parameter(0).get_ptr())->get_value()); }
    );
}

ScenegraphWindow::~ScenegraphWindow() = default;

void ScenegraphWindow::draw()
{
    if (!is_open_ && selected_np_)
        selected_np_.clear();

    WindowInterface::draw();
}

void ScenegraphWindow::draw_contents()
{
    // gizmo buttons
    ImGui::RadioButton("Translate", &gizmo_op_, 0); ImGui::SameLine();
    ImGui::RadioButton("Rotate", &gizmo_op_, 1); ImGui::SameLine();
    ImGui::RadioButton("Scale", &gizmo_op_, 2);

    // scenegraph
    ImGui::BeginChild("child_scenegraph", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    draw_nodepath(rpcore::Globals::base->get_render());

    ImGui::EndChild();

    // gizmo
    if (selected_np_)
        draw_gizmo();
}

void ScenegraphWindow::change_selected_nodepath(NodePath np)
{
    selected_np_ = np;

    throw_event(NODE_SELECTED_EVENT_NAME, EventParameter(new ParamNodePath(selected_np_)));
}

rppanda::Actor* ScenegraphWindow::get_actor(NodePath actor) const
{
    auto found = actor_map_.find(actor);
    if (found == actor_map_.end())
        return nullptr;
    else
        return found->second;
}

bool ScenegraphWindow::is_actor(NodePath actor) const
{
    return actor_map_.find(actor) != actor_map_.end();
}

void ScenegraphWindow::add_actor(PT(rppanda::Actor) actor)
{
    actor_map_.emplace(*actor, actor);
}

void ScenegraphWindow::remove_actor(rppanda::Actor* actor)
{
    actor_map_.erase(*actor);
}

void ScenegraphWindow::remove_actor(NodePath actor)
{
    actor_map_.erase(actor);
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

    draw_nodepath_context(np);

    if (node_open)
    {
        for (int k = 0, k_end = np.get_num_children(); k < k_end; ++k)
            draw_nodepath(np.get_child(k));

        if (np.node()->is_geom_node())
            draw_geomnode(DCAST(GeomNode, np.node()));

        ImGui::TreePop();
    }
}

void ScenegraphWindow::draw_nodepath_context(NodePath np)
{
    if (!ImGui::BeginPopupContextItem())
        return;

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

    if (np.has_texture())
    {
        if (ImGui::Selectable(SHOW_TEXTURE_WINDOW_TEXT))
        {
            //    send_show_event("###Texture");
            //    throw_event(TextureWindow::TEXTURE_SELECTED_EVENT_NAME, EventParameter(new ParamNodePath(np)));
        }
    }
    else
    {
        ImGui::TextDisabled(SHOW_TEXTURE_WINDOW_TEXT);
    }

    if (ImGui::Selectable("Copy"))
        plugin_.set_copied_nodepath(np);

    ImGui::EndPopup();
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

            if (state.has_texture())
            {
                if (ImGui::Selectable(SHOW_TEXTURE_WINDOW_TEXT))
                {
                //    send_show_event("###Texture");
                //    throw_event(TextureWindow::TEXTURE_SELECTED_EVENT_NAME, EventParameter(new ParamNodePath(np)));
                }
            }
            else
            {
                ImGui::TextDisabled(SHOW_TEXTURE_WINDOW_TEXT);
            }
            ImGui::EndPopup();
        }

        if (node_open)
        {
            ImGui::TreePop();
        }
    }
}

void ScenegraphWindow::draw_gizmo()
{
    ImGuizmo::BeginFrame();

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    static LMatrix4f view_mat;
    static LMatrix4f proj_mat;
    static LMatrix4f obj_mat;

    view_mat = rpcore::Globals::render.get_mat(rpcore::Globals::base->get_cam(0));
    proj_mat = rpcore::Globals::base->get_cam_lens(0)->get_projection_mat();
    obj_mat = selected_np_.get_mat(rpcore::Globals::render);

    ImGuizmo::Manipulate(&view_mat(0, 0), &proj_mat(0, 0), ImGuizmo::OPERATION(gizmo_op_), ImGuizmo::LOCAL, &obj_mat(0, 0), NULL, NULL, NULL, NULL);

    selected_np_.set_mat(rpcore::Globals::render, obj_mat);
}

}
