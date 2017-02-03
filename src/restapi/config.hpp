#pragma once

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>

namespace restapi {

#define RESTAPI_RESOURCE_STRING "resource"
#define RESTAPI_METHOD_STRING "method"
#define RESTAPI_MESSAGE_STRING "message"

#define ConfigureStaticInit(name) \
    class StaticInit ## name \
    { \
    public: \
        StaticInit ## name (); \
    }; \
    static StaticInit ## name name_; \
    StaticInit ## name:: StaticInit ## name()


/**
 * @return 'message' JSON object.
 */
inline rapidjson::Value& init_document(rapidjson::Document& doc, const std::string& resource, const std::string& method)
{
    auto& allocator = doc.GetAllocator();
    doc.SetObject();

    doc.AddMember(RESTAPI_RESOURCE_STRING, resource, allocator);
    doc.AddMember(RESTAPI_METHOD_STRING, method, allocator);
    doc.AddMember(RESTAPI_MESSAGE_STRING, rapidjson::Value(rapidjson::kObjectType).Move(), allocator);
    return doc["message"];
}

}   // namespace restapi
