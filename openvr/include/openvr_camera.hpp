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

#include <openvr.h>

namespace rpplugins {

class OpenVRPlugin;

class OpenVRCamera
{
public:
    OpenVRCamera(OpenVRPlugin& plugin);
    ~OpenVRCamera();

    virtual bool acquire_video_streaming_service();
    virtual void release_video_streaming_service();

    virtual std::string get_firmware_description() const;

private:
    OpenVRPlugin& plugin_;
    vr::IVRTrackedCamera* camera_instance_ = nullptr;
    vr::TrackedCameraHandle_t camera_handle_ = INVALID_TRACKED_CAMERA_HANDLE;
    vr::CameraVideoStreamFrameHeader_t last_camera_frame_header_;
};

}
