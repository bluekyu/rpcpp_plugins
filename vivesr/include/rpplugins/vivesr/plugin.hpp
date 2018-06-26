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
#include <render_pipeline/rppanda/showbase/direct_object.hpp>

namespace rpplugins {

class ViveSRPlugin : public rpcore::BasePlugin, public rppanda::DirectObject
{
public:
    enum CameraIndex : int
    {
        LEFT_CAMERA = 0,
        RIGHT_CAMERA,

        CAMERA_COUNT
    };

    enum FrameType : int
    {
        DISTORTED_FRAME = 0,
        UNDISTORTED_FRAME,
    };

public:
    ViveSRPlugin(rpcore::RenderPipeline& pipeline);
    virtual ~ViveSRPlugin();

    ALLOC_DELETED_CHAIN(ViveSRPlugin);

    RequrieType& get_required_plugins() const final;
    RENDER_PIPELINE_PLUGIN_DOWNCAST();

    void on_load() final;
    void on_unload() final;

    virtual void register_callback(FrameType frame_type);
    virtual void unregister_callback(FrameType frame_type);
    virtual bool is_callback_registered(FrameType frame_type) const;
    virtual std::array<Texture*, CAMERA_COUNT> get_textures(FrameType frame_type) const;

private:
    static RequrieType require_plugins_;

    class Impl;
    std::unique_ptr<Impl> impl_;
};

}
