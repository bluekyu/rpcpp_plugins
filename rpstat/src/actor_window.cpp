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

#include "actor_window.hpp"

#include <fmt/ostream.h>

#include <paramNodePath.h>
#include <showBoundsEffect.h>
#include <cullFaceAttrib.h>
#include <depthTestAttrib.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>

#include "rpplugins/rpstat/plugin.hpp"
#include "scenegraph_window.hpp"
#include "file_dialog.hpp"

namespace rpplugins {

ActorWindow::ActorWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline) : WindowInterface(plugin, pipeline, "Actor Window", "###Actor")
{
    window_flags_ |= ImGuiWindowFlags_MenuBar;

    accept(
        ScenegraphWindow::NODE_SELECTED_EVENT_NAME,
        [this](const Event* ev) {
            auto np = DCAST(ParamNodePath, ev->get_parameter(0).get_ptr())->get_value();

            auto scene_window = plugin_.get_scenegraph_window();
            if (scene_window->is_actor(np))
                set_actor(scene_window->get_actor(np));
            else
                set_actor(nullptr);
        }
    );
}

void ActorWindow::draw_contents()
{

}

void ActorWindow::set_actor(rppanda::Actor* actor)
{
    actor_ = actor;
}

}
