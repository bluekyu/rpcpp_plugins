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

#include <rpflex/plugin.hpp>
#include <rpflex/utils/shape.hpp>

namespace rpflex {

class RPFlexShapeBox: public RPFlexShape
{
public:
    RPFlexShapeBox(Plugin& rpflex_plugin, const LVecBase3f& half_edge=LVecBase3f(2.0f), const LVecBase3f& center=LVecBase3f(0.0f),
        const LQuaternionf& quat=LQuaternionf::ident_quat(), bool dynamic=false);
};

// ************************************************************************************************
inline RPFlexShapeBox::RPFlexShapeBox(Plugin& rpflex_plugin, const LVecBase3f& half_edge, const LVecBase3f& center, const LQuaternionf& quat, bool dynamic)
{
    auto& buffer = rpflex_plugin.get_flex_buffer();

    shape_buffer_index_ = buffer.shape_positions.size();

    buffer.shape_positions.push_back(LVecBase4f(center, 0.0f));
    buffer.shape_rotations.push_back(quat);

    buffer.shape_prev_positions.push_back(buffer.shape_positions.back());
    buffer.shape_prev_rotations.push_back(buffer.shape_rotations.back());

    NvFlexCollisionGeometry geo;
    geo.box.halfExtents[0] = half_edge[0];
    geo.box.halfExtents[1] = half_edge[1];
    geo.box.halfExtents[2] = half_edge[2];

    buffer.shape_geometry.push_back(geo);
    buffer.shape_flags.push_back(NvFlexMakeShapeFlags(eNvFlexShapeBox, dynamic));
}

}
