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

#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/pluginbase/setting_types.hpp>

#include <rpplugins/rpstat/gui_interface.hpp>

namespace rpplugins {

class PluginGUI : public GUIInterface
{
public:
    PluginGUI(rpcore::RenderPipeline& pipeline);
    virtual ~PluginGUI() = default;

    void on_draw_menu() override;
    void on_draw_new_frame() override;

private:
    bool is_open_ = false;

    rpcore::FloatType* distance_scale_;
    rpcore::BoolType* update_camera_pose_;
    rpcore::BoolType* update_eye_pose_;
};

// ************************************************************************************************

PluginGUI::PluginGUI(rpcore::RenderPipeline& pipeline): GUIInterface(pipeline, RPPLUGINS_GUI_ID_STRING)
{
    distance_scale_ = static_cast<rpcore::FloatType*>(get_setting_handle("distance_scale")->downcast());
    update_camera_pose_ = static_cast<rpcore::BoolType*>(get_setting_handle("update_camera_pose")->downcast());
    update_eye_pose_ = static_cast<rpcore::BoolType*>(get_setting_handle("update_eye_pose")->downcast());
}

void PluginGUI::on_draw_menu()
{
    if (ImGui::MenuItem("OpenVR"))
        is_open_ = true;
}

void PluginGUI::on_draw_new_frame()
{
    if (!is_open_)
        return;

    if (!ImGui::Begin("OpenVR Plugin", &is_open_))
        return ImGui::End();

    float distance_scale = distance_scale_->get_value_as_type();
    if (ImGui::InputFloat(distance_scale_->get_label().c_str(), &distance_scale))
    {
        distance_scale_->set_value(distance_scale);
        plugin_mgr_->on_setting_changed(plugin_id_, "distance_scale");
    }

    bool update_camera_pose = update_camera_pose_->get_value_as_type();
    if (ImGui::Checkbox("update_camera_pose", &update_camera_pose))
    {
        update_camera_pose_->set_value(update_camera_pose);
        plugin_mgr_->on_setting_changed(plugin_id_, "update_camera_pose");
    }

    bool update_eye_pose = update_eye_pose_->get_value_as_type();
    if (ImGui::Checkbox("update_eye_pose", &update_eye_pose))
    {
        update_eye_pose_->set_value(update_eye_pose);
        plugin_mgr_->on_setting_changed(plugin_id_, "update_eye_pose");
    }

    ImGui::End();
}

}

RPPLUGINS_GUI_CREATOR(rpplugins::PluginGUI)
