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

#include <callbackObject.h>

#include <openvr.h>

namespace rpcore {
class RenderTarget;
}

namespace rpplugins {

class SubmitCallback : public CallbackObject
{
public:
    SubmitCallback(rpcore::RenderTarget* left, rpcore::RenderTarget* right);

    void do_callback(CallbackData* cbdata) override;

    ALLOC_DELETED_CHAIN(SubmitCallback);

private:
    GraphicsStateGuardian * gsg_;
    const rpcore::RenderTarget* left_;
    const rpcore::RenderTarget* right_;

public:
    static TypeHandle get_class_type() { return _type_handle; }
    static void init_type()
    {
        CallbackObject::init_type();
        register_type(_type_handle, "rpplugins::SubmitCallback", CallbackObject::get_class_type());
    }
    TypeHandle get_type() const override { return get_class_type(); }
    TypeHandle force_init_type() override { init_type(); return get_class_type(); }

private:
    static TypeHandle _type_handle;
};

// ************************************************************************************************

class OpenVRRenderStage : public rpcore::RenderStage
{
public:
    OpenVRRenderStage(rpcore::RenderPipeline& pipeline): RenderStage(pipeline, "OpenVRRenderStage") {}

    RequireType& get_required_inputs() const final { return required_inputs_; }
    RequireType& get_required_pipes() const final { return required_pipes_; }

    void create() final;
    void reload_shaders() final;

    void set_dimensions() final;

private:
    std::string get_plugin_id() const final;

    static RequireType required_inputs_;
    static RequireType required_pipes_;

    rpcore::RenderTarget* target_left_ = nullptr;
    rpcore::RenderTarget* target_right_ = nullptr;
};

}
