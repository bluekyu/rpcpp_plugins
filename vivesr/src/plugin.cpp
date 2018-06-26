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

#include "rpplugins/vivesr/plugin.hpp"

#include <boost/any.hpp>
#include <boost/dll/alias.hpp>

#include <fmt/ostream.h>

#include <render_pipeline/rpcore/render_pipeline.hpp>

#include <ViveSR_API.h>
#include <ViveSR_Enums.h>

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::ViveSRPlugin)

namespace rpplugins {

class ViveSRPlugin::Impl
{
public:
    using BufferContainerType = std::array<std::vector<unsigned char>, CAMERA_COUNT>;

    class DataType
    {
    public:
        DataType();
        ~DataType();

    public:
        BufferContainerType buffers_;
        std::array<PT(Texture), CAMERA_COUNT> textures_;

        rppanda::FunctionalTask* task_ = nullptr;
    };

public:
    static void distorted_callback(int key);
    static void undistorted_callback(int key);

    void initialize(ViveSRPlugin& self);

    void on_load(ViveSRPlugin& self);

    void create_buffer(FrameType frame_index);

    void upload_texture(FrameType frame_index);

public:
    static std::unordered_map<FrameType, DataType> data_;

    int id_seethrough_;
    int id_depth_;
    int id_rigid_reconstruction_;

    CameraParams camera_params_;
};

ViveSRPlugin::Impl::DataType::DataType()
{
    for (int cam_index = 0; cam_index < CAMERA_COUNT; ++cam_index)
        textures_[cam_index] = new Texture("vivesr-texture-" + std::to_string(cam_index));
}

ViveSRPlugin::Impl::DataType::~DataType()
{
    if (task_)
        task_->remove();
}

// ************************************************************************************************

std::unordered_map<ViveSRPlugin::FrameType, ViveSRPlugin::Impl::DataType> ViveSRPlugin::Impl::data_;

void ViveSRPlugin::Impl::distorted_callback(int key)
{
    auto& buffer = data_[DISTORTED_FRAME].buffers_;

    unsigned char* ptr_l;
    unsigned char* ptr_r;

    ViveSR_GetPointer(key, ViveSR::SeeThrough::DataMask::DISTORTED_FRAME_LEFT, (void**)&ptr_l);
    ViveSR_GetPointer(key, ViveSR::SeeThrough::DataMask::DISTORTED_FRAME_RIGHT, (void**)&ptr_r);

    std::copy(ptr_l, ptr_l + buffer[LEFT_CAMERA].size(), buffer[LEFT_CAMERA].begin());
    std::copy(ptr_r, ptr_r + buffer[RIGHT_CAMERA].size(), buffer[RIGHT_CAMERA].begin());
}

void ViveSRPlugin::Impl::undistorted_callback(int key)
{
    auto& buffer = data_[UNDISTORTED_FRAME].buffers_;

    unsigned char* ptr_l;
    unsigned char* ptr_r;

    ViveSR_GetPointer(key, ViveSR::SeeThrough::DataMask::UNDISTORTED_FRAME_LEFT, (void**)&ptr_l);
    ViveSR_GetPointer(key, ViveSR::SeeThrough::DataMask::UNDISTORTED_FRAME_RIGHT, (void**)&ptr_r);

    std::copy(ptr_l, ptr_l + buffer[LEFT_CAMERA].size(), buffer[LEFT_CAMERA].begin());
    std::copy(ptr_r, ptr_r + buffer[RIGHT_CAMERA].size(), buffer[RIGHT_CAMERA].begin());
}

void ViveSRPlugin::Impl::initialize(ViveSRPlugin& self)
{
}

void ViveSRPlugin::Impl::on_load(ViveSRPlugin& self)
{
    char config_filepath[] = "ViveSR_Config.txt";

    ViveSR_Initial();

    ViveSR_EnableLog(boost::any_cast<bool>(self.get_setting("enable_log")));

    std::string log_level = boost::any_cast<std::string>(self.get_setting("log_level"));
    if (log_level == "1")
        ViveSR_SetLogLevel(ViveSR::SR_LogLevel::SRLOG_LEVEL_1);
    else if (log_level == "2")
        ViveSR_SetLogLevel(ViveSR::SR_LogLevel::SRLOG_LEVEL_2);
    else if (log_level == "max")
        ViveSR_SetLogLevel(ViveSR::SR_LogLevel::SRLOG_LEVEL_MAX);
    else
        ViveSR_SetLogLevel(ViveSR::SR_LogLevel::SRLOG_LEVEL_0);

    printf("ViveSR_CreateModule %d\n", ViveSR_CreateModule(ViveSR::SRModule_TYPE::ENGINE_SEETHROUGH, &id_seethrough_));
    printf("ViveSR_CreateModule %d\n", ViveSR_CreateModule(ViveSR::SRModule_TYPE::ENGINE_DEPTH, &id_depth_));
    printf("ViveSR_CreateModule %d\n", ViveSR_CreateModule(ViveSR::SRModule_TYPE::ENGINE_RIGID_RECONSTRUCTION, &id_rigid_reconstruction_));

    ViveSR_GetCameraParams(&camera_params_);

    ViveSR_SetParameterBool(id_seethrough_, ViveSR::SeeThrough::Param::VR_INIT, false);
    ViveSR_SetParameterInt(id_seethrough_, ViveSR::SeeThrough::Param::VR_INIT_TYPE, ViveSR::SeeThrough::InitType::SCENE);

    ViveSR_SetParameterString(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::CONFIG_FILEPATH, config_filepath);
    ViveSR_SetParameterFloat(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::FOCAL_LENGTH, static_cast<float>(camera_params_.FocalLength_L));
    ViveSR_SetParameterInt(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::CONFIG_QUALITY, 3);
    ViveSR_SetParameterInt(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::MESH_REFRESH_INTERVAL, 300);
    ViveSR_SetParameterBool(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::ENABLE_FRUSTUM_CULLING, true);
    ViveSR_SetParameterFloat(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::SECTOR_SIZE, 0.8);

    int res = ViveSR_StartModule(id_seethrough_);
    if (res != ViveSR::SR_Error::WORK)
        self.error(fmt::format("ViveSR_StartModule Seethrough ({}) failed ({})", id_seethrough_, res));

    res = ViveSR_StartModule(id_depth_);
    if (res != ViveSR::SR_Error::WORK)
        self.error(fmt::format("ViveSR_StartModule Depth ({}) failed ({})", id_depth_, res));

    res = ViveSR_StartModule(id_rigid_reconstruction_);
    if (res != ViveSR::SR_Error::WORK)
        self.error(fmt::format("ViveSR_StartModule Rigid Reconstruction ({}) failed ({})", id_rigid_reconstruction_, res));

    res = ViveSR_ModuleLink(id_seethrough_, id_depth_, ViveSR::SRWorks_Link_Method::SR_ACTIVE);
    if (res != ViveSR::SR_Error::WORK)
        self.error(fmt::format("ViveSRs_link Seethrough ({}) to Depth ({}) failed ({})", id_seethrough_, id_depth_, res));

    res = ViveSR_ModuleLink(id_depth_, id_rigid_reconstruction_, ViveSR::SRWorks_Link_Method::SR_ACTIVE);
    if (res != ViveSR::SR_Error::WORK)
        self.error(fmt::format("ViveSRs_link Depth ({}) to Rigid Reconstrunction ({}) failed ({})", id_depth_, id_rigid_reconstruction_, res));

    if (res != ViveSR::SR_Error::WORK)
        self.error("ViveSR_Start failed");
}

void ViveSRPlugin::Impl::create_buffer(FrameType frame_type)
{
    auto& data = data_[frame_type];

    if (data.task_)
        return;

    int width;
    int height;
    int channel;
    Texture::Format tex_format;
    Texture::ComponentType comp;

    switch (frame_type)
    {
    case DISTORTED_FRAME:
        ViveSR_GetParameterInt(id_seethrough_, ViveSR::SeeThrough::Param::OUTPUT_DISTORTED_WIDTH, &width);
        ViveSR_GetParameterInt(id_seethrough_, ViveSR::SeeThrough::Param::OUTPUT_DISTORTED_HEIGHT, &height);
        ViveSR_GetParameterInt(id_seethrough_, ViveSR::SeeThrough::Param::OUTPUT_DISTORTED_CHANNEL, &channel);
        tex_format = channel == 3 ? Texture::Format::F_srgb : Texture::Format::F_srgb_alpha;
        comp = Texture::ComponentType::T_unsigned_byte;
        break;

    case UNDISTORTED_FRAME:
        ViveSR_GetParameterInt(id_seethrough_, ViveSR::SeeThrough::Param::OUTPUT_UNDISTORTED_WIDTH, &width);
        ViveSR_GetParameterInt(id_seethrough_, ViveSR::SeeThrough::Param::OUTPUT_UNDISTORTED_HEIGHT, &height);
        ViveSR_GetParameterInt(id_seethrough_, ViveSR::SeeThrough::Param::OUTPUT_UNDISTORTED_CHANNEL, &channel);
        tex_format = channel == 3 ? Texture::Format::F_srgb : Texture::Format::F_srgb_alpha;
        comp = Texture::ComponentType::T_unsigned_byte;
        break;
    }

    for (int cam_index = 0; cam_index < CAMERA_COUNT; ++cam_index)
    {
        data.textures_[cam_index]->setup_2d_texture(width, height, comp, tex_format);
        data.buffers_[cam_index].resize(width * height * channel);
    }
}

void ViveSRPlugin::Impl::upload_texture(FrameType frame_type)
{
    auto& data = data_[frame_type];

    const auto tex_format = data.textures_[0]->get_format();

    switch (tex_format)
    {
        case Texture::Format::F_srgb:
        case Texture::Format::F_rgb:
        {
            for (int cam_index = 0; cam_index < CAMERA_COUNT; ++cam_index)
            {
                PTA_uchar ram_image = data.textures_[cam_index]->modify_ram_image();
                auto& buffer = data.buffers_[cam_index];

                auto dest = ram_image.p();
                for (size_t k = 0, k_end = buffer.size(); k < k_end; k += 3)
                {
                    // bgra = rgba
                    dest[k + 2] = buffer[k + 0];
                    dest[k + 1] = buffer[k + 1];
                    dest[k + 0] = buffer[k + 2];
                }
            }
            break;
        }

        case Texture::Format::F_srgb_alpha:
        case Texture::Format::F_rgba:
        {
            for (int cam_index = 0; cam_index < CAMERA_COUNT; ++cam_index)
            {
                PTA_uchar ram_image = data.textures_[cam_index]->modify_ram_image();
                auto& buffer = data.buffers_[cam_index];

                auto dest = ram_image.p();
                for (size_t k = 0, k_end = buffer.size(); k < k_end; k += 4)
                {
                    // bgra = rgba
                    dest[k + 2] = buffer[k + 0];
                    dest[k + 1] = buffer[k + 1];
                    dest[k + 0] = buffer[k + 2];
                    dest[k + 3] = buffer[k + 3];
                }
            }
            break;
        }

        default:
            break;
    }
}

// ************************************************************************************************

ViveSRPlugin::RequrieType ViveSRPlugin::require_plugins_;

ViveSRPlugin::ViveSRPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING),
    impl_(std::make_unique<Impl>())
{
    impl_->initialize(*this);
}

ViveSRPlugin::~ViveSRPlugin() = default;

ViveSRPlugin::RequrieType& ViveSRPlugin::get_required_plugins() const
{
    return require_plugins_;
}

void ViveSRPlugin::on_load()
{
    impl_->on_load(*this);
}

void ViveSRPlugin::on_unload()
{
    impl_->data_.clear();
    ViveSR_Stop();
}

void ViveSRPlugin::register_callback(FrameType frame_type)
{
    auto& data = impl_->data_[frame_type];

    if (data.task_)
        return;

    impl_->create_buffer(frame_type);

    switch (frame_type)
    {
    case DISTORTED_FRAME:
        ViveSR_RegisterCallback(impl_->id_seethrough_, ViveSR::SeeThrough::Callback::BASIC, Impl::distorted_callback);
        break;

    case UNDISTORTED_FRAME:
        ViveSR_RegisterCallback(impl_->id_seethrough_, ViveSR::SeeThrough::Callback::BASIC, Impl::undistorted_callback);
        break;
    }

    impl_->data_[frame_type].task_ = add_task([this, frame_type](rppanda::FunctionalTask*) {
        impl_->upload_texture(frame_type);
        return AsyncTask::DS_cont;
    }, "distorted_task");
}

void ViveSRPlugin::unregister_callback(FrameType frame_type)
{
    auto& data = impl_->data_[frame_type];

    if (!data.task_)
        return;

    data.task_->remove();
    data.task_ = nullptr;

    switch (frame_type)
    {
    case DISTORTED_FRAME:
        ViveSR_UnregisterCallback(impl_->id_seethrough_, ViveSR::SeeThrough::Callback::BASIC, Impl::distorted_callback);
        break;

    case UNDISTORTED_FRAME:
        ViveSR_UnregisterCallback(impl_->id_seethrough_, ViveSR::SeeThrough::Callback::BASIC, Impl::undistorted_callback);
        break;
    }
}

bool ViveSRPlugin::is_callback_registered(FrameType frame_type) const
{
    auto found = impl_->data_.find(frame_type);
    if (found != impl_->data_.end())
        return static_cast<bool>(found->second.task_);
    else
        return false;
}

std::array<Texture*, ViveSRPlugin::CAMERA_COUNT> ViveSRPlugin::get_textures(FrameType frame_type) const
{
    std::array<Texture*, CAMERA_COUNT> result = { nullptr };

    auto found = impl_->data_.find(frame_type);
    if (found != impl_->data_.end())
    {
        for (int cam_index = 0; cam_index < CAMERA_COUNT; ++cam_index)
            result[cam_index] = found->second.textures_[cam_index];
    }

    return result;
}

}
