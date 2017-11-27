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

#include "openvr_camera.hpp"

#include <spdlog/fmt/ostr.h>

#include "openvr_plugin.hpp"

namespace rpplugins {

OpenVRCamera::OpenVRCamera(OpenVRPlugin& plugin) : plugin_(plugin)
{
    // Accessing the FW description is just a further check to ensure camera communication is valid as expected.
    std::string firmware_desc = get_firmware_description();
    if (firmware_desc.empty())
        throw std::runtime_error("Cannot create OpenVR tracked camera.");

    plugin_.debug(fmt::format("Camera Firmware: {}", firmware_desc));

    camera_instance_ = vr::VRTrackedCamera();
}

OpenVRCamera::~OpenVRCamera()
{
    release_video_streaming_service();
}

bool OpenVRCamera::acquire_video_streaming_service()
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

void OpenVRCamera::release_video_streaming_service()
{
    if (camera_instance_ && camera_handle_)
    {
        plugin_.debug("Release video streaming service.");

        camera_instance_->ReleaseVideoStreamingService(camera_handle_);
        camera_handle_ = INVALID_TRACKED_CAMERA_HANDLE;
    }
}

uint32_t OpenVRCamera::get_frame_size(uint32_t& width, uint32_t& height, vr::EVRTrackedCameraFrameType frame_type) const
{
    uint32_t framebuffer_size = 0;
    if (camera_instance_->GetCameraFrameSize(vr::k_unTrackedDeviceIndex_Hmd, frame_type,
        &width, &height, &framebuffer_size) != vr::VRTrackedCameraError_None)
    {
        plugin_.error("get_frame_size() Failed!");
        return 0;
    }
    return framebuffer_size;
}

vr::EVRTrackedCameraError OpenVRCamera::get_frame_header(vr::CameraVideoStreamFrameHeader_t& header, vr::EVRTrackedCameraFrameType frame_type) const
{
    return camera_instance_->GetVideoStreamFrameBuffer(camera_handle_,
        frame_type, nullptr, 0, &header, sizeof(header));
}

vr::EVRTrackedCameraError OpenVRCamera::get_framebuffer(vr::CameraVideoStreamFrameHeader_t& header,
    std::vector<uint8_t>& buffer, vr::EVRTrackedCameraFrameType frame_type)
{
    return camera_instance_->GetVideoStreamFrameBuffer(camera_handle_, frame_type,
        buffer.data(), buffer.size(), &header, sizeof(header));
}

std::string OpenVRCamera::get_firmware_description() const
{
    std::string desc;
    if (!plugin_.get_tracked_device_property(desc, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_CameraFirmwareDescription_String))
        return "";
    return desc;
}

}
