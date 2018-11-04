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

#include <render_pipeline/rpcore/render_stage.hpp>

#include <graphicsOutput.h>

namespace rpplugins {

class RecordingStage : public rpcore::RenderStage
{
public:
    RecordingStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "RecordingStage") {}
    virtual ~RecordingStage();

    RequireType& get_required_inputs() const override { return required_inputs_; }
    RequireType& get_required_pipes() const override { return required_pipes_; }

    RENDER_PIPELINE_STAGE_DOWNCAST();

    void create() override;

    void reload_shaders() final;
    void set_dimensions() final;

    virtual void set_show_through_texture(Texture* tex);

    virtual rpcore::RenderTarget* make_recording_target(
        const std::string& target_name,
        Texture* source_texture,
        GraphicsOutput::RenderTextureMode rtmode = GraphicsOutput::RenderTextureMode::RTM_copy_ram);

private:
    std::string get_plugin_id() const override;

    void reload_recording_target_shader(size_t index);

    static RequireType required_inputs_;
    static RequireType required_pipes_;

    bool stereo_mode_ = false;

    rpcore::RenderTarget* show_through_target_;
    std::vector<std::pair<rpcore::RenderTarget*, Texture*>> recording_targets_;
};

}
