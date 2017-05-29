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

#pragma once

#include <luse.h>

#include <NvFlex.h>
#include <NvFlexExt.h>

namespace rpflex {

struct FlexBuffer
{
    FlexBuffer(NvFlexLibrary* lib);

    void destroy(void);

    virtual void map(void);
    virtual void unmap(void);

    // buffers
    NvFlexVector<LVecBase4f> positions_;
    NvFlexVector<LVecBase4f> rest_positions_;
    NvFlexVector<LVecBase3f> velocities_;
    NvFlexVector<int> phases_;
    NvFlexVector<float> densities_;
    NvFlexVector<LVecBase4f> anisotropy1_;
    NvFlexVector<LVecBase4f> anisotropy2_;
    NvFlexVector<LVecBase4f> anisotropy3_;
    NvFlexVector<LVecBase4f> normals_;
    NvFlexVector<LVecBase4f> smooth_positions_;
    NvFlexVector<LVecBase4f> diffuse_positions_;
    NvFlexVector<LVecBase4f> diffuse_velocities_;
    NvFlexVector<int> diffuse_indices_;
    NvFlexVector<int> active_indices_;

    // convexes
    NvFlexVector<NvFlexCollisionGeometry> shape_geometry_;
    NvFlexVector<LVecBase4f> shape_positions_;
    NvFlexVector<LQuaternionf> shape_rotations_;
    NvFlexVector<LVecBase4f> shape_prev_positions_;
    NvFlexVector<LQuaternionf> shape_prev_rotations_;
    NvFlexVector<int> shape_flags_;

    // rigids
    NvFlexVector<int> rigid_offsets_;
    NvFlexVector<int> rigid_indices_;
    NvFlexVector<int> rigid_mesh_size_;
    NvFlexVector<float> rigid_coefficients_;
    NvFlexVector<LQuaternionf> rigid_rotations_;
    NvFlexVector<LVecBase3f> rigid_translations_;
    NvFlexVector<LVecBase3f> rigid_local_positions_;
    NvFlexVector<LVecBase4f> rigid_local_normals_;

    // inflatables
    NvFlexVector<int> inflatable_tri_offsets_;
    NvFlexVector<int> inflatable_tri_counts_;
    NvFlexVector<float> inflatable_volumes_;
    NvFlexVector<float> inflatable_coefficients_;
    NvFlexVector<float> inflatable_pressures_;

    // springs
    NvFlexVector<int> spring_indices_;
    NvFlexVector<float> spring_lengths_;
    NvFlexVector<float> spring_stiffness_;

    NvFlexVector<int> triangles_;
    NvFlexVector<LVecBase3f> triangle_normals_;
    NvFlexVector<LVecBase3f> uvs_;
};

// ************************************************************************************************
inline FlexBuffer::FlexBuffer(NvFlexLibrary* lib):
    positions_(lib), rest_positions_(lib), velocities_(lib), phases_(lib), densities_(lib), anisotropy1_(lib),
    anisotropy2_(lib), anisotropy3_(lib), normals_(lib), smooth_positions_(lib),
    diffuse_positions_(lib), diffuse_velocities_(lib), diffuse_indices_(lib), active_indices_(lib),
    // convexes
    shape_geometry_(lib), shape_positions_(lib), shape_rotations_(lib), shape_prev_positions_(lib), shape_prev_rotations_(lib), shape_flags_(lib),
    // rigids
    rigid_offsets_(lib), rigid_indices_(lib), rigid_mesh_size_(lib), rigid_coefficients_(lib), rigid_rotations_(lib), rigid_translations_(lib),
    rigid_local_positions_(lib), rigid_local_normals_(lib),
    // inflatables
    inflatable_tri_offsets_(lib), inflatable_tri_counts_(lib), inflatable_volumes_(lib), inflatable_coefficients_(lib), inflatable_pressures_(lib),
    // springs
    spring_indices_(lib), spring_lengths_(lib), spring_stiffness_(lib),
    triangles_(lib), triangle_normals_(lib), uvs_(lib)
{
}

inline void FlexBuffer::destroy(void)
{
    positions_.destroy();
    rest_positions_.destroy();
    velocities_.destroy();
    phases_.destroy();
    densities_.destroy();
    anisotropy1_.destroy();
    anisotropy2_.destroy();
    anisotropy3_.destroy();
    normals_.destroy();
    smooth_positions_.destroy();
    diffuse_positions_.destroy();
    diffuse_velocities_.destroy();
    diffuse_indices_.destroy();
    active_indices_.destroy();

    // convexes
    shape_geometry_.destroy();
    shape_positions_.destroy();
    shape_rotations_.destroy();
    shape_prev_positions_.destroy();
    shape_prev_rotations_.destroy();
    shape_flags_.destroy();

    // rigids
    rigid_offsets_.destroy();
    rigid_indices_.destroy();
    rigid_mesh_size_.destroy();
    rigid_coefficients_.destroy();
    rigid_rotations_.destroy();
    rigid_translations_.destroy();
    rigid_local_positions_.destroy();
    rigid_local_normals_.destroy();

    // inflatables
    inflatable_tri_offsets_.destroy();
    inflatable_tri_counts_.destroy();
    inflatable_volumes_.destroy();
    inflatable_coefficients_.destroy();
    inflatable_pressures_.destroy();

    // springs
    spring_indices_.destroy();
    spring_lengths_.destroy();
    spring_stiffness_.destroy();

    triangles_.destroy();
    triangle_normals_.destroy();
    uvs_.destroy();
}

inline void FlexBuffer::map(void)
{
    positions_.map();
    rest_positions_.map();
    velocities_.map();
    phases_.map();
    densities_.map();
    anisotropy1_.map();
    anisotropy2_.map();
    anisotropy3_.map();
    normals_.map();
    smooth_positions_.map();
    diffuse_positions_.map();
    diffuse_velocities_.map();
    diffuse_indices_.map();
    active_indices_.map();

    // convexes
    shape_geometry_.map();
    shape_positions_.map();
    shape_rotations_.map();
    shape_prev_positions_.map();
    shape_prev_rotations_.map();
    shape_flags_.map();

    // rigids
    rigid_offsets_.map();
    rigid_indices_.map();
    rigid_mesh_size_.map();
    rigid_coefficients_.map();
    rigid_rotations_.map();
    rigid_translations_.map();
    rigid_local_positions_.map();
    rigid_local_normals_.map();

    // inflatables
    inflatable_tri_offsets_.map();
    inflatable_tri_counts_.map();
    inflatable_volumes_.map();
    inflatable_coefficients_.map();
    inflatable_pressures_.map();

    // springs
    spring_indices_.map();
    spring_lengths_.map();
    spring_stiffness_.map();

    triangles_.map();
    triangle_normals_.map();
    uvs_.map();
}

inline void FlexBuffer::unmap(void)
{
    positions_.unmap();
    rest_positions_.unmap();
    velocities_.unmap();
    phases_.unmap();
    densities_.unmap();
    anisotropy1_.unmap();
    anisotropy2_.unmap();
    anisotropy3_.unmap();
    normals_.unmap();
    smooth_positions_.unmap();
    diffuse_positions_.unmap();
    diffuse_velocities_.unmap();
    diffuse_indices_.unmap();
    active_indices_.unmap();

    // convexes
    shape_geometry_.unmap();
    shape_positions_.unmap();
    shape_rotations_.unmap();
    shape_prev_positions_.unmap();
    shape_prev_rotations_.unmap();
    shape_flags_.unmap();

    // rigids
    rigid_offsets_.unmap();
    rigid_indices_.unmap();
    rigid_mesh_size_.unmap();
    rigid_coefficients_.unmap();
    rigid_rotations_.unmap();
    rigid_translations_.unmap();
    rigid_local_positions_.unmap();
    rigid_local_normals_.unmap();

    // inflatables
    inflatable_tri_offsets_.unmap();
    inflatable_tri_counts_.unmap();
    inflatable_volumes_.unmap();
    inflatable_coefficients_.unmap();
    inflatable_pressures_.unmap();

    // springs
    spring_indices_.unmap();
    spring_lengths_.unmap();
    spring_stiffness_.unmap();

    triangles_.unmap();
    triangle_normals_.unmap();
    uvs_.unmap();
}

}
