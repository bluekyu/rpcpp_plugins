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

#include <nodePath.h>
#include <geomNode.h>
#include <geomVertexReader.h>

#include <fmt/format.h>

#include <render_pipeline/rpcore/rpobject.h>
#include <render_pipeline/rpcore/globals.h>

#include <rpflex/utils/shape.hpp>

namespace rpflex {

class RPFlexTriangleMesh: public RPFlexShape
{
public:
    static NvFlexTriangleMeshId create_flex_triangle_mesh(NvFlexLibrary* flex_libray, const LPoint3f& lower, const LPoint3f& upper,
        const std::vector<LVecBase3f>& positions, const std::vector<int>& indices,
        int vertices_count, int faces_count);

    RPFlexTriangleMesh(FlexBuffer& buffer, NvFlexLibrary* flex_libray, const LPoint3f& lower, const LPoint3f& upper,
        const std::vector<LVecBase3f>& positions, const std::vector<int>& indices,
        int vertices_count, int faces_count, const LPoint3f& translation, const LQuaternionf& rotation, const LVecBase3f& scale);

    RPFlexTriangleMesh(FlexBuffer& buffer, NvFlexLibrary* flex_libray, NodePath geom_nodepath);

    NvFlexTriangleMeshId get_triangle_mesh_id(const FlexBuffer& buffer) const;

private:
    void initilize(FlexBuffer& buffer, NvFlexLibrary* flex_libray, const LPoint3f& lower, const LPoint3f& upper,
        const std::vector<LVecBase3f>& positions, const std::vector<int>& indices,
        int vertices_count, int faces_count, const LPoint3f& translation, const LQuaternionf& rotation, const LVecBase3f& scale);
};

// ************************************************************************************************
inline NvFlexTriangleMeshId RPFlexTriangleMesh::create_flex_triangle_mesh(NvFlexLibrary* flex_libray, const LPoint3f& lower, const LPoint3f& upper,
    const std::vector<LVecBase3f>& positions, const std::vector<int>& indices,
    int vertices_count, int faces_count)
{
    NvFlexVector<LVecBase3f> flex_positions(flex_libray);
    NvFlexVector<int> flex_indices(flex_libray);

    flex_positions.assign(positions.data(), positions.size());
    flex_indices.assign(indices.data(), indices.size());

    flex_positions.unmap();
    flex_indices.unmap();

    NvFlexTriangleMeshId flex_mesh = NvFlexCreateTriangleMesh(flex_libray);
    NvFlexUpdateTriangleMesh(flex_libray, flex_mesh, flex_positions.buffer, flex_indices.buffer,
        vertices_count, faces_count, lower.get_data(), upper.get_data());

    return flex_mesh;
}

inline RPFlexTriangleMesh::RPFlexTriangleMesh(FlexBuffer& buffer, NvFlexLibrary* flex_libray, const LPoint3f& lower, const LPoint3f& upper,
    const std::vector<LVecBase3f>& positions, const std::vector<int>& indices, int vertices_count, int faces_count,
    const LPoint3f& translation, const LQuaternionf& rotation, const LVecBase3f& scale)
{
    initilize(buffer, flex_libray, lower, upper, positions, indices, vertices_count, faces_count,
        translation, rotation, scale);
}

inline RPFlexTriangleMesh::RPFlexTriangleMesh(FlexBuffer& buffer, NvFlexLibrary* flex_libray, NodePath geom_nodepath)
{
    if (geom_nodepath.is_empty() || !geom_nodepath.node()->is_geom_node())
    {
        rpcore::RPObject::global_error(RPPLUGIN_DEBUG_NAME,
            fmt::format("NodePath ({}) is empty OR is NOT GeomNode.", geom_nodepath.get_name()));
        return;
    }

    PT(GeomNode) geom_node = DCAST(GeomNode, geom_nodepath.node());

    if (geom_node->get_num_geoms() != 1)
    {
        rpcore::RPObject::global_error(RPPLUGIN_DEBUG_NAME,
            fmt::format("GeomNode ({}) has NOT only 1 geom.", geom_nodepath.get_name()));
        return;
    }

    CPT(Geom) geom = geom_node->get_geom(0);

    LPoint3f lower;
    LPoint3f upper;
    bool found = false;
    geom->calc_tight_bounds(lower, upper, found, Thread::get_current_thread());

    CPT(GeomVertexData) vdata = geom->get_vertex_data();

    GeomVertexReader vertex(vdata, "vertex");

    const int vertices_count = vdata->get_num_rows();
    std::vector<LVecBase3f> positions;
    positions.reserve(vertices_count);
    for (int k = 0; k < vertices_count; ++k)
        positions.push_back(vertex.get_data3f());

    std::vector<int> indices;
    indices.reserve(geom->get_num_primitives());
    for (int index=0, index_end=geom->get_num_primitives(); index < index_end; ++index)
    {
        const auto& primitive = geom->get_primitive(index);

        if (primitive->get_num_vertices_per_primitive() != 3)
        {
            rpcore::RPObject::global_error(RPPLUGIN_DEBUG_NAME,
                fmt::format("Geom (in {}) has non-triangle primitive.", geom_nodepath.get_name()));
            return;
        }

        indices.reserve(indices.size() + primitive->get_num_vertices());
        for (int k=0, k_end=primitive->get_num_vertices(); k < k_end; ++k)
            indices.push_back(primitive->get_vertex(k));
    }

    initilize(buffer, flex_libray, lower, upper, positions, indices, vertices_count, int(indices.size()),
        geom_nodepath.get_pos(rpcore::Globals::render),
        geom_nodepath.get_quat(rpcore::Globals::render),
        geom_nodepath.get_scale(rpcore::Globals::render));
}

inline NvFlexTriangleMeshId RPFlexTriangleMesh::get_triangle_mesh_id(const FlexBuffer& buffer) const
{
    return buffer.shape_geometry_[shape_buffer_index_].triMesh.mesh;
}

inline void RPFlexTriangleMesh::initilize(FlexBuffer& buffer, NvFlexLibrary* flex_libray, const LPoint3f& lower, const LPoint3f& upper,
    const std::vector<LVecBase3f>& positions, const std::vector<int>& indices, int vertices_count, int faces_count,
    const LPoint3f& translation, const LQuaternionf& rotation, const LVecBase3f& scale)
{
    NvFlexCollisionGeometry geo;
    geo.triMesh.mesh = create_flex_triangle_mesh(flex_libray, lower, upper, positions, indices, vertices_count, faces_count);
    geo.triMesh.scale[0] = scale[0];
    geo.triMesh.scale[1] = scale[1];
    geo.triMesh.scale[2] = scale[2];

    shape_buffer_index_ = buffer.shape_positions_.size();

    buffer.shape_positions_.push_back(LVecBase4f(translation, 0.0f));
    buffer.shape_rotations_.push_back(rotation);
    buffer.shape_prev_positions_.push_back(LVecBase4f(translation, 0.0f));
    buffer.shape_prev_rotations_.push_back(rotation);
    buffer.shape_geometry_.push_back((NvFlexCollisionGeometry&)geo);
    buffer.shape_flags_.push_back(NvFlexMakeShapeFlags(eNvFlexShapeTriangleMesh, false));
}

}
