# Render Pipeline C++ Plugins

This project is to develop some plugins for [Render Pipeline C++](https://github.com/bluekyu/render_pipeline_cpp).

#### Build Status
| OS       | Build Status           | Latest Build                                                                    |
| :------: | :--------------------: | :-----------------------------------------------------------------------------: |
| Windows  | [![ci-badge]][ci-link] | vc141 ([Debug][vc141-debug], [Release][vc141-release])                          |

[ci-badge]: https://ci.appveyor.com/api/projects/status/hu8budi8m0ww6rdg/branch/master?svg=true "AppVeyor build status"
[ci-link]: https://ci.appveyor.com/project/bluekyu/rpcpp-plugins/branch/master "AppVeyor build link"
[vc141-debug]: https://ci.appveyor.com/api/projects/bluekyu/rpcpp-plugins/artifacts/rpcpp_plugins.7z?branch=master&job=Configuration%3A+Debug "Download latest vc141 build (Debug)"
[vc141-release]: https://ci.appveyor.com/api/projects/bluekyu/rpcpp-plugins/artifacts/rpcpp_plugins.7z?branch=master&job=Configuration%3A+Release "Download latest vc141 build (Release)"



## Plugins
### background2d
This plugin shows 2D image in background in mono OR stereoscopic rendering.



### imgui
This plugin supports to use ImGui. See [imgui/README.md](imgui/README.md) for details.



### (**Experimental**) Line Renderer
This plugin supports to draw 3D line.



### openvr
This plugin supports OpenVR (https://github.com/ValveSoftware/openvr).
See [openvr/README.md](openvr/README.md) for details.



### (**Experimental**) rpeditor_server
This plugin runs websocket server to communicate data with another clients (ex, editor).



### rpflex
This plugin runs NVIDIA FleX library (https://developer.nvidia.com/flex)
which is particle based physics engine.

This plugin uses NVIDIA FleX library and some codes using FleX use the demo example.
(See `rpflex/NVIDIA-FLEX-LICENSE.txt` file.)

But it does NOT have binary files (ex, DLL) and SDK files (ex, headers).

If you want to use this plugin, you can get the SDK from
NVIDIA FleX site (https://developer.nvidia.com/flex).



### (**Experimental**) rpstat
This plugin draws Status GUI using imgui.



## Build
Just run CMake.

You can build with Render Pipeline C++ project and samples.
See [Building Render Pipeline C++](https://github.com/bluekyu/render_pipeline_cpp/blob/master/docs/build_rpcpp.md)



## Related Projects
- Panda3D Third-party: https://github.com/bluekyu/panda3d-thirdparty
- (patched) Panda3D: https://github.com/bluekyu/panda3d
- Render Pipeline C++: https://github.com/bluekyu/render_pipeline_cpp
- Samples for Render Pipeline C++: https://github.com/bluekyu/rpcpp_samples



## License
See `LICENSE.md` file.
