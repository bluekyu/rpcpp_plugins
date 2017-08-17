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

#include "rpeditor_server_plugin.hpp"

#include <thread>

#include <boost/dll/alias.hpp>

#include <QtCore/QCoreApplication>

#include <render_pipeline/rpcore/pluginbase/base_plugin.hpp>

#include "restapi/restapi_server.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpeditor::RPEditorServerPlugin)

// ************************************************************************************************

namespace rpeditor {

APIServerInterface* global_server = nullptr;

class RPEditorServerPlugin::Impl
{
public:
    static RequrieType require_plugins_;

    RestAPIServer* server_ = nullptr;

    std::unique_ptr<std::thread> network_thread_;
};

RPEditorServerPlugin::RequrieType RPEditorServerPlugin::Impl::require_plugins_;

RPEditorServerPlugin::RPEditorServerPlugin(rpcore::RenderPipeline& pipeline): rpcore::BasePlugin(pipeline, RPPLUGIN_ID_STRING), impl_(std::make_unique<Impl>())
{
}

RPEditorServerPlugin::~RPEditorServerPlugin() = default;

RPEditorServerPlugin::RequrieType& RPEditorServerPlugin::get_required_plugins() const
{
    return impl_->require_plugins_;
}

void RPEditorServerPlugin::on_load()
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

void RPEditorServerPlugin::on_unload()
{
    info("Closing WebSocket server thread ...");

    if (impl_->server_)
        impl_->server_->close();

    impl_->network_thread_->join();
    impl_->network_thread_.reset();
}

APIServerInterface& RPEditorServerPlugin::get_server() const
{
    return *impl_->server_;
}

}
