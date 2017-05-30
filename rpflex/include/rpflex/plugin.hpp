/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016, Center of human-centered interaction for coexistence.
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

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

struct NvFlexLibrary;
struct NvFlexSolver;
struct NvFlexParams;

namespace rpflex {

class InstanceInterface;
struct FlexBuffer;

class Plugin: public rpcore::BasePlugin
{
public:
    struct Parameters
    {
        int substeps_count;

        float wave_floor_tilt;

        LVecBase3f scene_lower;
        LVecBase3f scene_upper;

        int max_diffuse_particles;
        unsigned char max_neighbors_per_particle;
        int num_extra_particles;
        int num_extra_multiplier;
    };

public:
    Plugin(rpcore::RenderPipeline& pipeline);
    ~Plugin(void) final;

    RequrieType& get_required_plugins(void) const final;

    void on_load(void) final;
    void on_stage_setup(void) final;
    void on_pipeline_created(void) final;
    void on_pre_render_update(void) final;
    void on_post_render_update(void) final;
    void on_unload(void) final;

    virtual void add_instance(const std::shared_ptr<InstanceInterface>& instance);

    virtual NvFlexLibrary* get_flex_library(void) const;
    virtual NvFlexSolver* get_flex_solver(void) const;

    /** Read NvFlexParams. */
    virtual const NvFlexParams& get_flex_params(void) const;

    /** Modify NvFlexParams. */
    virtual NvFlexParams& get_flex_params(void);

    virtual const Parameters& get_plugin_params(void) const;
    virtual Parameters& get_plugin_params(void);

    virtual const FlexBuffer& get_flex_buffer(void) const;
    virtual FlexBuffer& get_flex_buffer(void);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
