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

#include <ViveSR_Enums.h>

#include "rpplugins/vivesr/see_through_module.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::ViveSRPlugin)

namespace rpplugins {

class ViveSRPlugin::Impl
{
public:
    using BufferContainerType = std::array<std::vector<unsigned char>, CAMERA_COUNT>;

public:
    void on_load(ViveSRPlugin& self);
    void on_unload(ViveSRPlugin& self);

public:
    std::unique_ptr<ViveSRSeeThroughModule> seethrough_module_;
    int id_depth_;
    int id_rigid_reconstruction_;

    CameraParams camera_params_;
};

// ************************************************************************************************

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

    seethrough_module_ = std::make_unique<ViveSRSeeThroughModule>(self);

    printf("ViveSR_CreateModule %d\n", ViveSR_CreateModule(ViveSR::SRModule_TYPE::ENGINE_DEPTH, &id_depth_));
    printf("ViveSR_CreateModule %d\n", ViveSR_CreateModule(ViveSR::SRModule_TYPE::ENGINE_RIGID_RECONSTRUCTION, &id_rigid_reconstruction_));

    ViveSR_GetCameraParams(&camera_params_);

    ViveSR_SetParameterString(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::CONFIG_FILEPATH, config_filepath);
    ViveSR_SetParameterFloat(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::FOCAL_LENGTH, static_cast<float>(camera_params_.FocalLength_L));
    ViveSR_SetParameterInt(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::CONFIG_QUALITY, 3);
    ViveSR_SetParameterInt(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::MESH_REFRESH_INTERVAL, 300);
    ViveSR_SetParameterBool(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::ENABLE_FRUSTUM_CULLING, true);
    ViveSR_SetParameterFloat(id_rigid_reconstruction_, ViveSR::RigidReconstruction::Param::SECTOR_SIZE, 0.8);

    seethrough_module_->start();

    int res = ViveSR_StartModule(id_depth_);
    if (res != ViveSR::SR_Error::WORK)
        self.error(fmt::format("ViveSR_StartModule Depth ({}) failed ({})", id_depth_, res));

    res = ViveSR_StartModule(id_rigid_reconstruction_);
    if (res != ViveSR::SR_Error::WORK)
        self.error(fmt::format("ViveSR_StartModule Rigid Reconstruction ({}) failed ({})", id_rigid_reconstruction_, res));

    seethrough_module_->link(id_depth_, ViveSR::SRWorks_Link_Method::SR_ACTIVE);

    res = ViveSR_ModuleLink(id_depth_, id_rigid_reconstruction_, ViveSR::SRWorks_Link_Method::SR_ACTIVE);
    if (res != ViveSR::SR_Error::WORK)
        self.error(fmt::format("ViveSRs_link Depth ({}) to Rigid Reconstrunction ({}) failed ({})", id_depth_, id_rigid_reconstruction_, res));

    if (res != ViveSR::SR_Error::WORK)
        self.error("ViveSR_Start failed");
}

void ViveSRPlugin::Impl::on_unload(ViveSRPlugin& self)
{
    ViveSR_Stop();

    seethrough_module_.reset();
}

// ************************************************************************************************

ViveSRPlugin::RequrieType ViveSRPlugin::require_plugins_;

ViveSRPlugin::ViveSRPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING),
    impl_(std::make_unique<Impl>())
{
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
    impl_->on_unload(*this);
}

const CameraParams& ViveSRPlugin::get_camera_params() const
{
    return impl_->camera_params_;
}

ViveSRSeeThroughModule* ViveSRPlugin::get_seethrough_module() const
{
    return impl_->seethrough_module_.get();
}

}
