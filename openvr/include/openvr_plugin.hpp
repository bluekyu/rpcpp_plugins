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

#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>

#include <openvr.h>

namespace rpplugins {

class OpenVRPlugin: public rpcore::BasePlugin
{
public:
    OpenVRPlugin(rpcore::RenderPipeline& pipeline);
    ~OpenVRPlugin() final;

    RequrieType& get_required_plugins() const final;

    void on_load() final;
    void on_stage_setup() final;
    void on_post_render_update() final;

    virtual vr::IVRSystem* hmd_instance() const;

    virtual NodePath render_model(int device_index) const;

    virtual const vr::TrackedDevicePose_t& tracked_device_pose(int device_index) const;

    virtual uint32_t render_width() const;
    virtual uint32_t render_height() const;

    virtual std::string driver_string() const;
    virtual std::string display_string() const;

    virtual void set_distance_scale(float distance_scale);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
