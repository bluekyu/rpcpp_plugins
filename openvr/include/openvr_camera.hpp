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

#include <string>
#include <vector>

#include <openvr.h>

class Texture;

namespace rpplugins {

class OpenVRPlugin;

class OpenVRCamera
{
public:
    OpenVRCamera(OpenVRPlugin& plugin);
    ~OpenVRCamera();

    /** Start streaming service. */
    virtual bool acquire_video_streaming_service();

    /** Stop streaming service. */
    virtual void release_video_streaming_service();

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
    virtual uint32_t get_frame_size(uint32_t& width, uint32_t& height,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_Undistorted) const;

    /**
     * Get header of current frame.
     *
     * @param[out]  header      Header of current frame.
     * @parma[in]   frame_type  Type of frame.
     * @return      Error result.
     */
    virtual vr::EVRTrackedCameraError get_frame_header(vr::CameraVideoStreamFrameHeader_t& header,
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
    virtual vr::EVRTrackedCameraError get_framebuffer(vr::CameraVideoStreamFrameHeader_t& header, std::vector<uint8_t>& buffer,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_Undistorted);

    virtual vr::EVRTrackedCameraError get_stream_texture_size(vr::VRTextureBounds_t& bound,
        uint32_t& width, uint32_t& height, vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_MaximumUndistorted);

    virtual vr::EVRTrackedCameraError get_stream_texture(vr::glUInt_t& texture_id, vr::CameraVideoStreamFrameHeader_t& header,
        vr::EVRTrackedCameraFrameType frame_type=vr::VRTrackedCameraFrameType_MaximumUndistorted);

    /** Get firmware string. */
    virtual std::string get_firmware_description() const;

    virtual vr::IVRTrackedCamera* get_vr_tracked_camera() const;
    virtual vr::TrackedCameraHandle_t get_tracked_camera_handle() const;

private:
    OpenVRPlugin& plugin_;
    vr::IVRTrackedCamera* camera_instance_ = nullptr;
    vr::TrackedCameraHandle_t camera_handle_ = INVALID_TRACKED_CAMERA_HANDLE;
};

}
