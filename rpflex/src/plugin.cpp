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

#include "rpflex/plugin.hpp"

#include <clockObject.h>

#include <boost/dll/alias.hpp>

#include <NvFlex.h>
#include <NvFlexDevice.h>

#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

#include "rpflex/flex_buffer.hpp"
#include "rpflex/instance_interface.hpp"
#include "rpflex/utils/helpers.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpflex::Plugin)

namespace rpflex {

class Plugin::Impl
{
public:
    Impl(Plugin& self);

    void destroy();
    void reset();

    void on_pipeline_created();
    void on_pre_render_update();
    void on_post_render_update();
    void on_unload();

    static RequrieType require_plugins_;

public:
    Plugin& self_;

    NvFlexLibrary* library_ = nullptr;
    FlexBuffer* buffer_ = nullptr;
    NvFlexSolver* solver_ = nullptr;

    NvFlexParams flex_params_;
    bool flex_params_changed_ = false;

    Plugin::Parameters params_;

    std::vector<std::shared_ptr<InstanceInterface>> instances_;
};

Plugin::RequrieType Plugin::Impl::require_plugins_;

Plugin::Impl::Impl(Plugin& self): self_(self)
{
    params_.substeps_count = 2;

    params_.num_extra_multiplier = 1;

    params_.wave_floor_tilt = 0.0f;
}

void Plugin::Impl::destroy()
{
    self_.trace("Destroy flex.");

    if (buffer_)
    {
        buffer_->destroy();
        delete buffer_;
        buffer_ = nullptr;
    }

    if (solver_)
    {
        NvFlexDestroySolver(solver_);
        solver_ = nullptr;
    }
}

void Plugin::Impl::reset()
{
    self_.trace("Reset flex.");

    if (solver_)
        destroy();

    self_.trace("Creating flex buffer.");

    // alloc buffers
    buffer_ = new FlexBuffer(library_);

    // map during initialization
    buffer_->map();

    buffer_->positions.resize(0);
    buffer_->velocities.resize(0);
    buffer_->phases.resize(0);

    buffer_->rigid_offsets.resize(0);
    buffer_->rigid_indices.resize(0);
    buffer_->rigid_mesh_size.resize(0);
    buffer_->rigid_coefficients.resize(0);
    buffer_->rigid_rotations.resize(0);
    buffer_->rigid_translations.resize(0);
    buffer_->rigid_local_positions.resize(0);
    buffer_->rigid_local_normals.resize(0);

    buffer_->shape_geometry.resize(0);
    buffer_->shape_positions.resize(0);
    buffer_->shape_rotations.resize(0);
    buffer_->shape_prev_positions.resize(0);
    buffer_->shape_prev_rotations.resize(0);
    buffer_->shape_flags.resize(0);

    self_.trace("Setup simulation parameters.");

    // sim params
    flex_params_.gravity[0] = 0.0f;
    flex_params_.gravity[1] = 0.0f;
    flex_params_.gravity[2] = -9.8f;

    flex_params_.wind[0] = 0.0f;
    flex_params_.wind[1] = 0.0f;
    flex_params_.wind[2] = 0.0f;

    flex_params_.radius = 0.2f;
    flex_params_.viscosity = 0.0f;
    flex_params_.dynamicFriction = 0.0f;
    flex_params_.staticFriction = 0.0f;
    flex_params_.particleFriction = 0.0f; // scale friction between particles by default
    flex_params_.freeSurfaceDrag = 0.0f;
    flex_params_.drag = 0.0f;
    flex_params_.lift = 0.0f;
    flex_params_.numIterations = 3;
    flex_params_.fluidRestDistance = 0.0f;
    flex_params_.solidRestDistance = 0.0f;

    flex_params_.anisotropyScale = 1.0f;
    flex_params_.anisotropyMin = 0.1f;
    flex_params_.anisotropyMax = 2.0f;
    flex_params_.smoothing = 1.0f;

    flex_params_.dissipation = 0.0f;
    flex_params_.damping = 0.0f;
    flex_params_.particleCollisionMargin = 0.0f;
    flex_params_.shapeCollisionMargin = 0.0f;
    flex_params_.collisionDistance = 0.0f;
    flex_params_.plasticThreshold = 0.0f;
    flex_params_.plasticCreep = 0.0f;
    flex_params_.fluid = false;
    flex_params_.sleepThreshold = 0.0f;
    flex_params_.shockPropagation = 0.0f;
    flex_params_.restitution = 0.0f;

    flex_params_.maxSpeed = FLT_MAX;
    flex_params_.maxAcceleration = 100.0f;    // approximately 10x gravity

    flex_params_.relaxationMode = eNvFlexRelaxationLocal;
    flex_params_.relaxationFactor = 1.0f;
    flex_params_.solidPressure = 1.0f;
    flex_params_.adhesion = 0.0f;
    flex_params_.cohesion = 0.025f;
    flex_params_.surfaceTension = 0.0f;
    flex_params_.vorticityConfinement = 0.0f;
    flex_params_.buoyancy = 1.0f;
    flex_params_.diffuseThreshold = 100.0f;
    flex_params_.diffuseBuoyancy = 1.0f;
    flex_params_.diffuseDrag = 0.8f;
    flex_params_.diffuseBallistic = 16;
    flex_params_.diffuseSortAxis[0] = 0.0f;
    flex_params_.diffuseSortAxis[1] = 0.0f;
    flex_params_.diffuseSortAxis[2] = 0.0f;
    flex_params_.diffuseLifetime = 2.0f;

    params_.substeps_count = 2;

    // planes created after particles
    flex_params_.numPlanes = 1;

    params_.max_diffuse_particles = 0;  // number of diffuse particles
    params_.max_neighbors_per_particle = 96;
    params_.num_extra_particles = 0;    // number of particles allocated but not made active    

    params_.scene_lower = FLT_MAX;
    params_.scene_upper = -FLT_MAX;

    // create scene
    for (auto& instance: instances_)
        instance->initialize(self_);

    uint32_t num_particles = buffer_->positions.size();
    uint32_t max_particles = num_particles + params_.num_extra_particles * params_.num_extra_multiplier;

    // by default solid particles use the maximum radius
    if (flex_params_.fluid && flex_params_.solidRestDistance == 0.0f)
        flex_params_.solidRestDistance = flex_params_.fluidRestDistance;
    else
        flex_params_.solidRestDistance = flex_params_.radius;

    // collision distance with shapes half the radius
    if (flex_params_.collisionDistance == 0.0f)
    {
        flex_params_.collisionDistance = flex_params_.radius*0.5f;

        if (flex_params_.fluid)
            flex_params_.collisionDistance = flex_params_.fluidRestDistance*0.5f;
    }

    // default particle friction to 10% of shape friction
    if (flex_params_.particleFriction == 0.0f)
        flex_params_.particleFriction = flex_params_.dynamicFriction*0.1f;

    // add a margin for detecting contacts between particles and shapes
    if (flex_params_.shapeCollisionMargin == 0.0f)
        flex_params_.shapeCollisionMargin = flex_params_.collisionDistance*0.5f;

    // calculate particle bounds
    LVecBase3f particle_lower;
    LVecBase3f particle_upper;
    GetParticleBounds(*buffer_, particle_lower, particle_upper);

    // TODO: implement
    // accommodate shapes
    //LVecBase3f shape_lower;
    //LVecBase3f shape_upper;
    //GetShapeBounds(shape_lower, shape_upper);

    // update bounds
    //params_.scene_lower = (std::min)((std::min)(params_.scene_lower, particle_lower), shape_lower);
    //params_.scene_upper = (std::max)((std::max)(params_.scene_upper, particle_upper), shape_upper);
    params_.scene_lower = params_.scene_lower.fmin(particle_lower);
    params_.scene_upper = params_.scene_upper.fmax(particle_upper);

    params_.scene_lower -= flex_params_.collisionDistance;
    params_.scene_upper += flex_params_.collisionDistance;

    // update collision planes to match flexs
    LVecBase3f up = LVecBase3f(-params_.wave_floor_tilt, 0.0f, 1.0f).normalized();

    reinterpret_cast<LVecBase4f&>(flex_params_.planes[0]) = LVecBase4f(up[0], up[1], up[2], 0.0f);
    reinterpret_cast<LVecBase4f&>(flex_params_.planes[1]) = LVecBase4f(0.0f, -1.0f, 0.0f, params_.scene_upper[1]);
    reinterpret_cast<LVecBase4f&>(flex_params_.planes[2]) = LVecBase4f(1.0f, 0.0f, 0.0f, -params_.scene_lower[0]);
    reinterpret_cast<LVecBase4f&>(flex_params_.planes[3]) = LVecBase4f(-1.0f, 0.0f, 0.0f, params_.scene_upper[0]);
    reinterpret_cast<LVecBase4f&>(flex_params_.planes[4]) = LVecBase4f(0.0f, 1.0f, 0.0f, params_.scene_lower[1]);
    reinterpret_cast<LVecBase4f&>(flex_params_.planes[5]) = LVecBase4f(0.0f, 0.0f, -1.0f, params_.scene_upper[2]);

    //g_wavePlane = g_params.planes[2][3];

    buffer_->diffuse_positions.resize(params_.max_diffuse_particles);
    buffer_->diffuse_velocities.resize(params_.max_diffuse_particles);
    buffer_->diffuse_indices.resize(params_.max_diffuse_particles);

    // for fluid rendering these are the Laplacian smoothed positions
    buffer_->smooth_positions.resize(max_particles);

    buffer_->normals.resize(0);
    buffer_->normals.resize(max_particles);

    self_.trace("Creating solver.");

    // main create method for the Flex solver
    solver_ = NvFlexCreateSolver(library_, max_particles, params_.max_diffuse_particles, params_.max_neighbors_per_particle);

    // create active indices (just a contiguous block for the demo)
    buffer_->active_indices.resize(buffer_->positions.size());
    for (int i = 0; i < buffer_->active_indices.size(); ++i)
        buffer_->active_indices[i] = i;

    // resize particle buffers to fit
    buffer_->positions.resize(max_particles);
    buffer_->velocities.resize(max_particles);
    buffer_->phases.resize(max_particles);

    buffer_->densities.resize(max_particles);
    buffer_->anisotropy1.resize(max_particles);
    buffer_->anisotropy2.resize(max_particles);
    buffer_->anisotropy3.resize(max_particles);

    // save rest positions
    buffer_->rest_positions.resize(buffer_->positions.size());
    for (int i=0, i_end=buffer_->positions.size(); i < i_end; ++i)
        buffer_->rest_positions[i] = buffer_->positions[i];

    // builds rigids constraints
    if (buffer_->rigid_offsets.size())
    {
        assert(buffer_->rigid_offsets.size() > 1);

        const int num_rigids = buffer_->rigid_offsets.size() - 1;

        // calculate local rest space positions
        buffer_->rigid_local_positions.resize(buffer_->rigid_offsets.back());
        CalculateRigidLocalPositions(&buffer_->positions[0], buffer_->positions.size(), &buffer_->rigid_offsets[0],
            &buffer_->rigid_indices[0], num_rigids, &buffer_->rigid_local_positions[0]);

        buffer_->rigid_rotations.resize(buffer_->rigid_offsets.size() - 1, LQuaternionf());
        buffer_->rigid_translations.resize(buffer_->rigid_offsets.size() - 1, LVecBase3f());
    }

    for (auto& instance: instances_)
        instance->post_initialize(self_);

    // unmap so we can start transferring data to GPU
    buffer_->unmap();

    self_.trace("Sending data.");

    // Send data to Flex
    NvFlexSetParams(solver_, &flex_params_);
    NvFlexSetParticles(solver_, buffer_->positions.buffer, num_particles);
    NvFlexSetVelocities(solver_, buffer_->velocities.buffer, num_particles);
    NvFlexSetNormals(solver_, buffer_->normals.buffer, num_particles);
    NvFlexSetPhases(solver_, buffer_->phases.buffer, buffer_->phases.size());
    NvFlexSetRestParticles(solver_, buffer_->rest_positions.buffer, buffer_->rest_positions.size());

    NvFlexSetActive(solver_, buffer_->active_indices.buffer, num_particles);

    // springs
    if (buffer_->spring_indices.size())
    {
        assert((buffer_->spring_indices.size() & 1) == 0);
        assert((buffer_->spring_indices.size() / 2) == g_buffers->spring_lengths_.size());

        NvFlexSetSprings(solver_,
            buffer_->spring_indices.buffer,
            buffer_->spring_lengths.buffer,
            buffer_->spring_stiffness.buffer,
            buffer_->spring_lengths.size());
    }

    // rigids
    if (buffer_->rigid_offsets.size())
    {
        NvFlexSetRigids(solver_,
            buffer_->rigid_offsets.buffer,
            buffer_->rigid_indices.buffer,
            buffer_->rigid_local_positions.buffer,
            buffer_->rigid_local_normals.buffer,
            buffer_->rigid_coefficients.buffer,
            buffer_->rigid_rotations.buffer,
            buffer_->rigid_translations.buffer,
            buffer_->rigid_offsets.size() - 1,
            buffer_->rigid_indices.size());
    }

    // inflatables
    if (buffer_->inflatable_tri_offsets.size())
    {
        NvFlexSetInflatables(solver_,
            buffer_->inflatable_tri_offsets.buffer,
            buffer_->inflatable_tri_counts.buffer,
            buffer_->inflatable_volumes.buffer,
            buffer_->inflatable_pressures.buffer,
            buffer_->inflatable_coefficients.buffer,
            buffer_->inflatable_tri_offsets.size());
    }

    // dynamic triangles
    if (buffer_->triangles.size())
    {
        NvFlexSetDynamicTriangles(solver_,
            buffer_->triangles.buffer,
            buffer_->triangle_normals.buffer,
            buffer_->triangles.size() / 3);
    }

    // collision shapes
    if (buffer_->shape_flags.size())
    {
        NvFlexSetShapes(
            solver_,
            buffer_->shape_geometry.buffer,
            buffer_->shape_positions.buffer,
            buffer_->shape_rotations.buffer,
            buffer_->shape_prev_positions.buffer,
            buffer_->shape_prev_rotations.buffer,
            buffer_->shape_flags.buffer,
            int(buffer_->shape_flags.size()));
    }
}

void Plugin::Impl::on_pipeline_created()
{
    rpcore::Globals::base->add_task([this](rppanda::FunctionalTask* task) {
        reset();
        return AsyncTask::DS_done;
    }, "Plugin::reset");
}

void Plugin::Impl::on_pre_render_update()
{
    // Scene Update
    buffer_->map();

    for (auto& instance: instances_)
        instance->sync_flex(self_);

    // unmap buffers
    buffer_->unmap();
}

void Plugin::Impl::on_post_render_update()
{
    // send any particle updates to the solver
    NvFlexSetParticles(solver_, buffer_->positions.buffer, buffer_->positions.size());
    NvFlexSetVelocities(solver_, buffer_->velocities.buffer, buffer_->velocities.size());
    NvFlexSetPhases(solver_, buffer_->phases.buffer, buffer_->phases.size());
    NvFlexSetActive(solver_, buffer_->active_indices.buffer, buffer_->active_indices.size());

    // tick solver
    if (flex_params_changed_)
    {
        NvFlexSetParams(solver_, &flex_params_);
        flex_params_changed_ = false;
    }
    NvFlexUpdateSolver(solver_, float(ClockObject::get_global_clock()->get_dt()), params_.substeps_count, false);

    // read back base particle data
    // Note that flexGet calls don't wait for the GPU, they just queue a GPU copy 
    // to be executed later.
    // When we're ready to read the fetched buffers we'll Map them, and that's when
    // the CPU will wait for the GPU flex update and GPU copy to finish.
    NvFlexGetParticles(solver_, buffer_->positions.buffer, buffer_->positions.size());
    NvFlexGetVelocities(solver_, buffer_->velocities.buffer, buffer_->velocities.size());

    // readback triangle normals
    if (buffer_->triangles.size())
        NvFlexGetDynamicTriangles(solver_, buffer_->triangles.buffer, buffer_->triangle_normals.buffer, buffer_->triangles.size() / 3);

    // readback rigid transforms
    if (buffer_->rigid_offsets.size())
        NvFlexGetRigidTransforms(solver_, buffer_->rigid_rotations.buffer, buffer_->rigid_translations.buffer);
}

void Plugin::Impl::on_unload()
{
    destroy();

    if (library_)
        NvFlexShutdown(library_);
}

// ************************************************************************************************

Plugin::Plugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>(*this))
{
}

Plugin::~Plugin() = default;

Plugin::RequrieType& Plugin::get_required_plugins() const
{
    return impl_->require_plugins_;
}

void Plugin::on_load()
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
        error("Error creating CUDA context.");
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
        RPObject::global_error(RPPLUGIN_ID_STRING, std::string(msg) + " - " + std::string(file) + ":" + std::to_string(line));
    }, &desc);

    if (!impl_->library_)
    {
        error("Could not initialize Flex, exiting.\n");
        return;
    }

    // store device name
    info(std::string("Compute Device: ") + NvFlexGetDeviceName(impl_->library_));
}

void Plugin::on_stage_setup()
{
}

void Plugin::on_pipeline_created()
{
    impl_->on_pipeline_created();
}

void Plugin::on_pre_render_update()
{
    impl_->on_pre_render_update();
}

void Plugin::on_post_render_update()
{
    impl_->on_post_render_update();
}

void Plugin::on_unload()
{
    impl_->on_unload();
}

void Plugin::add_instance(const std::shared_ptr<InstanceInterface>& instance)
{
    impl_->instances_.push_back(instance);
}

NvFlexLibrary* Plugin::get_flex_library() const
{
    return impl_->library_;
}

NvFlexSolver* Plugin::get_flex_solver() const
{
    return impl_->solver_;
}

const NvFlexParams& Plugin::get_flex_params() const
{
    return impl_->flex_params_;
}

NvFlexParams& Plugin::get_flex_params()
{
    impl_->flex_params_changed_ = true;
    return impl_->flex_params_;
}

const Plugin::Parameters& Plugin::get_plugin_params() const
{
    return impl_->params_;
}

Plugin::Parameters& Plugin::get_plugin_params()
{
    return impl_->params_;
}

const FlexBuffer& Plugin::get_flex_buffer() const
{
    return *impl_->buffer_;
}

FlexBuffer& Plugin::get_flex_buffer()
{
    return *impl_->buffer_;
}

}
