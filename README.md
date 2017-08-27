# Render Pipeline C++ Plugins

[![Build status](https://ci.appveyor.com/api/projects/status/hu8budi8m0ww6rdg/branch/master?svg=true)](https://ci.appveyor.com/project/bluekyu/rpcpp-plugins/branch/master)

This project is to develop some plugins for [Render Pipeline C++](https://github.com/bluekyu/render_pipeline_cpp).



## Documents
See `docs/index.md` documents.


## Plugins
### background2d
This plugin shows 2D image in background in mono OR stereoscopic rendering.



### (**Experimental**) Line Renderer
This plugin supports to draw 3D line.



### openvr
This plugin supports OpenVR (https://github.com/ValveSoftware/openvr)



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
