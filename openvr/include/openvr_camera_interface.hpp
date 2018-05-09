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

#include <camera.h>

#include <openvr_plugin.hpp>

namespace rpplugins {

class OpenVRCameraInterface
{
public:
    OpenVRCameraInterface(OpenVRPlugin& plugin);
    OpenVRCameraInterface(const OpenVRCameraInterface&) = delete;

    ~OpenVRCameraInterface();

    OpenVRCameraInterface& operator=(const OpenVRCameraInterface&) = delete;

    /** Start streaming service. */
    bool acquire_video_streaming_service();

    /** Stop streaming service. */
    void release_video_streaming_service();

    /**
     * Get framebuffer size and the width and height of frame.
     *
     * This function can be used without service start.
     *
     * @param[out]  width       The width of frame.
     * @param[out]  height      The height of frame.
     * @parma[in]   frame_type  Type of frame.
     * @return      The size of framebuffer in bytes.
     */
    vr::EVRTrackedCameraError get_frame_size(uint32_t& width, uint32_t& height, uint32_t& buffer_size,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_Undistorted) const;

    vr::EVRTrackedCameraError get_intrinsics(LVecBase2f& focal_length, LVecBase2f& center,
        vr::EVRTrackedCameraFrameType frame_type = vr::VRTrackedCameraFrameType_Undistorted) const;

    vr::EVRTrackedCameraError get_projection(const LVecBase2f& near_far, LMatrix4f& projection_matrix,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_Undistorted) const;

    /**
     * Get header of current frame.
     *
     * @param[out]  header      Header of current frame.
     * @parma[in]   frame_type  Type of frame.
     * @return      Error result.
     */
    vr::EVRTrackedCameraError get_frame_header(vr::CameraVideoStreamFrameHeader_t& header,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_Undistorted) const;

    /**
     * Get data of current framebuffer.
     *
     * The buffer will not be resized. You should resize buffer using OpenVRCameraInterface::get_frame_size().
     *
     * @param[out]  header      Header of current frame.
     * @param[out]  buffer      Buffer of current frame.
     * @parma[in]   frame_type  Type of frame.
     * @return      Error result.
     */
    vr::EVRTrackedCameraError get_framebuffer(vr::CameraVideoStreamFrameHeader_t& header, std::vector<uint8_t>& buffer,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_Undistorted) const;

    vr::EVRTrackedCameraError get_stream_texture_size(vr::VRTextureBounds_t& bound,
        uint32_t& width, uint32_t& height, vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_MaximumUndistorted) const;

    vr::EVRTrackedCameraError get_stream_texture(vr::glUInt_t& texture_id, vr::CameraVideoStreamFrameHeader_t& header,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_MaximumUndistorted) const;

    /** Get firmware string. */
    std::string get_firmware_description() const;

    vr::IVRTrackedCamera* get_vr_tracked_camera() const;
    vr::TrackedCameraHandle_t get_tracked_camera_handle() const;

    virtual PT(Camera) create_camera_node(const std::string& name="openvr_tracked_camera",
        const LVecBase2f& near_far=LVecBase2f(0),
        bool use_openvr_projection=true,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_Undistorted) const;

    virtual bool update_camera_node(Camera* cam, const LVecBase2f& near_far = LVecBase2f(0),
        vr::EVRTrackedCameraFrameType frame_type = vr::VRTrackedCameraFrameType_Undistorted) const;

private:
    OpenVRPlugin& plugin_;
    vr::IVRTrackedCamera* camera_instance_;
    vr::TrackedCameraHandle_t camera_handle_ = INVALID_TRACKED_CAMERA_HANDLE;
};

// ************************************************************************************************

inline OpenVRCameraInterface::OpenVRCameraInterface(OpenVRPlugin& plugin) : plugin_(plugin)
{
    // Accessing the FW description is just a further check to ensure camera communication is valid as expected.
    std::string firmware_desc = get_firmware_description();
    if (firmware_desc.empty())
        throw std::runtime_error("Cannot create OpenVR tracked camera.");

    plugin_.debug("OpenVR Camera Firmware: " + firmware_desc);

    camera_instance_ = vr::VRTrackedCamera();
}

inline OpenVRCameraInterface::~OpenVRCameraInterface()
{
    release_video_streaming_service();
}

inline bool OpenVRCameraInterface::acquire_video_streaming_service()
{
    plugin_.debug("Requesting to acquire video streaming service ...");

    if (camera_handle_)
    {
        plugin_.warn("Video streaming service was started already.");
    }
    else
    {
        camera_instance_->AcquireVideoStreamingService(vr::k_unTrackedDeviceIndex_Hmd, &camera_handle_);
        if (camera_handle_ == INVALID_TRACKED_CAMERA_HANDLE)
        {
            plugin_.error("acquire_video_streaming_service() failed!");
            return false;
        }
    }

    return true;
}

inline void OpenVRCameraInterface::release_video_streaming_service()
{
    if (camera_instance_ && camera_handle_)
    {
        plugin_.debug("Release video streaming service.");

        camera_instance_->ReleaseVideoStreamingService(camera_handle_);
        camera_handle_ = INVALID_TRACKED_CAMERA_HANDLE;
    }
}

inline vr::EVRTrackedCameraError OpenVRCameraInterface::get_frame_size(uint32_t& width, uint32_t& height, uint32_t& buffer_size, vr::EVRTrackedCameraFrameType frame_type) const
{
    return camera_instance_->GetCameraFrameSize(vr::k_unTrackedDeviceIndex_Hmd, frame_type,
        &width, &height, &buffer_size);
}

inline vr::EVRTrackedCameraError OpenVRCameraInterface::get_intrinsics(LVecBase2f& focal_length, LVecBase2f& center,
    vr::EVRTrackedCameraFrameType frame_type) const
{
    vr::HmdVector2_t f;
    vr::HmdVector2_t c;
    auto err = camera_instance_->GetCameraIntrinsics(vr::k_unTrackedDeviceIndex_Hmd, frame_type, &f, &c);
    if (err == vr::VRTrackedCameraError_None)
    {
        focal_length.set(f.v[0], f.v[1]);
        center.set(c.v[0], c.v[1]);
    }
    return err;
}

inline vr::EVRTrackedCameraError OpenVRCameraInterface::get_projection(const LVecBase2f& near_far, LMatrix4f& projection_matrix,
    vr::EVRTrackedCameraFrameType frame_type) const
{
    vr::HmdMatrix44_t mat;
    auto err = camera_instance_->GetCameraProjection(vr::k_unTrackedDeviceIndex_Hmd, frame_type, near_far[0], near_far[1], &mat);
    if (err == vr::VRTrackedCameraError_None)
        OpenVRPlugin::convert_matrix(mat, projection_matrix);
    return err;
}

inline vr::EVRTrackedCameraError OpenVRCameraInterface::get_frame_header(vr::CameraVideoStreamFrameHeader_t& header, vr::EVRTrackedCameraFrameType frame_type) const
{
    return camera_instance_->GetVideoStreamFrameBuffer(camera_handle_,
        frame_type, nullptr, 0, &header, sizeof(header));
}

inline vr::EVRTrackedCameraError OpenVRCameraInterface::get_framebuffer(vr::CameraVideoStreamFrameHeader_t& header,
    std::vector<uint8_t>& buffer, vr::EVRTrackedCameraFrameType frame_type) const
{
    return camera_instance_->GetVideoStreamFrameBuffer(camera_handle_, frame_type,
        buffer.data(), buffer.size(), &header, sizeof(header));
}

inline vr::EVRTrackedCameraError OpenVRCameraInterface::get_stream_texture_size(vr::VRTextureBounds_t& bound, uint32_t& width, uint32_t& height,
    vr::EVRTrackedCameraFrameType frame_type) const
{
    return camera_instance_->GetVideoStreamTextureSize(vr::k_unTrackedDeviceIndex_Hmd, frame_type, &bound, &width, &height);
}

inline vr::EVRTrackedCameraError OpenVRCameraInterface::get_stream_texture(vr::glUInt_t& texture_id, vr::CameraVideoStreamFrameHeader_t& header,
    vr::EVRTrackedCameraFrameType frame_type) const
{
    return camera_instance_->GetVideoStreamTextureGL(camera_handle_, frame_type, &texture_id, &header, sizeof(header));
}

inline std::string OpenVRCameraInterface::get_firmware_description() const
{
    std::string desc;
    if (!plugin_.get_tracked_device_property(desc, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_CameraFirmwareDescription_String))
        return "";
    return desc;
}

inline vr::IVRTrackedCamera* OpenVRCameraInterface::get_vr_tracked_camera() const
{
    return camera_instance_;
}

inline vr::TrackedCameraHandle_t OpenVRCameraInterface::get_tracked_camera_handle() const
{
    return camera_handle_;
}

}
