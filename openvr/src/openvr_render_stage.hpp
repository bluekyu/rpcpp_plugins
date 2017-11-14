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

#pragma once

#include <render_pipeline/rpcore/render_stage.hpp>

#include <openvr.h>

namespace rpcore {
class RenderTarget;
}

namespace rpplugins {

class OpenvrRenderStage : public rpcore::RenderStage
{
public:
    OpenvrRenderStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "OpenvrRenderStage") {}

    RequireType& get_required_inputs() const final { return required_inputs_; }
    RequireType& get_required_pipes() const final { return required_pipes_; }

    void create() final;
    void reload_shaders() final;

    void set_render_target_size(int width, int height);
    uint64_t get_eye_texture(vr::EVREye vr_eye);

private:
    std::string get_plugin_id() const final;

    static RequireType required_inputs_;
    static RequireType required_pipes_;

    std::shared_ptr<rpcore::RenderTarget> target_left_ = nullptr;
    std::shared_ptr<rpcore::RenderTarget> target_right_ = nullptr;

    LVecBase2i render_target_size_;
};

// ************************************************************************************************
inline void OpenvrRenderStage::set_render_target_size(int width, int height)
{
    render_target_size_ = LVecBase2i(width, height);
}

}    // namespace rpplugins
