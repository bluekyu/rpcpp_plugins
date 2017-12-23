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

#include "openvr_plugin.hpp"

#include <thread>

#include <boost/any.hpp>
#include <boost/dll/alias.hpp>

#include <spdlog/fmt/ostr.h>

#include <geomTriangles.h>
#include <geomNode.h>
#include <matrixLens.h>
#include <camera.h>
#include <material.h>
#include <materialAttrib.h>
#include <texture.h>
#include <textureAttrib.h>
#include <geomVertexWriter.h>

#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/util/filesystem.hpp>

#include "openvr_render_stage.hpp"
#include "openvr_controller.hpp"
#include "openvr_camera_interface.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::OpenVRPlugin)

namespace rpplugins {

static const LMatrix4f z_to_y = LMatrix4f::convert_mat(CS_zup_right, CS_yup_right);
static const LMatrix4f y_to_z = LMatrix4f::convert_mat(CS_yup_right, CS_zup_right);

class OpenVRPlugin::Impl
{
public:
    void on_stage_setup(OpenVRPlugin& self);

    void setup_camera();
    bool init_compositor(const OpenVRPlugin& self) const;
    void create_device_node_group();
    void setup_device_nodes(const OpenVRPlugin& self);
    NodePath setup_device_node(const OpenVRPlugin& self, vr::TrackedDeviceIndex_t unTrackedDeviceIndex);
    NodePath setup_render_model(const OpenVRPlugin& self, vr::TrackedDeviceIndex_t unTrackedDeviceIndex);
    NodePath load_model(const OpenVRPlugin& self, const std::string& model_name);
    NodePath create_mesh(const std::string& model_name, vr::RenderModel_t* render_model, vr::RenderModel_TextureMap_t* render_texture);

    void update_hmd_pose();

    std::string get_screenshot_error_message(vr::EVRScreenshotError err) const;

public:
    static RequrieType require_plugins_;

    std::shared_ptr<OpenvrRenderStage> render_stage_;
    float distance_scale_ = 1.0f;
    bool update_camera_pose_ = true;
    bool update_eye_pose_ = true;
    bool create_device_node_ = false;
    bool load_render_model_ = false;
    bool enable_rendering_ = true;

    // vive data
    vr::IVRSystem* vr_system_ = nullptr;

    vr::TrackedDevicePose_t tracked_device_pose_[vr::k_unMaxTrackedDeviceCount];

    uint32_t render_width_ = 0;
    uint32_t render_height_ = 0;

    NodePath device_node_group_;
    NodePath device_nodes_[vr::k_unMaxTrackedDeviceCount];
    NodePath controller_node_;

    std::unique_ptr<OpenVRCameraInterface> tracked_camera_;
};

// ************************************************************************************************

OpenVRPlugin::RequrieType OpenVRPlugin::Impl::require_plugins_;

void OpenVRPlugin::Impl::on_stage_setup(OpenVRPlugin& self)
{
    if (enable_rendering_)
    {
        render_stage_ = std::make_shared<OpenvrRenderStage>(self.pipeline_);
        render_stage_->set_render_target_size(render_width_, render_height_);
        self.add_stage(render_stage_);
    }

    distance_scale_ = boost::any_cast<float>(self.get_setting("distance_scale"));
    update_camera_pose_ = boost::any_cast<bool>(self.get_setting("update_camera_pose"));
    update_eye_pose_ = boost::any_cast<bool>(self.get_setting("update_eye_pose"));
    create_device_node_ = boost::any_cast<bool>(self.get_setting("create_device_node"));
    load_render_model_ = boost::any_cast<bool>(self.get_setting("load_render_model"));

    if (!init_compositor(self))
    {
        self.error("Failed to initialize VR Compositor!");
        return;
    }

    setup_device_nodes(self);

    if (boost::any_cast<bool>(self.get_setting("enable_controller")))
    {
        PT(OpenVRController) node = new OpenVRController(vr_system_);
        controller_node_ = rpcore::Globals::base->get_data_root().attach_new_node(node);
    }

    self.debug("Finish to initialize OpenVR.");
}

void OpenVRPlugin::Impl::setup_camera()
{
    if (!enable_rendering_)
        return;

    auto perspective_lens = rpcore::Globals::base->get_cam_lens();

    // create OpenVR lens and copy from original lens.
    PT(MatrixLens) vr_lens = new MatrixLens;
    *DCAST(Lens, vr_lens) = *rpcore::Globals::base->get_cam_lens();
    vr_lens->set_interocular_distance(0);
    vr_lens->set_convergence_distance(0);
    rpcore::Globals::base->get_cam_node()->set_lens(vr_lens);

    // Y-up matrix
    LMatrix4f proj_mat;

    // left
    convert_matrix(vr_system_->GetProjectionMatrix(vr::Eye_Left, vr_lens->get_near(), vr_lens->get_far()), proj_mat);

    // film size will be changed in WindowFramework::adjust_dimensions when resizing.
    // so, we need to post-multiply the inverse matrix to preserve our projection matrix.
    vr_lens->set_left_eye_mat(z_to_y * proj_mat * vr_lens->get_film_mat_inv());

    // FIXME: fix to projection matrix for stereo culling mono projection for culling
    // mono lens (see PerspectiveLens::do_compute_projection_mat)
    proj_mat(2, 0) = 0;
    proj_mat(2, 1) = 0;
    vr_lens->set_user_mat(z_to_y * proj_mat * vr_lens->get_film_mat_inv());

    // right
    convert_matrix(vr_system_->GetProjectionMatrix(vr::Eye_Right, vr_lens->get_near(), vr_lens->get_far()), proj_mat);
    vr_lens->set_right_eye_mat(z_to_y * proj_mat * vr_lens->get_film_mat_inv());
}

bool OpenVRPlugin::Impl::init_compositor(const OpenVRPlugin& self) const
{
    vr::EVRInitError peError = vr::VRInitError_None;

    if (!vr::VRCompositor())
    {
        self.error("Compositor initialization failed.");
        return false;
    }

    return true;
}

void OpenVRPlugin::Impl::create_device_node_group()
{
    if (!device_node_group_.is_empty())
        return;

    device_node_group_ = rpcore::Globals::render.attach_new_node("openvr_devices");
    device_node_group_.set_scale(distance_scale_);
}

void OpenVRPlugin::Impl::setup_device_nodes(const OpenVRPlugin& self)
{
    if (!vr_system_)
        return;

    if (load_render_model_)
    {
        setup_device_node(self, vr::k_unTrackedDeviceIndex_Hmd);
        for (vr::TrackedDeviceIndex_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; ++unTrackedDevice)
        {
            if (!vr_system_->IsTrackedDeviceConnected(unTrackedDevice))
                continue;
            setup_render_model(self, unTrackedDevice);
        }
    }
    else if (create_device_node_)
    {
        setup_device_node(self, vr::k_unTrackedDeviceIndex_Hmd);
        for (vr::TrackedDeviceIndex_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; ++unTrackedDevice)
            setup_device_node(self, unTrackedDevice);
    }
}

NodePath OpenVRPlugin::Impl::setup_device_node(const OpenVRPlugin& self, vr::TrackedDeviceIndex_t unTrackedDeviceIndex)
{
    if (unTrackedDeviceIndex >= vr::k_unMaxTrackedDeviceCount)
        return NodePath();

    create_device_node_group();

    std::string prop;
    self.get_tracked_device_property(prop, unTrackedDeviceIndex, vr::Prop_RenderModelName_String);
    device_nodes_[unTrackedDeviceIndex] = device_node_group_.attach_new_node(prop);

    self.get_tracked_device_property(prop, unTrackedDeviceIndex, vr::Prop_SerialNumber_String);
    device_nodes_[unTrackedDeviceIndex].set_tag("serial_number", prop);

    return device_nodes_[unTrackedDeviceIndex];
}

NodePath OpenVRPlugin::Impl::setup_render_model(const OpenVRPlugin& self, vr::TrackedDeviceIndex_t unTrackedDeviceIndex)
{
    if (unTrackedDeviceIndex >= vr::k_unMaxTrackedDeviceCount)
        return NodePath();

    create_device_node_group();

    // try to find a model we've already set up
    std::string model_name;
    std::string tracking_system_name;
    self.get_tracked_device_property(model_name, unTrackedDeviceIndex, vr::Prop_RenderModelName_String);
    self.get_tracked_device_property(tracking_system_name, unTrackedDeviceIndex, vr::Prop_TrackingSystemName_String);

    NodePath model = load_model(self, model_name);
    if (model.is_empty())
    {
        self.error(fmt::format("Unable to load render model for tracked device {} ({}, {})",
            unTrackedDeviceIndex, tracking_system_name, model_name));
    }
    else
    {
        model.reparent_to(device_node_group_);

        std::string serial_number;
        self.get_tracked_device_property(serial_number, unTrackedDeviceIndex, vr::Prop_SerialNumber_String);
        model.set_tag("serial_number", serial_number);

        device_nodes_[unTrackedDeviceIndex] = model;
    }

    return model;
}

NodePath OpenVRPlugin::Impl::load_model(const OpenVRPlugin& self, const std::string& model_name)
{
    vr::RenderModel_t* model = nullptr;
    vr::EVRRenderModelError model_error;
    while (1)
    {
        model_error = vr::VRRenderModels()->LoadRenderModel_Async(model_name.c_str(), &model);
        if (model_error != vr::VRRenderModelError_Loading)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (model_error != vr::VRRenderModelError_None)
    {
        self.error(fmt::format("Unable to load render model {} - {}", model_name, vr::VRRenderModels()->GetRenderModelErrorNameFromEnum(model_error)));
        return NodePath();
    }

    vr::RenderModel_TextureMap_t* texture;
    while (1)
    {
        model_error = vr::VRRenderModels()->LoadTexture_Async(model->diffuseTextureId, &texture);
        if (model_error != vr::VRRenderModelError_Loading)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (model_error != vr::VRRenderModelError_None)
    {
        self.error(fmt::format("Unable to load render texture for render model {}", model_name));
        vr::VRRenderModels()->FreeRenderModel(model);
        return NodePath();
    }

    NodePath model_np = create_mesh(model_name, model, texture);

    vr::VRRenderModels()->FreeRenderModel(model);
    vr::VRRenderModels()->FreeTexture(texture);

    return model_np;
}

NodePath OpenVRPlugin::Impl::create_mesh(const std::string& model_name, vr::RenderModel_t* render_model, vr::RenderModel_TextureMap_t* render_texture)
{
    // Add vertices
    PT(GeomVertexData) vdata = new GeomVertexData(model_name, GeomVertexFormat::get_v3n3t2(), Geom::UsageHint::UH_static);
    vdata->unclean_set_num_rows(render_model->unVertexCount);

    GeomVertexWriter vertex(vdata, InternalName::get_vertex());
    GeomVertexWriter normal(vdata, InternalName::get_normal());
    GeomVertexWriter texcoord0(vdata, InternalName::get_texcoord());

    for (uint32_t k=0, k_end=render_model->unVertexCount; k < k_end; ++k)
    {
        const auto& pos = render_model->rVertexData[k].vPosition;
        const auto& norm = render_model->rVertexData[k].vNormal;
        const auto& tc = render_model->rVertexData[k].rfTextureCoord;

        // Y-up to Z-up
        vertex.add_data3(pos.v[0], -pos.v[2], pos.v[1]);
        normal.add_data3(norm.v[0], -norm.v[2], norm.v[1]);
        texcoord0.add_data2(tc[0], tc[1]);
    }

    // Add indices
    const size_t triangle_count = render_model->unTriangleCount * 3;

    PT(GeomTriangles) prim = new GeomTriangles(Geom::UsageHint::UH_static);
    prim->reserve_num_vertices(triangle_count);
    for (size_t k = 0, k_end = triangle_count; k < k_end; k+=3)
        prim->add_vertices(render_model->rIndexData[k], render_model->rIndexData[k+1], render_model->rIndexData[k+2]);
    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    PT(GeomNode) geom_node = new GeomNode(model_name);
    geom_node->add_geom(geom);

    rpcore::RPGeomNode rpgeom_node(geom_node);

    rpcore::RPMaterial mat;
    mat.set_roughness(1);
    mat.set_specular_ior(1);

    rpgeom_node.set_material(0, mat);

    PT(Texture) texture = new Texture(model_name);
    texture->setup_2d_texture(render_texture->unWidth, render_texture->unHeight, Texture::ComponentType::T_unsigned_byte, Texture::Format::F_rgba8);

    PTA_uchar dest = texture->make_ram_image();
    const auto src = render_texture->rubTextureMapData;
    for (size_t k=0, k_end=dest.size(); k < k_end; k+=4)
    {
        dest[k+2] = src[k+0];   // r
        dest[k+1] = src[k+1];   // g
        dest[k+0] = src[k+2];   // b
        dest[k+3] = src[k+3];   // a
    }

    texture->set_wrap_u(SamplerState::WM_clamp);
    texture->set_wrap_v(SamplerState::WM_clamp);
    texture->set_magfilter(SamplerState::FT_linear);
    texture->set_minfilter(SamplerState::FT_linear_mipmap_linear);

    rpgeom_node.set_texture(0, texture);

    return NodePath(geom_node);
}

void OpenVRPlugin::Impl::update_hmd_pose()
{
    if (!vr_system_)
        return;

    vr::VRCompositor()->WaitGetPoses(tracked_device_pose_, vr::k_unMaxTrackedDeviceCount, NULL, 0);

    if (tracked_device_pose_[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
    {
        NodePath cam = rpcore::Globals::base->get_cam();

        LMatrix4f hmd_mat;
        convert_matrix(tracked_device_pose_[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking, hmd_mat);

        hmd_mat = z_to_y * hmd_mat * y_to_z;

        if (create_device_node_ || load_render_model_)
            device_nodes_[vr::k_unTrackedDeviceIndex_Hmd].set_mat(hmd_mat);

        hmd_mat[3][0] *= distance_scale_;
        hmd_mat[3][1] *= distance_scale_;
        hmd_mat[3][2] *= distance_scale_;

        if (update_camera_pose_)
            cam.set_mat(hmd_mat);

        // Update for IPD change.
        if (update_eye_pose_)
        {
            NodePath leye_np = cam.find("left_eye");
            if (leye_np)
            {
                LMatrix4f left_eye_mat;
                convert_matrix(vr_system_->GetEyeToHeadTransform(vr::Eye_Left), left_eye_mat);
                left_eye_mat[3][0] *= distance_scale_;
                left_eye_mat[3][1] *= distance_scale_;
                left_eye_mat[3][2] *= distance_scale_;
                leye_np.set_mat(z_to_y * left_eye_mat * y_to_z);
            }

            NodePath reye_np = cam.find("right_eye");
            if (reye_np)
            {
                LMatrix4f right_eye_mat;
                convert_matrix(vr_system_->GetEyeToHeadTransform(vr::Eye_Right), right_eye_mat);
                right_eye_mat[3][0] *= distance_scale_;
                right_eye_mat[3][1] *= distance_scale_;
                right_eye_mat[3][2] *= distance_scale_;
                reye_np.set_mat(z_to_y * right_eye_mat * y_to_z);
            }
        }
    }

    if (!(create_device_node_ || load_render_model_))
        return;

    for (int device_index = vr::k_unTrackedDeviceIndex_Hmd+1; device_index < vr::k_unMaxTrackedDeviceCount; ++device_index)
    {
        if (tracked_device_pose_[device_index].bPoseIsValid && !device_nodes_[device_index].is_empty())
        {
            device_nodes_[device_index].set_mat(z_to_y * 
                convert_matrix(tracked_device_pose_[device_index].mDeviceToAbsoluteTracking)
                * y_to_z);
        }
    }
}

std::string OpenVRPlugin::Impl::get_screenshot_error_message(vr::EVRScreenshotError err) const
{
    switch (err)
    {
        case vr::EVRScreenshotError::VRScreenshotError_None:
            return std::string("No error");
        case vr::VRScreenshotError_RequestFailed:
            return std::string("Failed to request");
        case vr::VRScreenshotError_IncompatibleVersion:
            return std::string("Incompatible version");
        case vr::VRScreenshotError_NotFound:
            return std::string("Not found");
        case vr::VRScreenshotError_BufferTooSmall:
            return std::string("Buffer too small");
        case vr::VRScreenshotError_ScreenshotAlreadyInProgress:
            return std::string("Screenshot already in pregress");
        default:
            return std::string("Undocumented error message");
    }
}

// ************************************************************************************************

OpenVRPlugin::OpenVRPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING),
    impl_(std::make_unique<Impl>())
{
#if defined(_WIN32)
    auto openvr_sdk_path = boost::any_cast<std::string>(get_setting("openvr_sdk_path"));
    Filename dll_path = "openvr_api";
    if (!openvr_sdk_path.empty())
    {
        Filename platform_suffix = ".";
        if (sizeof(void*) == 4)
            platform_suffix = "win32";
        else if (sizeof(void*) == 8)
            platform_suffix = "win64";
        else
            error("Unknown platform");

        dll_path = Filename(openvr_sdk_path) / "bin" / platform_suffix / dll_path;
    }

    if (!load_shared_library(dll_path))
        throw std::runtime_error("Failed to load openvr_api.dll");
#endif
}

OpenVRPlugin::~OpenVRPlugin()
{
    impl_->tracked_camera_.reset();
    for (vr::TrackedDeviceIndex_t k = 0; k < vr::k_unMaxTrackedDeviceCount; ++k)
    {
        impl_->device_nodes_[k].remove_node();
        impl_->controller_node_.remove_node();
    }
    vr::VR_Shutdown();
}

OpenVRPlugin::RequrieType& OpenVRPlugin::get_required_plugins() const 
{
    return impl_->require_plugins_;
}

void OpenVRPlugin::on_load()
{
    // Loading the SteamVR Runtime
    vr::EVRInitError eError = vr::VRInitError_None;
    impl_->vr_system_ = vr::VR_Init(&eError, vr::VRApplication_Scene);

    if (eError != vr::VRInitError_None)
    {
        impl_->vr_system_ = nullptr;
        error(fmt::format("Unable to init VR runtime: {}", vr::VR_GetVRInitErrorAsEnglishDescription(eError)));
        return;
    }

    if (!vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError))
    {
        impl_->vr_system_ = nullptr;
        vr::VR_Shutdown();

        error(fmt::format("Unable to get render model interface: {}", vr::VR_GetVRInitErrorAsEnglishDescription(eError)));
        return;
    }

    std::string data;
    if (get_tracked_device_property(data, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String))
        debug(fmt::format("Tracking System Name: {}", data));
    if (get_tracked_device_property(data, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String))
        debug(fmt::format("Serial Number: {}", data));

    impl_->enable_rendering_ = boost::any_cast<bool>(get_setting("enable_rendering"));
    if (impl_->enable_rendering_)
    {
        impl_->setup_camera();

        impl_->vr_system_->GetRecommendedRenderTargetSize(&impl_->render_width_, &impl_->render_height_);

        debug(fmt::format("OpenVR render target size: ({}, {})", impl_->render_width_, impl_->render_height_));
    }
    else
    {
        debug("OpenVR rendering is disabled.");
    }
}

void OpenVRPlugin::on_stage_setup()
{
    impl_->on_stage_setup(*this);
}

void OpenVRPlugin::on_post_render_update()
{
    if (impl_->enable_rendering_)
    {
        vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)(impl_->render_stage_->get_eye_texture(vr::Eye_Left)), vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
        vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);

        vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)(impl_->render_stage_->get_eye_texture(vr::Eye_Right)), vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
        vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
    }

    impl_->update_hmd_pose();
}

vr::IVRSystem* OpenVRPlugin::get_vr_system() const
{
    return impl_->vr_system_;
}

NodePath OpenVRPlugin::setup_device_node(vr::TrackedDeviceIndex_t unTrackedDeviceIndex)
{
    return impl_->setup_device_node(*this, unTrackedDeviceIndex);
}

NodePath OpenVRPlugin::setup_render_model(vr::TrackedDeviceIndex_t unTrackedDeviceIndex)
{
    return impl_->setup_render_model(*this, unTrackedDeviceIndex);
}

NodePath OpenVRPlugin::get_device_node_group() const
{
    return impl_->device_node_group_;
}

NodePath OpenVRPlugin::get_device_node(vr::TrackedDeviceIndex_t device_index) const
{
    if (device_index >= vr::k_unMaxTrackedDeviceCount)
        return NodePath();

    return impl_->device_nodes_[device_index];
}

uint32_t OpenVRPlugin::get_render_width() const
{
    return impl_->render_width_;
}

uint32_t OpenVRPlugin::get_render_height() const
{
    return impl_->render_height_;
}

float OpenVRPlugin::get_distance_scale() const
{
    return impl_->distance_scale_;
}

void OpenVRPlugin::set_distance_scale(float distance_scale)
{
    impl_->distance_scale_ = distance_scale;
    if (!impl_->device_node_group_.is_empty())
        impl_->device_node_group_.set_scale(impl_->distance_scale_);
}

const vr::TrackedDevicePose_t& OpenVRPlugin::get_tracked_device_pose(vr::TrackedDeviceIndex_t device_index) const
{
    if (device_index >= vr::k_unMaxTrackedDeviceCount)
    {
        static const vr::TrackedDevicePose_t invalid;
        return invalid;
    }

    return impl_->tracked_device_pose_[device_index];
}

vr::ETrackedDeviceClass OpenVRPlugin::get_tracked_device_class(vr::TrackedDeviceIndex_t device_index) const
{
    return impl_->vr_system_->GetTrackedDeviceClass(device_index);
}

bool OpenVRPlugin::has_tracked_camera() const
{
    auto tracked_camera_instance = vr::VRTrackedCamera();
    if (!tracked_camera_instance)
    {
        error("Unable to get Tracked Camera interface.");
        return false;
    }

    bool has_camera = false;
    vr::EVRTrackedCameraError camera_err = tracked_camera_instance->HasCamera(vr::k_unTrackedDeviceIndex_Hmd, &has_camera);
    if (camera_err != vr::VRTrackedCameraError_None || !has_camera)
        error(fmt::format("No Tracked Camera Available! ({})", tracked_camera_instance->GetCameraErrorNameFromEnum(camera_err)));

    return has_camera;
}

boost::optional<LVecBase2f> OpenVRPlugin::get_play_area_size() const
{
    auto chaperone = vr::VRChaperone();
    if (chaperone)
    {
        LVecBase2f area;
        if (chaperone->GetPlayAreaSize(&area[0], &area[1]))
            return area;
    }
    return {};
}

OpenVRCameraInterface* OpenVRPlugin::get_tracked_camera()
{
    if (impl_->tracked_camera_)
        return impl_->tracked_camera_.get();

    if (!has_tracked_camera())
        return nullptr;

    try
    {
        impl_->tracked_camera_ = std::make_unique<OpenVRCameraInterface>(*this);
    }
    catch (const std::runtime_error& err)
    {
        error(err.what());
        return nullptr;
    }

    return impl_->tracked_camera_.get();
}

bool OpenVRPlugin::get_tracked_device_property(std::string& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const
{
    uint32_t unRequiredBufferLen = impl_->vr_system_->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0);
    if (unRequiredBufferLen == 0)
    {
        result = "";
    }
    else
    {
        vr::ETrackedPropertyError err;
        char* pchBuffer = new char[unRequiredBufferLen];
        unRequiredBufferLen = impl_->vr_system_->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, &err);

        if (err == vr::ETrackedPropertyError::TrackedProp_Success)
        {
            result = pchBuffer;
            delete[] pchBuffer;
        }
        else
        {
            error(fmt::format("Failed to get tracked device property: {}", impl_->vr_system_->GetPropErrorNameFromEnum(err)));
            delete[] pchBuffer;
            return false;
        }
    }

    return true;
}

bool OpenVRPlugin::get_tracked_device_property(bool& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const
{
    vr::ETrackedPropertyError err;
    auto tmp = impl_->vr_system_->GetBoolTrackedDeviceProperty(unDevice, prop, &err);
    if (err == vr::ETrackedPropertyError::TrackedProp_Success)
    {
        result = tmp;
    }
    else
    {
        error(fmt::format("Failed to get tracked device property: {}", impl_->vr_system_->GetPropErrorNameFromEnum(err)));
        return false;
    }
    return true;
}

bool OpenVRPlugin::get_tracked_device_property(int32_t& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const
{
    vr::ETrackedPropertyError err;
    auto tmp = impl_->vr_system_->GetInt32TrackedDeviceProperty(unDevice, prop, &err);
    if (err == vr::ETrackedPropertyError::TrackedProp_Success)
    {
        result = tmp;
    }
    else
    {
        error(fmt::format("Failed to get tracked device property: {}", impl_->vr_system_->GetPropErrorNameFromEnum(err)));
        return false;
    }
    return true;
}

bool OpenVRPlugin::get_tracked_device_property(uint64_t& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const
{
    vr::ETrackedPropertyError err;
    auto tmp = impl_->vr_system_->GetUint64TrackedDeviceProperty(unDevice, prop, &err);
    if (err == vr::ETrackedPropertyError::TrackedProp_Success)
    {
        result = tmp;
    }
    else
    {
        error(fmt::format("Failed to get tracked device property: {}", impl_->vr_system_->GetPropErrorNameFromEnum(err)));
        return false;
    }
    return true;
}

bool OpenVRPlugin::get_tracked_device_property(float& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const
{
    vr::ETrackedPropertyError err;
    auto tmp = impl_->vr_system_->GetFloatTrackedDeviceProperty(unDevice, prop, &err);
    if (err == vr::ETrackedPropertyError::TrackedProp_Success)
    {
        result = tmp;
    }
    else
    {
        error(fmt::format("Failed to get tracked device property: {}", impl_->vr_system_->GetPropErrorNameFromEnum(err)));
        return false;
    }
    return true;
}

bool OpenVRPlugin::get_tracked_device_property(LMatrix4f& result, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop) const
{
    vr::ETrackedPropertyError err;
    auto mat = impl_->vr_system_->GetMatrix34TrackedDeviceProperty(unDevice, prop, &err);
    if (err == vr::ETrackedPropertyError::TrackedProp_Success)
    {
        convert_matrix(mat, result);
    }
    else
    {
        error(fmt::format("Failed to get tracked device property: {}", impl_->vr_system_->GetPropErrorNameFromEnum(err)));
        return false;
    }
    return true;
}

vr::EVRScreenshotError OpenVRPlugin::take_stereo_screenshots(const Filename& preview_file_path, const Filename& vr_file_path) const
{
    if (!vr::VRScreenshots())
    {
        error("Failed to get interface for VRScreenshots API");
        return vr::EVRScreenshotError::VRScreenshotError_RequestFailed;
    }

    if (preview_file_path.empty() || vr_file_path.empty())
    {
        error(fmt::format("File path is empty: preview ({}), VR ({})", preview_file_path.c_str(), vr_file_path.c_str()));
        return vr::EVRScreenshotError::VRScreenshotError_RequestFailed;
    }

    auto preview_file_realpath = boost::filesystem::absolute(rppanda::convert_path(preview_file_path));
    if (!boost::filesystem::exists(preview_file_realpath.parent_path()))
    {
        error(fmt::format("Parent directory does NOT exist: {}", preview_file_realpath.parent_path().string()));
        return vr::EVRScreenshotError::VRScreenshotError_RequestFailed;
    }

    auto vr_file_realpath = boost::filesystem::absolute(rppanda::convert_path(vr_file_path));
    if (!boost::filesystem::exists(vr_file_realpath.parent_path()))
    {
        error(fmt::format("Parent directory does NOT exist: {}", vr_file_realpath.parent_path().string()));
        return vr::EVRScreenshotError::VRScreenshotError_RequestFailed;
    }

    debug(fmt::format("Take screenshots: preview ({}), VR ({})", preview_file_realpath.string(), vr_file_realpath.string()));

    vr::ScreenshotHandle_t handle;
    auto err = vr::VRScreenshots()->TakeStereoScreenshot(
        &handle,
        preview_file_realpath.generic_string().c_str(),
        vr_file_realpath.generic_string().c_str());

    if (err == vr::EVRScreenshotError::VRScreenshotError_ScreenshotAlreadyInProgress)
    {
        warn("Screnshots already in progress ...");
    }
    else if (err != vr::EVRScreenshotError::VRScreenshotError_None)
    {
        error(fmt::format("Failed to take screnshots: {}", impl_->get_screenshot_error_message(err)));
        error(fmt::format("Tried to take screnshots: preview ({}), VR ({})",
            preview_file_realpath.string(),
            vr_file_realpath.string()));
    }

    return err;
}

}
