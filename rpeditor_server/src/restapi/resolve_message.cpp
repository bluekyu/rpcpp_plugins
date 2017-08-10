/**
 * MIT License
 * 
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "restapi/resolve_message.hpp"

#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>

#include <fmt/format.h>

#include <render_pipeline/rpcore/rpobject.hpp>

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
        rpcore::RPObject::global_error("plugin::" RPPLUGIN_ID_STRING, fmt::format("JSON parse error: {}", rapidjson::GetParseError_En(result.Code())));
        return;
    }

    if (!doc.IsObject())
    {
        rpcore::RPObject::global_error("plugin::" RPPLUGIN_ID_STRING, fmt::format("JSON Document is NOT object: {}", restapi_message));
        return;
    }

    if (!doc.HasMember("resource"))
    {
        rpcore::RPObject::global_error("plugin::" RPPLUGIN_ID_STRING, fmt::format("Message has NOT 'resource': {}", restapi_message));
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

            rpcore::RPObject::global_trace("plugin::" RPPLUGIN_ID_STRING, std::string(buffer.GetString(), buffer.GetSize()));
        }
    }
    else
    {
        rpcore::RPObject::global_error("plugin::" RPPLUGIN_ID_STRING, fmt::format("NO resolver for the resource: {}", resource));
    }
}

}   // namespace rpeditor
