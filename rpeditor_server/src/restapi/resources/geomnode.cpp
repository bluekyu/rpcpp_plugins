#include <geomNode.h>

#include <fmt/format.h>

#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/rpobject.h>

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
            rpcore::RPObject::global_error("plugin::" RPPLUGIN_ID_STRING, fmt::format("This node is NOT GeomNode: ", np.get_name()));
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
        rpcore::RPObject::global_error("plugin::" RPPLUGIN_ID_STRING, fmt::format("Unknown method: ", method));
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
