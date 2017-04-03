#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "line_3d_impl.hpp"

namespace rpplugins {

class LineRendererPlugin: public rpcore::BasePlugin
{
public:
#if _MSC_VER >= 1900
    using BasePlugin::BasePlugin;
#else
    LineRendererPlugin(rpcore::RenderPipeline* pipeline, const PluginInfo& plugin_info);
#endif

    virtual std::shared_ptr<Line3D> create_line(const LPoint3& start_point, const LPoint3& end_point, const LVecBase3& normal) = 0;
};

}
