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

#include "background2d_stage.hpp"

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/render_target.hpp>

class Background2DStage::Impl
{
public:
    static RequireType required_inputs;
    static RequireType required_pipes;

    bool stereo_mode_ = false;

    rpcore::RenderTarget* target_;
};

Background2DStage::RequireType Background2DStage::Impl::required_inputs;
Background2DStage::RequireType Background2DStage::Impl::required_pipes = { "GBuffer", "ShadedScene" };

Background2DStage::Background2DStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "Background2DStage"), impl_(std::make_unique<Impl>())
{
}

Background2DStage::~Background2DStage() = default;

Background2DStage::RequireType& Background2DStage::get_required_inputs() const
{
    return Impl::required_inputs;
}

Background2DStage::RequireType& Background2DStage::get_required_pipes() const
{
    return Impl::required_pipes;
}

Background2DStage::ProduceType Background2DStage::get_produced_pipes() const
{
    return {
        ShaderInput("ShadedScene", impl_->target_->get_color_tex()),
    };
}

void Background2DStage::create()
{
    impl_->stereo_mode_ = pipeline_.is_stereo_mode();

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

void Background2DStage::reload_shaders()
{
    impl_->target_->set_shader(load_plugin_shader({"background2d.frag.glsl"}, impl_->stereo_mode_));
}

void Background2DStage::set_enable(bool enable)
{
    impl_->target_->set_shader_input(ShaderInput("enabled", LVecBase4i(enable, 0, 0, 0)));
}

void Background2DStage::set_background(Texture* tex)
{
    impl_->target_->set_shader_input(ShaderInput("background_tex", tex));
}

std::string Background2DStage::get_plugin_id() const
{
    return RPPLUGIN_ID_STRING;
}
