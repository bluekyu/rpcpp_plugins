/**
 * MIT License
 *
 * Copyright (c) 2017 Center of Human-centered Interaction for Coexistence
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

#include "rpplugins/openvr/camera_interface.hpp"

#include <matrixLens.h>

#include <fmt/format.h>

#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

namespace rpplugins {

PT(Camera) OpenVRCameraInterface::create_camera_node(uint32_t camera_index, const std::string& name,
    const LVecBase2& near_far, bool use_openvr_projection,
    vr::EVRTrackedCameraFrameType frame_type) const
{
    PT(Camera) cam;
    if (use_openvr_projection)
        cam = new Camera(name, new MatrixLens);
    else
        cam = new Camera(name, new PerspectiveLens);

    if (update_camera_node(cam, camera_index, near_far, frame_type))
        return cam;
    else
        return nullptr;
}

bool OpenVRCameraInterface::update_camera_node(Camera* cam, uint32_t camera_index, const LVecBase2& near_far,
    vr::EVRTrackedCameraFrameType frame_type) const
{
    const auto vr_lens = rpcore::Globals::base->get_cam_lens();
    LVecBase2 vr_near_far;
    vr_near_far[0] = near_far[0] <= 0 ? vr_lens->get_near() : near_far[0];
    vr_near_far[1] = near_far[1] <= 0 ? vr_lens->get_far() : near_far[1];

    auto cam_lens = cam->get_lens();

    if (cam_lens->is_of_type(MatrixLens::get_class_type()))
    {
        LMatrix4 proj_mat;
        if (get_projection(camera_index, vr_near_far, proj_mat, frame_type) != vr::VRTrackedCameraError_None)
        {
            plugin_.error("Failed to get projection matrix.");
            return false;
        }

        auto lens = DCAST(MatrixLens, cam_lens);

        // OpenGL film (NDC) is [-1, 1] on zero origin.
        lens->set_film_size(2, 2);
        lens->set_user_mat(
            LMatrix4::z_to_y_up_mat() * proj_mat);
    }
    else if (cam_lens->is_of_type(PerspectiveLens::get_class_type()))
    {
        uint32_t width;
        uint32_t height;
        uint32_t buffer_size;
        if (get_frame_size(width, height, buffer_size, frame_type) != vr::VRTrackedCameraError_None)
        {
            plugin_.error("Failed to get camera frame size.");
            return false;
        }

        LVecBase2 focal_length;
        LVecBase2 center;
        if (get_intrinsics(camera_index, focal_length, center, frame_type) != vr::VRTrackedCameraError_None)
        {
            plugin_.error("Failed to get camera intrinsic.");
            return false;
        }

        if (focal_length[0] != focal_length[1])
            plugin_.warn("X and Y of focal length are NOT same.");

        auto lens = DCAST(PerspectiveLens, cam_lens);
        lens->set_near_far(vr_near_far[0], vr_near_far[1]);
        lens->set_film_size(width, height);
        lens->set_focal_length(focal_length[0]);
        lens->set_film_offset(width / 2.0f - center[0], center[1] - height / 2.0f);
    }
    else
    {
        plugin_.error(fmt::format("Unknown camera type: {}", cam_lens->get_type().get_name()));
        return false;
    }

    return true;
}

}
