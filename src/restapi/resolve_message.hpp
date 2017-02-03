#pragma once

#include <string>
#include <unordered_map>

#include "restapi/config.hpp"

namespace restapi {

using ResolverMapType = std::unordered_map<std::string, void(*)(const rapidjson::Document&)>;

ResolverMapType& get_resolver_map(void);

void resolve_message(const std::string& restapi_message);

}   // namespace restapi
