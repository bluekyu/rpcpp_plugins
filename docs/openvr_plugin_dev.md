# OpenVR Plugin Development

# Synchronization Timing
`WaitGetPoses` will be waiting the end of rendering (or rendering of submitted textures).
This can cause performance degradation if game logic has long time.

In OpenVR plugin, `WaitGetPoses` is performed in task with -50 sort
to guarantee correct behavior in normal cases.

## References and Sites
- https://github.com/ValveSoftware/openvr/wiki/IVRCompositor_Overview
- https://github.com/ValveSoftware/openvr/wiki/IVRSystem::GetDeviceToAbsoluteTrackingPose
- https://steamcommunity.com/app/358720/discussions/0/385429125016008605/
- https://steamcommunity.com/app/358720/discussions/0/517142253877107262/
- https://steamcommunity.com/app/358720/discussions/0/350532795335128039/
- https://steamcommunity.com/app/358720/discussions/0/405691491122676152/
- https://steamcommunity.com/app/358720/discussions/0/1699415798780168976/
- https://steamcommunity.com/app/358720/discussions/0/152391285460288350/
- https://steamcommunity.com/app/358720/discussions/0/351659808493203424/
