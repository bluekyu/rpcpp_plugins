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

namespace rpplugins {

class ARCompositeStage : public rpcore::RenderStage
{
public:
    ARCompositeStage(rpcore::RenderPipeline& pipeline);
    virtual ~ARCompositeStage();

    RequireType& get_required_inputs() const override;
    RequireType& get_required_pipes() const override;
    ProduceType get_produced_pipes() const override;

    void create() override;
    void reload_shaders() override;

    virtual bool composite_disabled() const;
    virtual void disable_composite(bool set);
    virtual void render_only_valid_ar_depth(bool enable);
    virtual void remove_occlusion(bool enable);

    void set_ar_camera_color_texture(Texture* tex, bool is_bgr, bool flip_vertical);
    void set_ar_camera(const NodePath& cam);
    void set_ar_camera(const NodePath& left_cam, const NodePath& right_cam);

private:
    std::string get_plugin_id() const override;

    static RequireType required_inputs_;
    static RequireType required_pipes_;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
