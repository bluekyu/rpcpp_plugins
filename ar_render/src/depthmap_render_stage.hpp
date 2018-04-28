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

#include <nodePath.h>
#include <camera.h>

#include <render_pipeline/rpcore/render_stage.hpp>

namespace rpplugins {

class DepthmapRenderStage : public rpcore::RenderStage
{
public:
    DepthmapRenderStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "DepthmapRenderStage") {}
    virtual ~DepthmapRenderStage();

    RequireType& get_required_inputs() const override { return required_inputs; }
    RequireType& get_required_pipes() const override { return required_pipes; }
    ProduceType get_produced_pipes() const override;

    void create() override;

    void set_shader_input(const ShaderInput& inp) final;

    NodePath get_camera() const;

    void set_ar_camera(const NodePath& cam);

private:
    std::string get_plugin_id() const override;

    static RequireType required_inputs;
    static RequireType required_pipes;

    bool stereo_mode_ = false;

    rpcore::RenderTarget* target_;
    PT(Camera) real_depth_cam_;
	NodePath real_depth_cam_np_;
};

}
