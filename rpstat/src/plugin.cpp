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

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/showbase/loader.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/pluginbase/manager.hpp>

#include "scenegraph_window.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::RPStatPlugin)

namespace rpplugins {

RENDER_PIPELINE_PLUGIN_DOWNCAST_IMPL(RPStatPlugin);

rpcore::BasePlugin::RequrieType RPStatPlugin::require_plugins_;

RPStatPlugin::RPStatPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING)
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

    axis_model_ = rpcore::Globals::base->get_loader()->load_model(get_resource("models/zup-axis.bam"));

    {
        auto holder = std::make_unique<ScenegraphWindow>(axis_model_);
        scenegraph_window_ = holder.get();
        windows_.push_back(std::move(holder));
    }
}

void RPStatPlugin::on_imgui_new_frame()
{
    draw_main_menu_bar();

    for (const auto& window: windows_)
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

    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
        if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
        ImGui::Separator();

        if (ImGui::MenuItem("Cut", "CTRL+X")) {}
        if (ImGui::MenuItem("Copy", "CTRL+C")) {}
        if (ImGui::MenuItem("Paste", "CTRL+V")) {}

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools"))
    {
        if (ImGui::MenuItem("Scenegraph Window"))
        {
            scenegraph_window_->show();
        }

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

}
