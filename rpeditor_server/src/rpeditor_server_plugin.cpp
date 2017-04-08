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

#include "rpeditor_server_plugin.hpp"

#include <thread>

#include <boost/dll/alias.hpp>

#include <QtCore/QCoreApplication>

#include <render_pipeline/rpcore/pluginbase/base_plugin.h>

#include "restapi/restapi_server.hpp"

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "editor",
    PLUGIN_ID_STRING,
    "Render Pipeline Editor Server",
    "Younguk Kim <bluekyu.dev@gmail.com>",
    "0.1",

    "Plugin to communicate Render Pipeline Editor."
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<rpeditor::RPEditorServerPlugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

namespace rpeditor {

APIServerInterface* global_server = nullptr;

struct RPEditorServerPlugin::Impl
{
    static RequrieType require_plugins_;

    RestAPIServer* server_ = nullptr;

    std::unique_ptr<std::thread> network_thread_;
};

RPEditorServerPlugin::RequrieType RPEditorServerPlugin::Impl::require_plugins_;

RPEditorServerPlugin::RPEditorServerPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, plugin_info), impl_(std::make_unique<Impl>())
{
}

RPEditorServerPlugin::~RPEditorServerPlugin(void) = default;

RPEditorServerPlugin::RequrieType& RPEditorServerPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void RPEditorServerPlugin::on_load(void)
{
    impl_->network_thread_ = std::make_unique<std::thread>([this]() {
        info("Starting WebSocket server thread ...");

        int argc = 0;
        char* argv[] ={ nullptr };
        QCoreApplication app(argc, argv);

        global_server = impl_->server_ = new RestAPIServer(8888);
        QObject::connect(impl_->server_, &RestAPIServer::closed, &app, &QCoreApplication::quit);

        app.exec();

        delete impl_->server_;
        impl_->server_ = nullptr;
        global_server = nullptr;

        info("WebSocket server thread is done.");
    });
}

void RPEditorServerPlugin::on_unload(void)
{
    info("Closing WebSocket server thread ...");

    if (impl_->server_)
        impl_->server_->close();

    impl_->network_thread_->join();
    impl_->network_thread_.reset();
}

APIServerInterface& RPEditorServerPlugin::get_server(void) const
{
    return *impl_->server_;
}

}
