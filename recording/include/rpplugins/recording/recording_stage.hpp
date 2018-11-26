/**
 * MIT License
 *
 * Copyright (c) 2018 Center of Human-centered Interaction for Coexistence
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

#include <tuple>

#include <render_pipeline/rpcore/render_stage.hpp>

#include <graphicsOutput.h>
#include <filename.h>

#include <boost/optional.hpp>

namespace rpplugins {

class RecordingStage : public rpcore::RenderStage
{
public:
    RecordingStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "RecordingStage") {}
    ~RecordingStage() override;

    RequireType& get_required_inputs() const override { return required_inputs_; }
    RequireType& get_required_pipes() const override { return required_pipes_; }

    RENDER_PIPELINE_STAGE_DOWNCAST();

    void create() override;

    void reload_shaders() final;
    void set_dimensions() final;

    virtual void set_show_through_texture(Texture* tex);

    /**
     * Recording basic 2D or stereo (2DArray) texture.
     */
    virtual rpcore::RenderTarget* make_recording_target(
        const std::string& target_name,
        Texture* source_texture,
        GraphicsOutput::RenderTextureMode rtmode = GraphicsOutput::RenderTextureMode::RTM_copy_ram,
        const Filename& fragment_shader_path = Filename());

    /**
     * Recording general texture to single 2D texture.
     */
    virtual rpcore::RenderTarget* make_recording_target_as_mono(
        const std::string& target_name,
        Texture* source_texture,
        int layer,
        GraphicsOutput::RenderTextureMode rtmode = GraphicsOutput::RenderTextureMode::RTM_copy_ram,
        const Filename& fragment_shader_path = Filename());

private:
    std::string get_plugin_id() const override;

    rpcore::RenderTarget* setup_recording_target(
        const std::string& target_name,
        Texture* source_texture,
        GraphicsOutput::RenderTextureMode rtmode,
        const Filename& fragment_shader_path);

    void reload_recording_target_shader(size_t index);

    static RequireType required_inputs_;
    static RequireType required_pipes_;

    bool stereo_mode_ = false;

    rpcore::RenderTarget* show_through_target_;

    struct TargetInfo
    {
        rpcore::RenderTarget* target;
        Texture* source_texture;
        Filename shader_path;
        boost::optional<int> layer;
    };
    std::vector<TargetInfo> recording_targets_;
};

}
