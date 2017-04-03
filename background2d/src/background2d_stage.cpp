#include "background2d_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/render_target.h>

struct Background2DStage::Impl
{
    static RequireType required_inputs;
    static RequireType required_pipes;

    bool stereo_mode_ = false;

    std::shared_ptr<rpcore::RenderTarget> target_;
};

Background2DStage::RequireType Background2DStage::Impl::required_inputs;
Background2DStage::RequireType Background2DStage::Impl::required_pipes = { "GBuffer", "ShadedScene" };

Background2DStage::Background2DStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "Background2DStage"), impl_(new Impl)
{
}

Background2DStage::RequireType& Background2DStage::get_required_inputs(void) const
{
    return Impl::required_inputs;
}

Background2DStage::RequireType& Background2DStage::get_required_pipes(void) const
{
    return Impl::required_pipes;
}

Background2DStage::ProduceType Background2DStage::get_produced_pipes(void) const
{
    return {
        new ShaderInput("ShadedScene", impl_->target_->get_color_tex()),
    };
}

void Background2DStage::create(void)
{
    impl_->stereo_mode_ = pipeline_.get_setting<bool>("pipeline.stereo_mode");

    impl_->target_ = create_target("Background2D");
    impl_->target_->add_color_attachment(16);
    if (impl_->stereo_mode_)
        impl_->target_->set_layers(2);
    impl_->target_->prepare_buffer();

    PT(Texture) tex = new Texture("BackgroundTex-Empty");
    tex->setup_2d_texture(1, 1, Texture::ComponentType::T_byte, Texture::Format::F_rgb8);
    set_enable(false);
    set_background(tex);
}

void Background2DStage::reload_shaders(void)
{
    impl_->target_->set_shader(load_plugin_shader({"background2d.frag.glsl"}, impl_->stereo_mode_));
}

void Background2DStage::set_enable(bool enable)
{
    impl_->target_->set_shader_input(new ShaderInput("enabled", LVecBase4i(enable, 0, 0, 0)));
}

void Background2DStage::set_background(Texture* tex)
{
    impl_->target_->set_shader_input(new ShaderInput("background_tex", tex));
}

std::string Background2DStage::get_plugin_id(void) const
{
    return PLUGIN_ID_STRING;
}
