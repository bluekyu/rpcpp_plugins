/**
 * MIT License
 *
 * Copyright (c) 2018 Younguk Kim (bluekyu)
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

#include "window_interface.hpp"

namespace rpplugins {

class ScenegraphWindow : public WindowInterface
{
public:
    static constexpr const char* NODE_SELECTED_EVENT_NAME = "rpstat-scenegraph-selected";

public:
    ScenegraphWindow(RPStatPlugin& plugin, NodePath axis_model);

    void draw() final;
    void draw_contents() final;

private:
    void draw_nodepath(NodePath np);
    void draw_geomnode(GeomNode* node);
    void change_selected_nodepath(NodePath np);

    NodePath selected_np_;
    const Geom* selected_geom_ = nullptr;

    NodePath root_;
    NodePath axis_model_;
};

}
