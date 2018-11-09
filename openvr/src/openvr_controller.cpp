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

#include "rpplugins/openvr/controller.hpp"

namespace rpplugins {

TypeHandle OpenVRController::type_handle_;

void OpenVRController::do_transmit_data(DataGraphTraverser* trav,
    const DataNodeTransmit&,
    DataNodeTransmit& output)
{
    // Process SteamVR controller state
    for (vr::TrackedDeviceIndex_t index = 0; index < vr::k_unMaxTrackedDeviceCount; ++index)
    {
        vr::VRControllerState_t state;
        if (!HMD_->GetControllerState(index, &state, sizeof(state)))
            continue;

        if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_System))
        {
        }

        if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu))
        {
        }

        if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip))
        {
        }

        for (uint32_t k = 0; k < vr::k_unControllerStateAxisCount; ++k)
        {
            auto button_id = static_cast<vr::EVRButtonId>(vr::EVRButtonId::k_EButton_Axis0 + k);

            if (state.ulButtonPressed & vr::ButtonMaskFromId(button_id))
            {

            }
        }
    }
}

}
