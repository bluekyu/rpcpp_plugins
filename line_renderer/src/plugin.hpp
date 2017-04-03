#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "../include/line_renderer_plugin.hpp"
#include "line_render_stage.hpp"

namespace rpplugins {

class Plugin: public LineRendererPlugin
{
public:
    Plugin(rpcore::RenderPipeline* pipeline);

    RequrieType& get_required_plugins(void) const override { return require_plugins; }

    void on_stage_setup(void) override;
    void on_pipeline_created(void) override;

    std::shared_ptr<Line3D> create_line(const LPoint3& start_point, const LPoint3& end_point, const LVecBase3& normal) override;

private:
    static RequrieType require_plugins;

    std::shared_ptr<LineRenderStage> line_render_stage_;
};

}
