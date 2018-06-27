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

#include <geomNode.h>

#include <fmt/format.h>

#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/rpobject.hpp>

#include "restapi/resources/common.hpp"
#include "restapi/resolve_message.hpp"

#include "api_server_interface.hpp"

namespace rpeditor {

extern APIServerInterface* global_server;

bool resolve_gemonode(const rapidjson::Document& doc)
{
    const std::string& method = doc["method"].GetString();
    if (method == RPEDITOR_API_READ_STRING)
    {
        const NodePath& np = get_nodepath(doc["message"]["path"]);
        if (!np)
            return false;

        GeomNode* gnode = DCAST(GeomNode, np.node());
        if (!gnode)
        {
            rpcore::RPObject::global_error("plugin::" RPPLUGINS_ID_STRING, fmt::format("This node is NOT GeomNode: ", np.get_name()));
            return false;
        }

        int geom_index = (std::min)(gnode->get_num_geoms()-1, doc["message"]["index"].GetInt());

        rapidjson::Document new_doc;
        rapidjson::Value& message = init_document(new_doc, "GeomNode", RPEDITOR_API_UPDATE_STRING);
        auto& allocator = new_doc.GetAllocator();

        message.AddMember("num_geoms", gnode->get_num_geoms(), allocator);
        message.AddMember("index", geom_index, allocator);

        CPT(Geom) geom = gnode->get_geom(geom_index);

        message.AddMember("num_primitives", geom->get_num_primitives(), allocator);

        global_server->broadcast(new_doc);
    }
    else
    {
        rpcore::RPObject::global_error("plugin::" RPPLUGINS_ID_STRING, fmt::format("Unknown method: ", method));
        return false;
    }

    return true;
}

// ************************************************************************************************

ConfigureStaticInit(GeomNode)
{
    auto& resolver_map = get_resolver_map();
    resolver_map["GeomNode"] = resolve_gemonode;
}

}   // namespace rpeditor
