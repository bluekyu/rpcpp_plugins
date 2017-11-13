#include "openvr_controller.hpp"

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
