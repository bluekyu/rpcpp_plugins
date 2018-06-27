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

#include <windowFramework.h>
#include <geomNode.h>

#include <fmt/format.h>

#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/rpobject.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

#include "restapi/resolve_message.hpp"

#include "api_server_interface.hpp"

namespace rpeditor {

extern APIServerInterface* global_server;

void create_nodepath_json(const NodePath& np, rapidjson::Value& child_array, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator)
{
    rapidjson::Value self(rapidjson::kObjectType);

    self.AddMember("name", np.get_name(), allocator);
    self.AddMember("type", np.node()->get_type().get_name(), allocator);

    rapidjson::Value children(rapidjson::kArrayType);
    for (int k=0, k_end=np.get_num_children(); k < k_end; ++k)
    {
        create_nodepath_json(np.get_child(k), children, allocator);
    }

    self.AddMember("children", children, allocator);

    if (child_array.IsArray())
        child_array.PushBack(self, allocator);
}

bool resolve_showbase(const rapidjson::Document& doc)
{
    const std::string& method = doc["method"].GetString();
    if (method == RPEDITOR_API_CREATE_STRING)
    {
        rpcore::Globals::base->get_window_framework()->load_model(rpcore::Globals::render, Filename::from_os_specific(doc["message"]["path"].GetString()));
    }
    else if (method == RPEDITOR_API_READ_STRING)
    {
        rapidjson::Document doc;
        rapidjson::Value& message = init_document(doc, "ShowBase", RPEDITOR_API_UPDATE_STRING);
        auto& allocator = doc.GetAllocator();

        const NodePath& np = rpcore::Globals::render;
        rapidjson::Value root(rapidjson::kObjectType);
        root.AddMember("name", np.get_name(), allocator);
        root.AddMember("type", np.node()->get_type().get_name(), allocator);
        rapidjson::Value children(rapidjson::kArrayType);
        for (int k=0, k_end=np.get_num_children(); k < k_end; ++k)
            create_nodepath_json(np.get_child(k), children, allocator);
        root.AddMember("children", children, allocator);

        message.AddMember("root", root, allocator);

        global_server->broadcast(doc);
    }
    else
    {
        rpcore::RPObject::global_error("plugin::" RPPLUGINS_ID_STRING, fmt::format("Unknown method: ", method));
        return false;
    }

    return true;
}

// ************************************************************************************************

ConfigureStaticInit(ShowBase)
{
    auto& resolver_map = get_resolver_map();
    resolver_map["ShowBase"] = resolve_showbase;
}

}    // namespace rpeditor
