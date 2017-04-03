#include "plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <render_pipeline/rpcore/loader.h>

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "rendering",
    PLUGIN_ID_STRING,
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

void Plugin::on_stage_setup(void)
{
	line_render_stage_ = std::make_shared<LineRenderStage>(_pipeline);
	add_stage(line_render_stage_);
}

void Plugin::on_pipeline_created(void)
{
}

std::shared_ptr<Line3D> Plugin::create_line(const LPoint3& start_point, const LPoint3& end_point, const LVecBase3& normal)
{
    auto line = std::make_shared<Line3DImpl>(start_point, end_point, normal);

    return std::dynamic_pointer_cast<Line3D>(line);
}

}
