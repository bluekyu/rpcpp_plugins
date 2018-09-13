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

#include "rpplugins/rpstat/plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <fmt/ostream.h>

#include <imgui.h>

#include <paramNodePath.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/showbase/loader.hpp>
#include <render_pipeline/rppanda/actor/actor.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/pluginbase/manager.hpp>

#include <rpplugins/imgui/plugin.hpp>

#include "scenegraph_window.hpp"
#include "nodepath_window.hpp"
#include "material_window.hpp"
#include "texture_window.hpp"
#include "day_manager_window.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::RPStatPlugin)

namespace rpplugins {

rpcore::BasePlugin::RequrieType RPStatPlugin::require_plugins_ = { "imgui" };

RPStatPlugin::RPStatPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGINS_ID_STRING)
{
}

RPStatPlugin::~RPStatPlugin() = default;

rpcore::BasePlugin::RequrieType& RPStatPlugin::get_required_plugins(void) const
{
    return require_plugins_;
}

void RPStatPlugin::on_load()
{
}

void RPStatPlugin::on_pipeline_created()
{
    if (!pipeline_.get_plugin_mgr()->is_plugin_enabled("imgui"))
    {
        error("imgui plugin is not enabled.");
        return;
    }

    rppanda::Messenger::get_global_instance()->send(
        "imgui-setup-context",
        EventParameter(new rppanda::FunctionalTask([this](rppanda::FunctionalTask* task) {
            ImGui::SetCurrentContext(std::static_pointer_cast<ImGuiContext>(task->get_user_data()).get());
            accept("imgui-new-frame", [this](auto) { on_imgui_new_frame(); });
            return AsyncTask::DS_done;
        }, "World::setup-imgui"))
    );

    auto scenegraph_window_holder = std::make_unique<ScenegraphWindow>(*this, pipeline_);
    scenegraph_window_ = scenegraph_window_holder.get();
    windows_.push_back(std::move(scenegraph_window_holder));
    windows_.push_back(std::make_unique<NodePathWindow>(*this, pipeline_));
    windows_.push_back(std::make_unique<MaterialWindow>(*this, pipeline_));
    windows_.push_back(std::make_unique<TextureWindow>(*this, pipeline_));
    windows_.push_back(std::make_unique<DayManagerWindow>(*this, pipeline_));

    imgui_plugin_ = static_cast<ImGuiPlugin*>(pipeline_.get_plugin_mgr()->get_instance("imgui")->downcast());
    accept(ImGuiPlugin::DROPFILES_EVENT_NAME, [this](auto) { file_dropped_ = true; });
}

void RPStatPlugin::on_imgui_new_frame()
{
    draw_main_menu_bar();

    for (const auto& window: windows_)
        window->draw();

    // not processed on windows
    draw_dropped_file();
}

void RPStatPlugin::draw_main_menu_bar()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    if (ImGui::BeginMenu("File"))
    {
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools"))
    {
        if (ImGui::BeginMenu("RPStat"))
        {
            for (const auto& window_title: {"Scenegraph", "NodePath", "Material", "Texture", "Day Manager"})
            {
                if (ImGui::MenuItem((window_title + std::string(" Window")).c_str()))
                {
                    WindowInterface::send_show_event(std::string("###") + window_title);
                }
            }
            ImGui::EndMenu();
        }

        rppanda::Messenger::get_global_instance()->send("rpstat-menu-tools");

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

void RPStatPlugin::draw_dropped_file()
{
    static Filename dropped_file;

    if (file_dropped_)
    {
        const auto& files = imgui_plugin_->get_dropped_files();
        if (files.size() > 0)
        {
            dropped_file = files[0];
            if (!dropped_file.empty())
                ImGui::OpenPopup("drop-files");
        }

        file_dropped_ = false;
    }

    if (ImGui::BeginPopup("drop-files"))
    {
        if (ImGui::Selectable("Load as Model"))
        {
            NodePath np;
            try
            {
                np = rpcore::Globals::base->get_loader()->load_model(dropped_file);
            }
            catch (const std::runtime_error& err)
            {
                error(err.what());
            }

            if (np)
            {
                np.reparent_to(rpcore::Globals::render);
                throw_event(ScenegraphWindow::CHANGE_SELECTED_NODE_EVENT_NAME, EventParameter(new ParamNodePath(np)));
            }
        }

        if (ImGui::Selectable("Load as Actor"))
        {
            PT(rppanda::Actor) actor;
            try
            {
                actor = new rppanda::Actor(rppanda::Actor::ModelsType{ dropped_file });
            }
            catch (const std::runtime_error& err)
            {
                error(err.what());
            }

            if (actor)
            {
                scenegraph_window_->add_actor(actor);
                actor->reparent_to(rpcore::Globals::render);
                throw_event(ScenegraphWindow::CHANGE_SELECTED_NODE_EVENT_NAME, EventParameter(new ParamNodePath(NodePath(*actor))));
            }
        }

        ImGui::EndPopup();
    }
}

}
