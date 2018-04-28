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

#include <rpflex/flex_buffer.hpp>

namespace rpflex {

class RPFlexShape
{
public:
    virtual ~RPFlexShape() {}

    int get_shape_buffer_index() const;
    int get_shape_flag(const FlexBuffer& buffer) const;

    NvFlexCollisionShapeType get_collision_shape_type(const FlexBuffer& buffer) const;
    const LQuaternionf& get_prev_rotation(const FlexBuffer& buffer) const;
    const LVecBase4f& get_prev_position(const FlexBuffer& buffer) const;
    const NvFlexCollisionGeometry& get_collision_geometry(const FlexBuffer& buffer) const;

    LMatrix4f get_transform(const FlexBuffer& buffer) const;

protected:
    int shape_buffer_index_;
};

// ************************************************************************************************
inline int RPFlexShape::get_shape_buffer_index() const
{
    return shape_buffer_index_;
}

inline int RPFlexShape::get_shape_flag(const FlexBuffer& buffer) const
{
    return buffer.shape_flags[shape_buffer_index_];
}

inline NvFlexCollisionShapeType RPFlexShape::get_collision_shape_type(const FlexBuffer& buffer) const
{
    return NvFlexCollisionShapeType(get_shape_flag(buffer) & eNvFlexShapeFlagTypeMask);
}

inline const LQuaternionf& RPFlexShape::get_prev_rotation(const FlexBuffer& buffer) const
{
    return buffer.shape_prev_rotations[shape_buffer_index_];
}

inline const LVecBase4f& RPFlexShape::get_prev_position(const FlexBuffer& buffer) const
{
    return buffer.shape_prev_positions[shape_buffer_index_];
}

inline const NvFlexCollisionGeometry& RPFlexShape::get_collision_geometry(const FlexBuffer& buffer) const
{
    return buffer.shape_geometry[shape_buffer_index_];
}

inline LMatrix4f RPFlexShape::get_transform(const FlexBuffer& buffer) const
{
    // render with prev positions to match particle update order
    // can also think of this as current/next
    const LQuaternionf& rotation = get_prev_rotation(buffer);
    const LVecBase3f& position = get_prev_position(buffer).get_xyz();

    const NvFlexCollisionGeometry& geo = get_collision_geometry(buffer);

    LMatrix4f mat;
    switch (get_collision_shape_type(buffer))
    {
        case eNvFlexShapeSphere:
        {
            break;
        }
        case eNvFlexShapeCapsule:
        {
            break;
        }
        case eNvFlexShapeBox:
        {
            compose_matrix(mat,
                LVecBase3f(geo.box.halfExtents[0], geo.box.halfExtents[1], geo.box.halfExtents[2]) * 2.0f,
                LVecBase3f::zero(),
                rotation.get_hpr(),
                position);
            break;
        }
        case eNvFlexShapeConvexMesh:
        {
            break;
        }
        case eNvFlexShapeTriangleMesh:
        {
            break;
        }
        case eNvFlexShapeSDF:
        {
            break;
        }
    }

    return mat;
}

}
