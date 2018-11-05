/**
 * MIT License
 *
 * Copyright (c) 2018 Center of Human-centered Interaction for Coexistence
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

#include "rpplugins/recording/plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>

#include <fmt/ostream.h>

#include "rpplugins/recording/recording_stage.hpp"

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::RecordingPlugin)

namespace rpplugins {

rpcore::BasePlugin::RequrieType RecordingPlugin::require_plugins_;

RecordingPlugin::RecordingPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGINS_ID_STRING)
{
}

rpcore::BasePlugin::RequrieType& RecordingPlugin::get_required_plugins(void) const
{
    return require_plugins_;
}

void RecordingPlugin::on_stage_setup(void)
{
    add_stage(std::make_unique<RecordingStage>(pipeline_));
}

}
