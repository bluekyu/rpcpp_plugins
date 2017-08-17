/**
 * MIT License
 * 
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence
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

#include "plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <render_pipeline/rpcore/loader.hpp>

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    RPCPP_PLUGIN_ID_STRING,
    "Line Renderer",
    "yukim <yukim@chic.re.kr>",
    "0.1 alpha",

    "This plugin draw 3D lines."
};

}

namespace rpplugins {

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline* pipeline)
{
    return std::make_shared<rpplugins::Plugin>(pipeline);
}
BOOST_DLL_ALIAS(rpplugins::create_plugin, create_plugin)

// ************************************************************************************************

#if _MSC_VER < 1900
LineRendererPlugin::LineRendererPlugin(rpcore::RenderPipeline* pipeline, const PluginInfo& plugin_info): BasePlugin(pipeline, plugin_info)
{
}
#endif

Plugin::RequrieType Plugin::require_plugins;

Plugin::Plugin(rpcore::RenderPipeline* pipeline): LineRendererPlugin(pipeline, plugin_info)
{
}

void Plugin::on_stage_setup()
{
    line_render_stage_ = std::make_shared<LineRenderStage>(_pipeline);
    add_stage(line_render_stage_);
}

void Plugin::on_pipeline_created()
{
}

std::shared_ptr<Line3D> Plugin::create_line(const LPoint3& start_point, const LPoint3& end_point, const LVecBase3& normal)
{
    auto line = std::make_shared<Line3DImpl>(start_point, end_point, normal);

    return std::dynamic_pointer_cast<Line3D>(line);
}

}
