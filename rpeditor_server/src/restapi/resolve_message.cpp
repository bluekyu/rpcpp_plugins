#include "restapi/resolve_message.hpp"

#include <iostream>

#include <boost/log/trivial.hpp>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>

#include <fmt/format.h>

#include <render_pipeline/rpcore/rpobject.h>

namespace rpeditor {

ResolverMapType& get_resolver_map(void)
{
    static ResolverMapType resolver_map;
    return resolver_map;
}

void resolve_message(const std::string& restapi_message)
{
    rapidjson::Document doc;
    rapidjson::ParseResult result = doc.Parse(restapi_message.c_str());

    if (!result)
    {
        rpcore::RPObject::global_error("plugin::" PLUGIN_ID_STRING, fmt::format("JSON parse error: {}", rapidjson::GetParseError_En(result.Code())));
        return;
    }

    if (!doc.IsObject())
    {
        rpcore::RPObject::global_error("plugin::" PLUGIN_ID_STRING, fmt::format("JSON Document is NOT object: {}", restapi_message));
        return;
    }

    if (!doc.HasMember("resource"))
    {
        rpcore::RPObject::global_error("plugin::" PLUGIN_ID_STRING, fmt::format("Message has NOT 'resource': {}", restapi_message));
        return;
    }

    const auto& resource = doc["resource"].GetString();
    auto& resolver_map = get_resolver_map();
    if (resolver_map.find(resource) != resolver_map.end())
    {
        if (!resolver_map.at(resource)(doc))
        {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            doc.Accept(writer);

            rpcore::RPObject::global_trace("plugin::" PLUGIN_ID_STRING, std::string(buffer.GetString(), buffer.GetSize()));
        }
    }
    else
    {
        rpcore::RPObject::global_error("plugin::" PLUGIN_ID_STRING, fmt::format("NO resolver for the resource: {}", resource));
    }
}

}   // namespace rpeditor
