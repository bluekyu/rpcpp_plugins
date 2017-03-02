/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016, Center of human-centered interaction for coexistence.
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

#include <boost/dll/alias.hpp>

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "restapi/restapi_server.hpp"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
struct PluginInfo
{
    const char* id = PLUGIN_ID_STRING;
    const char* name = "Render Pipeline Editor Server";
    const char* author = "Younguk Kim <bluekyu.dev@gmail.com>";
    const char* description =
        "Plugin to communicate Render Pipeline Editor.";
    const char* version = "0.1";
};
BOOST_SYMBOL_EXPORT const PluginInfo plugin_info;

}

class Plugin: public rpcore::BasePlugin
{
public:
    static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline* pipeline)
    {
        return std::make_shared<Plugin>(pipeline);
    }

    Plugin(rpcore::RenderPipeline* pipeline): rpcore::BasePlugin(pipeline, plugin_info.id) {}
    ~Plugin(void) override;

    std::string get_name(void) const override;
    std::string get_author(void) const override;
    std::string get_description(void) const override;
    std::string get_version(void) const override;
    RequrieType& get_required_plugins(void) const override;

    void on_load(void) override;

private:
    static RequrieType require_plugins_;
};

Plugin::RequrieType Plugin::require_plugins_;

Plugin::~Plugin(void)
{
    restapi::RestAPIServer::close();
}

std::string Plugin::get_name(void) const
{
    return plugin_info.name;
}

std::string Plugin::get_author(void) const
{
    return plugin_info.author;
}

std::string Plugin::get_description(void) const
{
    return plugin_info.description;
}

std::string Plugin::get_version(void) const
{
    return plugin_info.version;
}

Plugin::RequrieType& Plugin::get_required_plugins(void) const
{
    return require_plugins_;
}

void Plugin::on_load(void)
{
    restapi::RestAPIServer::run();
}

// ************************************************************************************************

BOOST_DLL_ALIAS(
    Plugin::create_plugin,
    create_plugin
)
