#pragma once

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>

namespace rpeditor {

#define RPEDITOR_API_RESOURCE_STRING "resource"
#define RPEDITOR_API_METHOD_STRING "method"
#define RPEDITOR_API_MESSAGE_STRING "message"

#define RPEDITOR_API_CREATE_STRING "CREATE"
#define RPEDITOR_API_READ_STRING "READ"
#define RPEDITOR_API_UPDATE_STRING "UPDATE"

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

    doc.AddMember(RPEDITOR_API_RESOURCE_STRING, resource, allocator);
    doc.AddMember(RPEDITOR_API_METHOD_STRING, method, allocator);
    doc.AddMember(RPEDITOR_API_MESSAGE_STRING, rapidjson::Value(rapidjson::kObjectType).Move(), allocator);
    return doc[RPEDITOR_API_MESSAGE_STRING];
}

}   // namespace rpeditor
