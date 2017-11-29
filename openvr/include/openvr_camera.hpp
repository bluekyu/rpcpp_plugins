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

#include <openvr_plugin.hpp>

namespace rpplugins {

class OpenVRCamera
{
public:
    OpenVRCamera(OpenVRPlugin& plugin);
    OpenVRCamera(const OpenVRCamera&) = delete;

    ~OpenVRCamera();

    OpenVRCamera& operator=(const OpenVRCamera&) = delete;

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

    vr::EVRTrackedCameraError get_projection(float near_distance, float far_distance, LMatrix4f& projection_matrix,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_Undistorted);

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
     * The buffer will not be resized. You should resize buffer using OpenVRCamera::get_frame_size().
     *
     * @param[out]  header      Header of current frame.
     * @param[out]  buffer      Buffer of current frame.
     * @parma[in]   frame_type  Type of frame.
     * @return      Error result.
     */
    vr::EVRTrackedCameraError get_framebuffer(vr::CameraVideoStreamFrameHeader_t& header, std::vector<uint8_t>& buffer,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_Undistorted);

    vr::EVRTrackedCameraError get_stream_texture_size(vr::VRTextureBounds_t& bound,
        uint32_t& width, uint32_t& height, vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_MaximumUndistorted);

    vr::EVRTrackedCameraError get_stream_texture(vr::glUInt_t& texture_id, vr::CameraVideoStreamFrameHeader_t& header,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_MaximumUndistorted);

    /** Get firmware string. */
    std::string get_firmware_description() const;

    vr::IVRTrackedCamera* get_vr_tracked_camera() const;
    vr::TrackedCameraHandle_t get_tracked_camera_handle() const;

private:
    OpenVRPlugin& plugin_;
    vr::IVRTrackedCamera* camera_instance_;
    vr::TrackedCameraHandle_t camera_handle_ = INVALID_TRACKED_CAMERA_HANDLE;
};

// ************************************************************************************************

inline OpenVRCamera::OpenVRCamera(OpenVRPlugin& plugin) : plugin_(plugin)
{
    // Accessing the FW description is just a further check to ensure camera communication is valid as expected.
    std::string firmware_desc = get_firmware_description();
    if (firmware_desc.empty())
        throw std::runtime_error("Cannot create OpenVR tracked camera.");

    plugin_.debug("Camera Firmware: " + firmware_desc);

    camera_instance_ = vr::VRTrackedCamera();
}

inline OpenVRCamera::~OpenVRCamera()
{
    release_video_streaming_service();
}

inline bool OpenVRCamera::acquire_video_streaming_service()
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

inline void OpenVRCamera::release_video_streaming_service()
{
    if (camera_instance_ && camera_handle_)
    {
        plugin_.debug("Release video streaming service.");

        camera_instance_->ReleaseVideoStreamingService(camera_handle_);
        camera_handle_ = INVALID_TRACKED_CAMERA_HANDLE;
    }
}

inline vr::EVRTrackedCameraError OpenVRCamera::get_frame_size(uint32_t& width, uint32_t& height, uint32_t& buffer_size, vr::EVRTrackedCameraFrameType frame_type) const
{
    return camera_instance_->GetCameraFrameSize(vr::k_unTrackedDeviceIndex_Hmd, frame_type,
        &width, &height, &buffer_size);
}

inline vr::EVRTrackedCameraError OpenVRCamera::get_projection(float near_distance, float far_distance, LMatrix4f& projection_matrix,
    vr::EVRTrackedCameraFrameType frame_type)
{
    vr::HmdMatrix44_t mat;
    auto err = camera_instance_->GetCameraProjection(vr::k_unTrackedDeviceIndex_Hmd, frame_type, near_distance, far_distance, &mat);
    if (err == vr::VRTrackedCameraError_None)
        OpenVRPlugin::convert_matrix(mat, projection_matrix);
    return err;
}

inline vr::EVRTrackedCameraError OpenVRCamera::get_frame_header(vr::CameraVideoStreamFrameHeader_t& header, vr::EVRTrackedCameraFrameType frame_type) const
{
    return camera_instance_->GetVideoStreamFrameBuffer(camera_handle_,
        frame_type, nullptr, 0, &header, sizeof(header));
}

inline vr::EVRTrackedCameraError OpenVRCamera::get_framebuffer(vr::CameraVideoStreamFrameHeader_t& header,
    std::vector<uint8_t>& buffer, vr::EVRTrackedCameraFrameType frame_type)
{
    return camera_instance_->GetVideoStreamFrameBuffer(camera_handle_, frame_type,
        buffer.data(), buffer.size(), &header, sizeof(header));
}

inline vr::EVRTrackedCameraError OpenVRCamera::get_stream_texture_size(vr::VRTextureBounds_t& bound, uint32_t& width, uint32_t& height,
    vr::EVRTrackedCameraFrameType frame_type)
{
    return camera_instance_->GetVideoStreamTextureSize(vr::k_unTrackedDeviceIndex_Hmd, frame_type, &bound, &width, &height);
}

inline vr::EVRTrackedCameraError OpenVRCamera::get_stream_texture(vr::glUInt_t& texture_id, vr::CameraVideoStreamFrameHeader_t& header,
    vr::EVRTrackedCameraFrameType frame_type)
{
    return camera_instance_->GetVideoStreamTextureGL(camera_handle_, frame_type, &texture_id, &header, sizeof(header));
}

inline std::string OpenVRCamera::get_firmware_description() const
{
    std::string desc;
    if (!plugin_.get_tracked_device_property(desc, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_CameraFirmwareDescription_String))
        return "";
    return desc;
}

inline vr::IVRTrackedCamera* OpenVRCamera::get_vr_tracked_camera() const
{
    return camera_instance_;
}

inline vr::TrackedCameraHandle_t OpenVRCamera::get_tracked_camera_handle() const
{
    return camera_handle_;
}

}
