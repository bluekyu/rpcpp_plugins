#include "restapi/resolve_message.hpp"

#include <iostream>

#include <boost/log/trivial.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

namespace restapi {

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
        BOOST_LOG_TRIVIAL(error) << "JSON parse error: " << rapidjson::GetParseError_En(result.Code());
        return;
    }

    if (!doc.IsObject())
    {
        BOOST_LOG_TRIVIAL(error) << "JSON Document is NOT object: " << restapi_message;
        return;
    }

    if (!doc.HasMember("resource"))
    {
        BOOST_LOG_TRIVIAL(error) << "Message has NOT 'resource': " << restapi_message;
        return;
    }

    const auto& resource = doc["resource"].GetString();
    auto& resolver_map = get_resolver_map();
    if (resolver_map.find(resource) != resolver_map.end())
    {
        resolver_map.at(resource)(doc);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "NO resolver for the resource: " << resource;
    }
}

}   // namespace restapi
