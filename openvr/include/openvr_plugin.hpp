#pragma once

#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>

#include <openvr.h>

namespace rpplugins {

class OpenVRPlugin: public rpcore::BasePlugin
{
public:
    OpenVRPlugin(rpcore::RenderPipeline& pipeline);
    ~OpenVRPlugin(void) final;

    RequrieType& get_required_plugins(void) const final;

    void on_load(void) final;
    void on_stage_setup(void) final;
    void on_post_render_update(void) final;

    virtual vr::IVRSystem* hmd_instance(void) const;

    virtual NodePath render_model(int device_index) const;

    virtual const vr::TrackedDevicePose_t& tracked_device_pose(int device_index) const;

    virtual uint32_t render_width(void) const;
    virtual uint32_t render_height(void) const;

    virtual std::string driver_string(void) const;
    virtual std::string display_string(void) const;

    virtual void set_distance_scale(float distance_scale);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
