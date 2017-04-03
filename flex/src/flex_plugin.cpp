/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016, Center of human-centered interaction for coexistence.
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

#include "../include/flex_plugin.hpp"

#include <clockObject.h>

#include <boost/dll/alias.hpp>

#include <NvFlex.h>
#include <NvFlexDevice.h>

#include "../include/flex_buffer.hpp"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "physics",
    PLUGIN_ID_STRING,
    "FlexPlugin",
    "Younguk Kim <yukim@chic.re.kr>",
    "0.1 alpha",

    "NVIDIA Flex plugin."
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<FlexPlugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

struct FlexPlugin::Impl
{
    ~Impl(void);

    void on_pipeline_created(void);
    void on_pre_render_update(void);
    void on_post_render_update(void);

    static RequrieType require_plugins_;

    NvFlexLibrary* library_ = nullptr;
    FlexBuffer* buffer_ = nullptr;
    NvFlexSolver* solver_ = nullptr;

    NvFlexParams params_;

    const int max_particles_ = 50000;
    const int substeps_ = 2;
};

FlexPlugin::RequrieType FlexPlugin::Impl::require_plugins_;

FlexPlugin::Impl::~Impl(void)
{
    if (buffer_)
    {
        buffer_->destroy();
        delete buffer_;
    }

    if (solver_)
        NvFlexDestroySolver(solver_);

    NvFlexShutdown(library_);
}

void FlexPlugin::Impl::on_pipeline_created(void)
{
    // alloc buffers
    buffer_ = new FlexBuffer(library_);

    // map during initialization
    buffer_->map();

    buffer_->positions.resize(0);
    buffer_->velocities.resize(0);
    buffer_->phases.resize(0);

    // sim params
    params_.gravity[0] = 0.0f;
    params_.gravity[1] = 0.0f;
    params_.gravity[2] = -9.8f;

    params_.wind[0] = 0.0f;
    params_.wind[1] = 0.0f;
    params_.wind[2] = 0.0f;

    params_.radius = 2.0f;
    params_.viscosity = 0.0f;
    params_.dynamicFriction = 0.0f;
    params_.staticFriction = 0.0f;
    params_.particleFriction = 0.0f; // scale friction between particles by default
    params_.freeSurfaceDrag = 0.0f;
    params_.drag = 0.0f;
    params_.lift = 0.0f;
    params_.numIterations = 3;
    params_.fluidRestDistance = 0.0f;
    params_.solidRestDistance = 0.0f;

    params_.anisotropyScale = 1.0f;
    params_.anisotropyMin = 0.1f;
    params_.anisotropyMax = 2.0f;
    params_.smoothing = 1.0f;

    params_.dissipation = 0.0f;
    params_.damping = 0.0f;
    params_.particleCollisionMargin = params_.radius*0.5f;
    params_.shapeCollisionMargin =  params_.radius*0.5f;
    params_.collisionDistance = 0.0f;
    params_.plasticThreshold = 0.0f;
    params_.plasticCreep = 0.0f;
    params_.fluid = false;
    params_.sleepThreshold = 0.0f;
    params_.shockPropagation = 0.0f;
    params_.restitution = 0.0f;

    params_.maxSpeed = FLT_MAX;
    params_.maxAcceleration = 100.0f;	// approximately 10x gravity

    params_.relaxationMode = eNvFlexRelaxationLocal;
    params_.relaxationFactor = 1.0f;
    params_.solidPressure = 1.0f;
    params_.adhesion = 0.0f;
    params_.cohesion = 0.025f;
    params_.surfaceTension = 0.0f;
    params_.vorticityConfinement = 0.0f;
    params_.buoyancy = 1.0f;
    params_.diffuseThreshold = 100.0f;
    params_.diffuseBuoyancy = 1.0f;
    params_.diffuseDrag = 0.8f;
    params_.diffuseBallistic = 16;
    params_.diffuseSortAxis[0] = 0.0f;
    params_.diffuseSortAxis[1] = 0.0f;
    params_.diffuseSortAxis[2] = 0.0f;
    params_.diffuseLifetime = 2.0f;

    // planes created after particles
    params_.numPlanes = 1;

    // create scene

    int numParticles = buffer_->positions.size();

    // create active indices (just a contiguous block for the demo)
    buffer_->active_indices.resize(buffer_->positions.size());
    for (int i = 0; i < buffer_->active_indices.size(); ++i)
        buffer_->active_indices[i] = i;

    // by default solid particles use the maximum radius
    if (params_.fluid && params_.solidRestDistance == 0.0f)
        params_.solidRestDistance = params_.fluidRestDistance;
    else
        params_.solidRestDistance = params_.radius;

    // collision distance with shapes half the radius
    if (params_.collisionDistance == 0.0f)
    {
        params_.collisionDistance = params_.radius*0.5f;

        if (params_.fluid)
            params_.collisionDistance = params_.fluidRestDistance*0.5f;
    }

    // default particle friction to 10% of shape friction
    if (params_.particleFriction == 0.0f)
        params_.particleFriction = params_.dynamicFriction*0.1f;

    // add a margin for detecting contacts between particles and shapes
    if (params_.shapeCollisionMargin == 0.0f)
        params_.shapeCollisionMargin = params_.collisionDistance*0.5f;

    // update collision planes to match flexs
    (LVecBase4f&)params_.planes[0] = LVecBase4f(0.0f, 0.0f, 1.0f, 0.0f);

    // main create method for the Flex solver
    solver_ = NvFlexCreateSolver(library_, max_particles_, 0);

    // unmap so we can start transferring data to GPU
    buffer_->unmap();

    // Send data to Flex
    NvFlexSetParams(solver_, &params_);
    NvFlexSetParticles(solver_, buffer_->positions.buffer, numParticles);
    NvFlexSetVelocities(solver_, buffer_->velocities.buffer, numParticles);
    NvFlexSetPhases(solver_, buffer_->phases.buffer, buffer_->phases.size());

    NvFlexSetActive(solver_, buffer_->active_indices.buffer, numParticles);
}

void FlexPlugin::Impl::on_pre_render_update(void)
{
#if 0
    // Scene Update
    buffer_->map();

    LVecBase4f* particles = buffer_->positions.mappedPtr;
    std::vector<LMatrix4f> mats(instancing_node->get_instance_count());
    for (auto& m: mats)
    {
        m = LMatrix4f::translate_mat(particles->get_xyz());
        ++particles;
    }
    instancing_node->set_transforms(mats);
    instancing_node->upload_transforms();

    // unmap buffers
    buffer_->unmap();
#endif
}

void FlexPlugin::Impl::on_post_render_update(void)
{
    // send any particle updates to the solver
    //NvFlexSetParticles(solver_, buffer_->positions.buffer, buffer_->positions.size());
    //NvFlexSetVelocities(solver_, buffer_->velocities.buffer, buffer_->velocities.size());
    //NvFlexSetPhases(solver_, buffer_->phases.buffer, buffer_->phases.size());

    // tick solver
    NvFlexUpdateSolver(solver_, float(ClockObject::get_global_clock()->get_dt()), substeps_, false);

    // read back base particle data
    // Note that flexGet calls don't wait for the GPU, they just queue a GPU copy 
    // to be executed later.
    // When we're ready to read the fetched buffers we'll Map them, and that's when
    // the CPU will wait for the GPU flex update and GPU copy to finish.
    NvFlexGetParticles(solver_, buffer_->positions.buffer, buffer_->positions.size());
    //NvFlexGetVelocities(solver_, buffer_->velocities.buffer, buffer_->velocities.size());
}

// ************************************************************************************************

FlexPlugin::FlexPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, plugin_info), impl_(new Impl)
{
}

FlexPlugin::~FlexPlugin(void) = default;

FlexPlugin::RequrieType& FlexPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void FlexPlugin::on_load(void)
{
    // use the PhysX GPU selected from the NVIDIA control panel
    int device_index = NvFlexDeviceGetSuggestedOrdinal();

    // Create an optimized CUDA context for Flex and set it on the 
    // calling thread. This is an optional call, it is fine to use 
    // a regular CUDA context, although creating one through this API
    // is recommended for best performance.
    bool success = NvFlexDeviceCreateCudaContext(device_index);

    if (!success)
    {
        std::cout << "Error creating CUDA context." << std::endl;
        return;
    }

    NvFlexInitDesc desc;
    desc.deviceIndex = device_index;
    desc.enableExtensions = true;
    desc.renderDevice = 0;
    desc.renderContext = 0;
    desc.computeType = eNvFlexCUDA;

    // Init Flex library, note that no CUDA methods should be called before this 
    // point to ensure we get the device context we want
    impl_->library_ = NvFlexInit(NV_FLEX_VERSION, [](NvFlexErrorSeverity, const char* msg, const char* file, int line) {
        RPObject::global_error(PLUGIN_ID_STRING, std::string(msg) + " - " + std::string(file) + ":" + std::to_string(line));
    }, &desc);

    if (!impl_->library_)
    {
        error("Could not initialize Flex, exiting.\n");
        return;
    }

    // store device name
    info(std::string("Compute Device: ") + NvFlexGetDeviceName(impl_->library_));
}

void FlexPlugin::on_stage_setup(void)
{
}

void FlexPlugin::on_pipeline_created(void)
{
    impl_->on_pipeline_created();
}

void FlexPlugin::on_pre_render_update(void)
{
    impl_->on_pre_render_update();
}

void FlexPlugin::on_post_render_update(void)
{
    impl_->on_post_render_update();
}
