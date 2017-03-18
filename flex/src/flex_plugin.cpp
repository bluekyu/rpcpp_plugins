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

#include "../include/flex_plugin.hpp"

#include <boost/dll/alias.hpp>

#include <NvFlex.h>
#include <NvFlexDevice.h>

extern "C" {

/** Plugin information for native DLL loader (ex. Python ctypes). */
BOOST_SYMBOL_EXPORT const rpcore::BasePlugin::PluginInfo plugin_info = {
    "physics",
    PLUGIN_ID_STRING,
    "FlexPlugin",
    "Younguk Kim <yukim@chic.re.kr>",
    "0.1 alpha",

    "NVIDIA Flex plugin."
};

}

static std::shared_ptr<rpcore::BasePlugin> create_plugin(rpcore::RenderPipeline& pipeline)
{
    return std::make_shared<FlexPlugin>(pipeline);
}
BOOST_DLL_ALIAS(::create_plugin, create_plugin)

// ************************************************************************************************

struct FlexPlugin::Impl
{
    static RequrieType require_plugins_;

    NvFlexLibrary* library_ = nullptr;
};

FlexPlugin::RequrieType FlexPlugin::Impl::require_plugins_;

// ************************************************************************************************

FlexPlugin::FlexPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, plugin_info), impl_(new Impl)
{
}

FlexPlugin::RequrieType& FlexPlugin::get_required_plugins(void) const
{
    return impl_->require_plugins_;
}

void FlexPlugin::on_load(void)
{
    // use the PhysX GPU selected from the NVIDIA control panel
    int device_index = NvFlexDeviceGetSuggestedOrdinal();

    // Create an optimized CUDA context for Flex and set it on the 
    // calling thread. This is an optional call, it is fine to use 
    // a regular CUDA context, although creating one through this API
    // is recommended for best performance.
    bool success = NvFlexDeviceCreateCudaContext(device_index);

    if (!success)
    {
        std::cout << "Error creating CUDA context." << std::endl;
        return;
    }

    NvFlexInitDesc desc;
    desc.deviceIndex = device_index;
    desc.enableExtensions = true;
    desc.renderDevice = 0;
    desc.renderContext = 0;
    desc.computeType = eNvFlexCUDA;

    // Init Flex library, note that no CUDA methods should be called before this 
    // point to ensure we get the device context we want
    impl_->library_ = NvFlexInit(NV_FLEX_VERSION, [](NvFlexErrorSeverity, const char* msg, const char* file, int line) {
        RPObject::global_error(PLUGIN_ID_STRING, std::string(msg) + " - " + std::string(file) + ":" + std::to_string(line));
    }, &desc);

    if (!impl_->library_)
    {
        error("Could not initialize Flex, exiting.\n");
        return;
    }

    // store device name
    info(std::string("Compute Device: ") + NvFlexGetDeviceName(impl_->library_));
}

void FlexPlugin::on_stage_setup(void)
{
}

void FlexPlugin::on_pipeline_created(void)
{
}
