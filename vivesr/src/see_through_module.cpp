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

#include "rpplugins/vivesr/see_through_module.hpp"

#include <fmt/ostream.h>

#include "rpplugins/vivesr/plugin.hpp"

namespace rpplugins {

static ViveSRSeeThroughModule* global_ptr_ = nullptr;

ViveSRSeeThroughModule::ViveSRSeeThroughModule(ViveSRPlugin& plugin) : plugin_(plugin)
{
    global_ptr_ = this;

    ViveSR_CreateModule(module_type, &id_);

    ViveSR_SetParameterBool(id_, ViveSR::SeeThrough::Param::VR_INIT, false);
    ViveSR_SetParameterInt(id_, ViveSR::SeeThrough::Param::VR_INIT_TYPE, ViveSR::SeeThrough::InitType::SCENE);
}

ViveSRSeeThroughModule::~ViveSRSeeThroughModule()
{
    global_ptr_ = nullptr;
}

void ViveSRSeeThroughModule::start()
{
    int res = ViveSR_StartModule(id_);
    if (res != ViveSR::SR_Error::WORK)
        plugin_.error(fmt::format("ViveSR_StartModule Seethrough ({}) failed ({})", id_, res));
}

void ViveSRSeeThroughModule::link(int to, int mode)
{
    int res = ViveSR_ModuleLink(id_, to, mode);
    if (res != ViveSR::SR_Error::WORK)
        plugin_.error(fmt::format("ViveSRs_link Seethrough ({}) to Depth ({}) failed ({})", id_, to, res));
}

void ViveSRSeeThroughModule::register_callback(bool undistorted)
{
    const auto frame = get_frame_size(undistorted);
    const auto frame_size = std::get<0>(frame) * std::get<1>(frame) * std::get<2>(frame);

    if (undistorted)
    {
        undistorted_frame_data_ = std::make_unique<FrameData>();
        undistorted_frame_data_->frame_left.resize(frame_size);
        undistorted_frame_data_->frame_right.resize(frame_size);

        ViveSR_RegisterCallback(id_, ViveSR::SeeThrough::Callback::BASIC, undistorted_callback);
    }
    else
    {
        distorted_frame_data_ = std::make_unique<FrameData>();
        distorted_frame_data_->frame_left.resize(frame_size);
        distorted_frame_data_->frame_right.resize(frame_size);

        ViveSR_RegisterCallback(id_, ViveSR::SeeThrough::Callback::BASIC, distorted_callback);
    }
}

void ViveSRSeeThroughModule::unregister_callback(bool undistorted)
{
    if (undistorted)
    {
        ViveSR_UnregisterCallback(id_, ViveSR::SeeThrough::Callback::BASIC, undistorted_callback);
        undistorted_frame_data_.reset();
    }
    else
    {
        ViveSR_UnregisterCallback(id_, ViveSR::SeeThrough::Callback::BASIC, distorted_callback);
        distorted_frame_data_.reset();
    }
}

void ViveSRSeeThroughModule::distorted_callback(int key)
{
    global_ptr_->callback(key, ViveSR::SeeThrough::DataMask::DISTORTED_FRAME_LEFT);
}

void ViveSRSeeThroughModule::undistorted_callback(int key)
{
    global_ptr_->callback(key, ViveSR::SeeThrough::DataMask::UNDISTORTED_FRAME_LEFT);
}

void ViveSRSeeThroughModule::callback(int key, ViveSR::SeeThrough::DataMask left_data_mask)
{
    auto frame_data = left_data_mask == ViveSR::SeeThrough::DataMask::DISTORTED_FRAME_LEFT ?
        distorted_frame_data_.get() : undistorted_frame_data_.get();

    unsigned char* ptr_l;
    unsigned char* ptr_r;
    unsigned int* frame_seq;
    unsigned int* time_stp;
    float* pose_left;
    float* pose_right;

    ViveSR_GetPointer(key, left_data_mask, (void**)&ptr_l);
    ViveSR_GetPointer(key, left_data_mask + 1, (void**)&ptr_r);
    ViveSR_GetPointer(key, ViveSR::SeeThrough::DataMask::FRAME_SEQ, (void**)&frame_seq);
    ViveSR_GetPointer(key, ViveSR::SeeThrough::DataMask::TIME_STP, (void**)&time_stp);
    ViveSR_GetPointer(key, ViveSR::SeeThrough::DataMask::POSE_LEFT, (void**)&pose_left);
    ViveSR_GetPointer(key, ViveSR::SeeThrough::DataMask::POSE_RIGHT, (void**)&pose_right);

    std::copy(ptr_l, ptr_l + frame_data->frame_left.size(), frame_data->frame_left.begin());
    std::copy(ptr_r, ptr_r + frame_data->frame_right.size(), frame_data->frame_right.begin());
    frame_data->frame_sequence = *frame_seq;
    frame_data->time_stamp = *time_stp;

    std::memcpy(&frame_data->pose_left(0, 0), pose_left, LMatrix4f::num_components * sizeof(LMatrix4f::numeric_type));
    std::memcpy(&frame_data->pose_right(0, 0), pose_right, LMatrix4f::num_components * sizeof(LMatrix4f::numeric_type));
}

}
