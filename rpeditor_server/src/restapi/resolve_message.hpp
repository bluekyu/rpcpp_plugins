#pragma once

#include <string>
#include <unordered_map>

#include <spdlog/logger.h>

#include "restapi/config.hpp"

namespace rpeditor {

using ResolverMapType = std::unordered_map<std::string, bool(*)(const rapidjson::Document&)>;

ResolverMapType& get_resolver_map(void);

void resolve_message(const std::string& restapi_message);

}   // namespace rpeditor
