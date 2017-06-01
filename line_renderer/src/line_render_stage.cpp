#include "line_render_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>
#include <render_pipeline/rpcore/globals.hpp>

namespace rpplugins {

LineRenderStage::RequireType LineRenderStage::required_inputs;
LineRenderStage::RequireType LineRenderStage::required_pipes;

LineRenderStage::ProduceType LineRenderStage::get_produced_pipes(void) const
{
    return {
    };
}

void LineRenderStage::create(void)
{
    stereo_mode_ = pipeline_->get_setting<bool>("pipeline.stereo_mode");
}

std::string LineRenderStage::get_plugin_id(void) const
{
    return RPCPP_PLUGIN_ID_STRING;
}

}
