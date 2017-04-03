#pragma once

#include <render_pipeline/rpcore/render_stage.h>

namespace rpplugins {

class LineRenderStage: public rpcore::RenderStage
{
public:
    LineRenderStage(rpcore::RenderPipeline* pipeline): RenderStage(pipeline, "LineRenderStage") {}

    virtual RequireType& get_required_inputs(void) const { return required_inputs; }
    virtual RequireType& get_required_pipes(void) const { return required_pipes; }
    virtual ProduceType get_produced_pipes(void) const;

    virtual void create(void) override;

private:
    virtual std::string get_plugin_id(void) const;

    static RequireType required_inputs;
    static RequireType required_pipes;

    bool stereo_mode_ = false;
};

}
