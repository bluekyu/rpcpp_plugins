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
#include "actor_window.hpp"

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
    windows_.push_back(std::make_unique<ActorWindow>(*this, pipeline_));
    windows_.push_back(std::make_unique<MaterialWindow>(*this, pipeline_));
    windows_.push_back(std::make_unique<TextureWindow>(*this, pipeline_));
    windows_.push_back(std::make_unique<DayManagerWindow>(*this, pipeline_));

    imgui_plugin_ = static_cast<ImGuiPlugin*>(pipeline_.get_plugin_mgr()->get_instance("imgui")->downcast());
    accept(ImGuiPlugin::DROPFILES_EVENT_NAME, [this](auto) { file_dropped_ = true; });
}

const std::vector<Filename>& RPStatPlugin::get_dropped_files() const
{
    return imgui_plugin_->get_dropped_files();
}

void RPStatPlugin::add_window(std::unique_ptr<WindowInterface> window)
{
    windows_.push_back(std::move(window));
}

std::unique_ptr<WindowInterface> RPStatPlugin::remove_window(WindowInterface* window)
{
    auto found = std::find_if(windows_.begin(), windows_.end(), [&](const std::unique_ptr<WindowInterface>& w) {
        return w.get() == window;
    });
    if (found == windows_.end())
    {
        return nullptr;
    }
    else
    {
        std::unique_ptr<WindowInterface> holder = std::move(*found);
        windows_.erase(found);
        return holder;
    }
}

void RPStatPlugin::on_imgui_new_frame()
{
    draw_main_menu_bar();

    for (const auto& window : windows_)
        window->draw();
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
            for (const auto& window_title: {"Scenegraph", "NodePath", "Actor", "Material", "Texture", "Day Manager"})
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

}
