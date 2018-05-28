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

#include <asyncTask.h>

#include <render_pipeline/rppanda/showbase/direct_object.hpp>
#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>

class Texture;

struct ImGuiContext;

namespace rppanda {
class FunctionalTask;
}

namespace rpplugins {

class ImGuiPlugin : public rpcore::BasePlugin, public rppanda::DirectObject
{
public:
    static const char* NEW_FRAME_EVENT_NAME;

public:
    ImGuiPlugin(rpcore::RenderPipeline& pipeline);
    
    virtual ~ImGuiPlugin();

    ALLOC_DELETED_CHAIN(ImGuiPlugin);

    RequrieType& get_required_plugins(void) const override;

    RENDER_PIPELINE_PLUGIN_DOWNCAST_DECL();

    ImGuiContext* get_context() const;
    NodePath get_root() const;

private:
    void on_pipeline_created() override;
    void on_window_resized() override;
    void on_unload() override;

    void setup_geom();
    void setup_font();
    void setup_shader();
    void setup_event();

    AsyncTask::DoneStatus new_frame(rppanda::FunctionalTask* task);
    void update(rppanda::FunctionalTask* task);
    void render(rppanda::FunctionalTask* task);

    ImGuiContext* context_ = nullptr;

    NodePath root_;
    NodePath geom_np_;
    PT(Texture) font_texture_;

    static RequrieType require_plugins_;
};

// ************************************************************************************************

inline ImGuiContext* ImGuiPlugin::get_context() const
{
    return context_;
}

inline NodePath ImGuiPlugin::get_root() const
{
    return root_;
}

}