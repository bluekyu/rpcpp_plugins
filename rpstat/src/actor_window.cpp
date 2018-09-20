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

#include <paramNodePath.h>

#include <fmt/ostream.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>

#include "rpplugins/rpstat/plugin.hpp"
#include "scenegraph_window.hpp"
#include "load_animation_dialog.hpp"
#include "anim_control_window.hpp"

namespace rpplugins {

ActorWindow::ActorWindow(RPStatPlugin& plugin, rpcore::RenderPipeline& pipeline) : WindowInterface(plugin, pipeline, "Actor Window", "###Actor")
{
    window_flags_ |= ImGuiWindowFlags_MenuBar;

    load_animation_dialog_ = std::make_unique<LoadAnimationDialog>(plugin_, FileDialog::OperationFlag::open);

    for (int k = 0; k <= int(PartBundle::BlendType::BT_componentwise_quat); ++k)
    {
        std::stringstream ss;
        // TODO: "operator<<" is not exported
        //ss << static_cast<PartBundle::BlendType>(k);

        switch (PartBundle::BlendType(k))
        {
        case PartBundle::BT_linear:
            ss << "linear";
            break;
        case PartBundle::BT_normalized_linear:
            ss << "normalized_linear";
            break;
        case PartBundle::BT_componentwise:
            ss << "componentwise";
            break;
        case PartBundle::BT_componentwise_quat:
            ss << "componentwise_quat";
            break;
        }

        blend_type_cache_list_.push_back(ss.str());
    }

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
    enum class MenuID : int
    {
        None,
    };
    static MenuID menu_selected = MenuID::None;

    const bool actor_exist = actor_ != nullptr;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File", actor_exist))
        {
            if (ImGui::MenuItem("Load Animation"))
                load_animation_dialog_->open();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (!actor_exist)
        return;

    ui_load_animation();

    {
        static auto getter = [](void* data, int idx, const char** out_text) {
            auto plugin = reinterpret_cast<ActorWindow*>(data);
            *out_text = std::get<0>(plugin->actor_info_[idx]).c_str();
            return true;
        };

        const int combo_size = static_cast<int>(actor_info_.size());
        lod_item_index_ = (std::min)(lod_item_index_, combo_size - 1);

        if (ImGui::Combo("Level of Details", &lod_item_index_, getter, this, combo_size))
        {
            part_item_index_ = 0;
        }
    }

    ImGui::Separator();

    auto parts = lod_item_index_ != -1 ? &std::get<1>(actor_info_[lod_item_index_]) : nullptr;

    {
        static auto getter = [](void* data, int idx, const char** out_text) {
            auto p = reinterpret_cast<decltype(parts)>(data);
            *out_text = std::get<0>((*p)[idx]).c_str();
            return true;
        };

        const int combo_size = parts ? static_cast<int>(parts->size()) : 0;
        part_item_index_ = (std::min)(part_item_index_, combo_size - 1);

        if (ImGui::Combo("Parts", &part_item_index_, getter, parts, combo_size))
        {
            anim_item_index_ = 0;
        }
    }

    PartBundle* part_bundle = part_item_index_ != -1 ? std::get<1>((*parts)[part_item_index_]) : nullptr;
    auto anims = part_item_index_ != -1 ? &std::get<2>((*parts)[part_item_index_]) : nullptr;

    // part bundle
    if (!part_bundle)
        ImGui::SetNextTreeNodeOpen(false);
    if (ImGui::CollapsingHeader("Part Bundle") && part_bundle)
    {
        {
            static const int start_enum_value = int(PartBundle::BlendType::BT_linear);

            static auto getter = [](void* data, int idx, const char** out_text) {
                const auto& cache_list = *reinterpret_cast<decltype(blend_type_cache_list_)*>(data);
                *out_text = cache_list[idx].c_str();
                return true;
            };

            int item_current = static_cast<int>(part_bundle->get_blend_type()) - start_enum_value;
            if (ImGui::Combo("Format", &item_current, getter, &blend_type_cache_list_, static_cast<int>(blend_type_cache_list_.size())))
            {
                part_bundle->set_blend_type(static_cast<PartBundle::BlendType>(item_current + start_enum_value));
            }

            bool anim_blend_flag = part_bundle->get_anim_blend_flag();
            if (ImGui::Checkbox("Anim Blend Flag", &anim_blend_flag))
                part_bundle->set_anim_blend_flag(anim_blend_flag);

            bool frame_blend_flag = part_bundle->get_frame_blend_flag();
            if (ImGui::Checkbox("Frame Blend Flag", &frame_blend_flag))
                part_bundle->set_frame_blend_flag(frame_blend_flag);
        }
    }

    ImGui::Separator();

    {
        static auto getter = [](void* data, int idx, const char** out_text) {
            auto p = reinterpret_cast<decltype(anims)>(data);
            *out_text = std::get<0>((*p)[idx]).c_str();
            return true;
        };

        const int combo_size = anims ? static_cast<int>(anims->size()) : 0;
        anim_item_index_ = (std::min)(anim_item_index_, combo_size - 1);

        ImGui::Combo("Anims", &anim_item_index_, getter, anims, combo_size);
    }

    std::string* filename = anim_item_index_ != -1 ? &std::get<1>((*anims)[anim_item_index_]) : nullptr;
    AnimControl* anim_control = anim_item_index_ != -1 ? std::get<2>((*anims)[anim_item_index_]) : nullptr;

    ImGui::LabelText("Filename", "%s", filename ? filename->c_str() : "");

    if (!anim_control)
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
    const bool clicked = ImGui::Button("Open Anim Control");
    if (!anim_control)
        ImGui::PopStyleColor();

    if (clicked && anim_control)
        AnimControlWindow::create_window(plugin_, pipeline_, anim_control);
}

void ActorWindow::set_actor(rppanda::Actor* actor)
{
    actor_ = actor;
    if (!actor_)
    {
        actor_info_.clear();
        return;
    }

    lod_item_index_ = 0;
    part_item_index_ = 0;
    anim_item_index_ = 0;

    actor_updated();
}

void ActorWindow::ui_load_animation()
{
    const auto& accepted = load_animation_dialog_->draw();
    if (!(accepted && *accepted))
        return;

    const auto& fname = load_animation_dialog_->get_filename();
    if (!fname.empty())
    {
        try
        {
            actor_->load_anims({ { load_animation_dialog_->get_animation_name(), fname} });

            actor_updated();
        }
        catch (const std::exception& err)
        {
            plugin_.error(err.what());
        }
    }
}

void ActorWindow::actor_updated()
{
    actor_info_ = actor_->get_actor_info();
}

}
