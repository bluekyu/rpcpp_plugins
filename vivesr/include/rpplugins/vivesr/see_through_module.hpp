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

#include <luse.h>

#include <ViveSR_API.h>
#include <ViveSR_Enums.h>

namespace rpplugins {

class ViveSRPlugin;

class ViveSRSeeThroughModule
{
public:
    static constexpr ViveSR::SRModule_TYPE module_type = ViveSR::SRModule_TYPE::ENGINE_SEETHROUGH;

    using BufferType = std::vector<unsigned char>;
    struct FrameData
    {
        BufferType frame_left;
        BufferType frame_right;
        unsigned int frame_sequence;
        unsigned int time_stamp;
        LMatrix4f pose_left;
        LMatrix4f pose_right;
    };

public:
    ViveSRSeeThroughModule(ViveSRPlugin& plugin);
    ~ViveSRSeeThroughModule();

    void start();
    void link(int to, int mode);

    constexpr int get_id() const;

    std::tuple<int, int, int> get_frame_size(bool undistorted) const;

    virtual void register_callback(bool undistorted);
    virtual void unregister_callback(bool undistorted);
    bool is_callback_registered(bool undistorted) const;

    FrameData* get_frame_data(bool undistorted) const;

private:
    static void distorted_callback(int key);
    static void undistorted_callback(int key);

    void callback(int key, ViveSR::SeeThrough::DataMask left_data_mask);

    ViveSRPlugin& plugin_;
    int id_;

    std::unique_ptr<FrameData> distorted_frame_data_;
    std::unique_ptr<FrameData> undistorted_frame_data_;
};

// ************************************************************************************************

inline constexpr int ViveSRSeeThroughModule::get_id() const
{
    return id_;
}

inline std::tuple<int, int, int> ViveSRSeeThroughModule::get_frame_size(bool undistorted) const
{
    int width;
    int height;
    int channel;

    ViveSR_GetParameterInt(
        id_,
        undistorted ? ViveSR::SeeThrough::Param::OUTPUT_UNDISTORTED_WIDTH : ViveSR::SeeThrough::Param::OUTPUT_DISTORTED_WIDTH,
        &width);

    ViveSR_GetParameterInt(
        id_,
        undistorted ? ViveSR::SeeThrough::Param::OUTPUT_UNDISTORTED_HEIGHT : ViveSR::SeeThrough::Param::OUTPUT_DISTORTED_HEIGHT,
        &height);

    ViveSR_GetParameterInt(
        id_,
        undistorted ? ViveSR::SeeThrough::Param::OUTPUT_UNDISTORTED_CHANNEL : ViveSR::SeeThrough::Param::OUTPUT_DISTORTED_CHANNEL,
        &channel);

    return { width, height, channel };
}

inline bool ViveSRSeeThroughModule::is_callback_registered(bool undistorted) const
{
    return static_cast<bool>(undistorted ? undistorted_frame_data_ : distorted_frame_data_);
}

inline auto ViveSRSeeThroughModule::get_frame_data(bool undistorted) const -> FrameData*
{
    return undistorted ? undistorted_frame_data_.get() : distorted_frame_data_.get();
}

}
