#include <boost/log/trivial.hpp>

#include <windowFramework.h>
#include <geomNode.h>

#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rppanda/showbase/showbase.h>

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
        BOOST_LOG_TRIVIAL(error) << "Unknown method: " << method;
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

}	// namespace rpeditor
