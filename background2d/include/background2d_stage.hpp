#pragma once

#include <render_pipeline/rpcore/render_stage.h>

class Background2DStage: public rpcore::RenderStage
{
public:
    Background2DStage(rpcore::RenderPipeline& pipeline);

    RequireType& get_required_inputs(void) const override;
    RequireType& get_required_pipes(void) const override;
    ProduceType get_produced_pipes(void) const;

    void create(void) override;
    void reload_shaders(void) override;

    virtual void set_enable(bool enable);
    virtual void set_background(Texture* tex);

private:
    std::string get_plugin_id(void) const override;

    struct Impl;
    std::unique_ptr<Impl> impl_;
};
