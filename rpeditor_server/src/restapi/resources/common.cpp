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

#include "restapi/resources/common.hpp"

#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/rpobject.hpp>

namespace rpeditor {

NodePath get_nodepath(const rapidjson::Value& scene_path)
{
    if (!scene_path.IsArray())
    {
        rpcore::RPObject::global_error("plugin::" RPPLUGINS_ID_STRING, "Scene path is NOT array of indices.");
        return NodePath();
    }

    NodePath np = rpcore::Globals::render;
    for (const auto& index: scene_path.GetArray())
    {
        if (index.GetInt() < np.get_num_children())
        {
            np = np.get_child(index.GetInt());
        }
        else
        {
            rpcore::RPObject::global_error("plugin::" RPPLUGINS_ID_STRING, "Out of range of children.");
            return NodePath();
        }
    }

    return np;
}

}    // namespace rpeditor
