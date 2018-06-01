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

#include <paramNodePath.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>

namespace rpplugins {

void ScenegraphWindow::draw_contents()
{
    draw_nodepath(rpcore::Globals::base->get_render());
}

void ScenegraphWindow::draw_nodepath(NodePath np)
{
    if (!np || np == axis_model_)
        return;

    bool node_open = false;
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        (selected_np_ == np ? ImGuiTreeNodeFlags_Selected : 0);

    if (np.get_num_children() == 0)
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

    if (node_open)
    {
        for (int k = 0, k_end = np.get_num_children(); k < k_end; ++k)
            draw_nodepath(np.get_child(k));

        ImGui::TreePop();
    }
}

void ScenegraphWindow::change_selected_nodepath(NodePath np)
{
    selected_np_ = np;

    axis_model_.reparent_to(np);
    axis_model_.set_scale(rpcore::Globals::render, 0.25f);

    throw_event(NODE_SELECTED_EVENT_NAME, EventParameter(new ParamNodePath(selected_np_)));
}

}
