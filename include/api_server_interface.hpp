#pragma once

#include <rapidjson/document.h>

namespace rpeditor {

class APIServerInterface
{
public:
    virtual void broadcast(const std::string& json_msg) = 0;
    virtual void broadcast(const rapidjson::Document& doc) = 0;
};

}
