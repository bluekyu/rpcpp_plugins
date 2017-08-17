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

#include <NvFlex.h>
#include <NvFlexExt.h>

namespace rpflex {

struct FlexBuffer
{
    FlexBuffer(NvFlexLibrary* lib);

    void destroy();

    void map();
    void unmap();

    // buffers
    NvFlexVector<LVecBase4f> positions;
    NvFlexVector<LVecBase4f> rest_positions;
    NvFlexVector<LVecBase3f> velocities;
    NvFlexVector<int> phases;
    NvFlexVector<float> densities;
    NvFlexVector<LVecBase4f> anisotropy1;
    NvFlexVector<LVecBase4f> anisotropy2;
    NvFlexVector<LVecBase4f> anisotropy3;
    NvFlexVector<LVecBase4f> normals;
    NvFlexVector<LVecBase4f> smooth_positions;
    NvFlexVector<LVecBase4f> diffuse_positions;
    NvFlexVector<LVecBase4f> diffuse_velocities;
    NvFlexVector<int> diffuse_indices;
    NvFlexVector<int> active_indices;

    // convexes
    NvFlexVector<NvFlexCollisionGeometry> shape_geometry;
    NvFlexVector<LVecBase4f> shape_positions;
    NvFlexVector<LQuaternionf> shape_rotations;
    NvFlexVector<LVecBase4f> shape_prev_positions;
    NvFlexVector<LQuaternionf> shape_prev_rotations;
    NvFlexVector<int> shape_flags;

    // rigids
    NvFlexVector<int> rigid_offsets;
    NvFlexVector<int> rigid_indices;
    NvFlexVector<int> rigid_mesh_size;
    NvFlexVector<float> rigid_coefficients;
    NvFlexVector<LQuaternionf> rigid_rotations;
    NvFlexVector<LVecBase3f> rigid_translations;
    NvFlexVector<LVecBase3f> rigid_local_positions;
    NvFlexVector<LVecBase4f> rigid_local_normals;

    // inflatables
    NvFlexVector<int> inflatable_tri_offsets;
    NvFlexVector<int> inflatable_tri_counts;
    NvFlexVector<float> inflatable_volumes;
    NvFlexVector<float> inflatable_coefficients;
    NvFlexVector<float> inflatable_pressures;

    // springs
    NvFlexVector<int> spring_indices;
    NvFlexVector<float> spring_lengths;
    NvFlexVector<float> spring_stiffness;

    NvFlexVector<int> triangles;
    NvFlexVector<LVecBase3f> triangle_normals;
    NvFlexVector<LVecBase3f> uvs;
};

// ************************************************************************************************
inline FlexBuffer::FlexBuffer(NvFlexLibrary* lib):
    positions(lib), rest_positions(lib), velocities(lib), phases(lib), densities(lib), anisotropy1(lib),
    anisotropy2(lib), anisotropy3(lib), normals(lib), smooth_positions(lib),
    diffuse_positions(lib), diffuse_velocities(lib), diffuse_indices(lib), active_indices(lib),
    // convexes
    shape_geometry(lib), shape_positions(lib), shape_rotations(lib), shape_prev_positions(lib), shape_prev_rotations(lib), shape_flags(lib),
    // rigids
    rigid_offsets(lib), rigid_indices(lib), rigid_mesh_size(lib), rigid_coefficients(lib), rigid_rotations(lib), rigid_translations(lib),
    rigid_local_positions(lib), rigid_local_normals(lib),
    // inflatables
    inflatable_tri_offsets(lib), inflatable_tri_counts(lib), inflatable_volumes(lib), inflatable_coefficients(lib), inflatable_pressures(lib),
    // springs
    spring_indices(lib), spring_lengths(lib), spring_stiffness(lib),
    triangles(lib), triangle_normals(lib), uvs(lib)
{
}

inline void FlexBuffer::destroy()
{
    positions.destroy();
    rest_positions.destroy();
    velocities.destroy();
    phases.destroy();
    densities.destroy();
    anisotropy1.destroy();
    anisotropy2.destroy();
    anisotropy3.destroy();
    normals.destroy();
    smooth_positions.destroy();
    diffuse_positions.destroy();
    diffuse_velocities.destroy();
    diffuse_indices.destroy();
    active_indices.destroy();

    // convexes
    shape_geometry.destroy();
    shape_positions.destroy();
    shape_rotations.destroy();
    shape_prev_positions.destroy();
    shape_prev_rotations.destroy();
    shape_flags.destroy();

    // rigids
    rigid_offsets.destroy();
    rigid_indices.destroy();
    rigid_mesh_size.destroy();
    rigid_coefficients.destroy();
    rigid_rotations.destroy();
    rigid_translations.destroy();
    rigid_local_positions.destroy();
    rigid_local_normals.destroy();

    // inflatables
    inflatable_tri_offsets.destroy();
    inflatable_tri_counts.destroy();
    inflatable_volumes.destroy();
    inflatable_coefficients.destroy();
    inflatable_pressures.destroy();

    // springs
    spring_indices.destroy();
    spring_lengths.destroy();
    spring_stiffness.destroy();

    triangles.destroy();
    triangle_normals.destroy();
    uvs.destroy();
}

inline void FlexBuffer::map()
{
    positions.map();
    rest_positions.map();
    velocities.map();
    phases.map();
    densities.map();
    anisotropy1.map();
    anisotropy2.map();
    anisotropy3.map();
    normals.map();
    smooth_positions.map();
    diffuse_positions.map();
    diffuse_velocities.map();
    diffuse_indices.map();
    active_indices.map();

    // convexes
    shape_geometry.map();
    shape_positions.map();
    shape_rotations.map();
    shape_prev_positions.map();
    shape_prev_rotations.map();
    shape_flags.map();

    // rigids
    rigid_offsets.map();
    rigid_indices.map();
    rigid_mesh_size.map();
    rigid_coefficients.map();
    rigid_rotations.map();
    rigid_translations.map();
    rigid_local_positions.map();
    rigid_local_normals.map();

    // inflatables
    inflatable_tri_offsets.map();
    inflatable_tri_counts.map();
    inflatable_volumes.map();
    inflatable_coefficients.map();
    inflatable_pressures.map();

    // springs
    spring_indices.map();
    spring_lengths.map();
    spring_stiffness.map();

    triangles.map();
    triangle_normals.map();
    uvs.map();
}

inline void FlexBuffer::unmap()
{
    positions.unmap();
    rest_positions.unmap();
    velocities.unmap();
    phases.unmap();
    densities.unmap();
    anisotropy1.unmap();
    anisotropy2.unmap();
    anisotropy3.unmap();
    normals.unmap();
    smooth_positions.unmap();
    diffuse_positions.unmap();
    diffuse_velocities.unmap();
    diffuse_indices.unmap();
    active_indices.unmap();

    // convexes
    shape_geometry.unmap();
    shape_positions.unmap();
    shape_rotations.unmap();
    shape_prev_positions.unmap();
    shape_prev_rotations.unmap();
    shape_flags.unmap();

    // rigids
    rigid_offsets.unmap();
    rigid_indices.unmap();
    rigid_mesh_size.unmap();
    rigid_coefficients.unmap();
    rigid_rotations.unmap();
    rigid_translations.unmap();
    rigid_local_positions.unmap();
    rigid_local_normals.unmap();

    // inflatables
    inflatable_tri_offsets.unmap();
    inflatable_tri_counts.unmap();
    inflatable_volumes.unmap();
    inflatable_coefficients.unmap();
    inflatable_pressures.unmap();

    // springs
    spring_indices.unmap();
    spring_lengths.unmap();
    spring_stiffness.unmap();

    triangles.unmap();
    triangle_normals.unmap();
    uvs.unmap();
}

}
