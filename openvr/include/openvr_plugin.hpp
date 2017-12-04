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

class OpenVRCamera;

class OpenVRPlugin : public rpcore::BasePlugin
{
public:
    static LMatrix4f OpenVRPlugin::convert_matrix(const vr::HmdMatrix34_t& from);
    static LMatrix4f OpenVRPlugin::convert_matrix(const vr::HmdMatrix44_t& from);
    static void OpenVRPlugin::convert_matrix(const vr::HmdMatrix34_t& from, LMatrix4f& to);
    static void OpenVRPlugin::convert_matrix(const vr::HmdMatrix44_t& from, LMatrix4f& to);
    static void OpenVRPlugin::convert_matrix(const LMatrix4f& from, vr::HmdMatrix34_t& to);
    static void OpenVRPlugin::convert_matrix(const LMatrix4f& from, vr::HmdMatrix44_t& to);

public:
    OpenVRPlugin(rpcore::RenderPipeline& pipeline);
    ~OpenVRPlugin() final;

    RequrieType& get_required_plugins() const final;

    void on_load() final;
    void on_stage_setup() final;
    void on_post_render_update() final;

    virtual vr::IVRSystem* hmd_instance() const;
    virtual vr::IVRScreenshots* screenshots_instance() const;

    virtual NodePath setup_device_node(vr::TrackedDeviceIndex_t unTrackedDeviceIndex);
    virtual NodePath setup_render_model(vr::TrackedDeviceIndex_t unTrackedDeviceIndex);
    virtual NodePath get_device_node_group() const;
    virtual NodePath get_device_node(vr::TrackedDeviceIndex_t device_index) const;

    virtual const vr::TrackedDevicePose_t& tracked_device_pose(vr::TrackedDeviceIndex_t device_index) const;

    virtual uint32_t render_width() const;
    virtual uint32_t render_height() const;

    virtual float get_distance_scale() const;
    virtual void set_distance_scale(float distance_scale);

    virtual bool has_tracked_camera() const;

    /**
     * Get instance for tracking camera in HMD.
     */
    virtual OpenVRCamera* get_tracked_camera();

    virtual bool get_tracked_device_property(std::string& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const;
    virtual bool get_tracked_device_property(bool& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const;
    virtual bool get_tracked_device_property(int32_t& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const;
    virtual bool get_tracked_device_property(uint64_t& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const;
    virtual bool get_tracked_device_property(float& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const;
    virtual bool get_tracked_device_property(vr::HmdMatrix34_t& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const;

    /**
     * Take stereo screenshots.
     *
     * Generate a preview image (left eye image) and a stereo image as PNG.
     * The paths should not empty and the parent directory of the paths should exist.
     *
     * @param   preview_file_path   The file path of preview image (left eye image) without extension.
     * @param   vr_file_path        The file path of stereo image without extension.
     */
    virtual vr::EVRScreenshotError take_stereo_screenshots(const Filename& preview_file_path, const Filename& vr_file_path) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// ************************************************************************************************

inline LMatrix4f OpenVRPlugin::convert_matrix(const vr::HmdMatrix34_t& from)
{
    return LMatrix4f(
        from.m[0][0], from.m[1][0], from.m[2][0], 0.0,
        from.m[0][1], from.m[1][1], from.m[2][1], 0.0,
        from.m[0][2], from.m[1][2], from.m[2][2], 0.0,
        from.m[0][3], from.m[1][3], from.m[2][3], 1.0f
    );
}

inline LMatrix4f OpenVRPlugin::convert_matrix(const vr::HmdMatrix44_t& from)
{
    return LMatrix4f(
        from.m[0][0], from.m[1][0], from.m[2][0], from.m[3][0],
        from.m[0][1], from.m[1][1], from.m[2][1], from.m[3][1],
        from.m[0][2], from.m[1][2], from.m[2][2], from.m[3][2],
        from.m[0][3], from.m[1][3], from.m[2][3], from.m[3][3]
    );
}

inline void OpenVRPlugin::convert_matrix(const vr::HmdMatrix34_t& from, LMatrix4f& to)
{
    to.set(
        from.m[0][0], from.m[1][0], from.m[2][0], 0.0,
        from.m[0][1], from.m[1][1], from.m[2][1], 0.0,
        from.m[0][2], from.m[1][2], from.m[2][2], 0.0,
        from.m[0][3], from.m[1][3], from.m[2][3], 1.0f
    );
}

inline void OpenVRPlugin::convert_matrix(const vr::HmdMatrix44_t& from, LMatrix4f& to)
{
    to.set(
        from.m[0][0], from.m[1][0], from.m[2][0], from.m[3][0],
        from.m[0][1], from.m[1][1], from.m[2][1], from.m[3][1],
        from.m[0][2], from.m[1][2], from.m[2][2], from.m[3][2],
        from.m[0][3], from.m[1][3], from.m[2][3], from.m[3][3]
    );
}

inline void OpenVRPlugin::convert_matrix(const LMatrix4f& from, vr::HmdMatrix34_t& to)
{
    to = vr::HmdMatrix34_t{ {
        { from(0, 0), from(1, 0), from(2, 0), from(3, 0) },
        { from(0, 1), from(1, 1), from(2, 1), from(3, 1) },
        { from(0, 2), from(1, 2), from(2, 2), from(3, 2) },
        } };
}

inline void OpenVRPlugin::convert_matrix(const LMatrix4f& from, vr::HmdMatrix44_t& to)
{
    to = vr::HmdMatrix44_t{ {
        { from(0, 0), from(1, 0), from(2, 0), from(3, 0) },
        { from(0, 1), from(1, 1), from(2, 1), from(3, 1) },
        { from(0, 2), from(1, 2), from(2, 2), from(3, 2) },
        { from(0, 3), from(1, 3), from(2, 3), from(3, 3) },
        } };
}

}
