#include <boost/log/trivial.hpp>

#include <geomNode.h>

#include <render_pipeline/rpcore/globals.h>

#include "restapi/resources/common.hpp"
#include "restapi/resolve_message.hpp"
#include "restapi/restapi_server.hpp"

namespace restapi {

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
            BOOST_LOG_TRIVIAL(error) << "This node is NOT GeomNode: " << np;
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

        RestAPIServer::get_instance()->broadcast(new_doc);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Unknown method: " << method;
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

}   // namespace restapi
