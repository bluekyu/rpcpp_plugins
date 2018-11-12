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

#include <memory>

#include <boost/dll/alias.hpp>

#include <rpplugins/rpstat/gui_interface.hpp>

#include <rpplugins/openvr/plugin.hpp>

namespace rpplugins {

class OpenVRPluginGUI : public GUIInterface
{
public:
    OpenVRPluginGUI(rpcore::RenderPipeline& pipeline);
    virtual ~OpenVRPluginGUI() = default;

    void on_draw_new_frame() override;

private:
    OpenVRPlugin* plugin_;
};

// ************************************************************************************************

OpenVRPluginGUI::OpenVRPluginGUI(rpcore::RenderPipeline& pipeline): GUIInterface(pipeline, RPPLUGINS_GUI_ID_STRING)
{
    plugin_ = static_cast<decltype(plugin_)>(pipeline_.get_plugin_mgr()->get_instance(RPPLUGINS_GUI_ID_STRING)->downcast());
}

void OpenVRPluginGUI::on_draw_new_frame()
{
    ImGui::Begin("OpenVR Plugin Window");

    ImGui::Text("Test");

    ImGui::End();
}

}

RPPLUGINS_GUI_CREATOR(rpplugins::OpenVRPluginGUI)
