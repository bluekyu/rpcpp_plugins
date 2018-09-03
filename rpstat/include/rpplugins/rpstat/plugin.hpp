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

#include <render_pipeline/rppanda/showbase/direct_object.hpp>
#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>

namespace rpplugins {

class ImGuiPlugin;

class WindowInterface;

class RPStatPlugin : public rpcore::BasePlugin, public rppanda::DirectObject
{
public:
    RPStatPlugin(rpcore::RenderPipeline& pipeline);
    
    virtual ~RPStatPlugin();

    RequrieType& get_required_plugins(void) const override;

    RENDER_PIPELINE_PLUGIN_DOWNCAST();

    void on_load() override;
    void on_pipeline_created() override;

    NodePath get_copied_nodepath() const;
    void set_copied_nodepath(NodePath np);

private:
    void on_imgui_new_frame();

    void draw_main_menu_bar();
    void draw_dropped_file();

    static RequrieType require_plugins_;

    ImGuiPlugin* imgui_plugin_;

    std::vector<std::unique_ptr<WindowInterface>> windows_;

    NodePath copied_np_;

    bool file_dropped_ = false;
};

// ************************************************************************************************

inline NodePath RPStatPlugin::get_copied_nodepath() const
{
    return copied_np_;
}

inline void RPStatPlugin::set_copied_nodepath(NodePath np)
{
    copied_np_ = np;
}

}
