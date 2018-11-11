# Render Pipeline C++ Plugins

This project is to develop some plugins for [Render Pipeline C++](https://github.com/bluekyu/render_pipeline_cpp).

#### Build Status
| Build Status                       | About                      |
| :--------------------------------: | :------------------------: |
| [![azure-badge]][azure-link]       | Visual Studio 2017         |
| [![appveyor-badge]][appveyor-link] | Visual Studio 2017 Preview |

[azure-badge]: https://dev.azure.com/bluekyu/rpcpp-devops/_apis/build/status/render_pipeline_cpp/rpcpp_plugins "Azure build status"
[azure-link]: https://dev.azure.com/bluekyu/rpcpp-devops/_build/latest?definitionId=6 "Azure build link"
[appveyor-badge]: https://ci.appveyor.com/api/projects/status/hu8budi8m0ww6rdg/branch/master?svg=true "AppVeyor build status"
[appveyor-link]: https://ci.appveyor.com/project/bluekyu/rpcpp-plugins/branch/master "AppVeyor build link"

You can download built files from each Build Page.



## Plugins
### background2d
This plugin shows 2D image in background in mono OR stereoscopic rendering.



### (**Experimental**) Line Renderer
This plugin supports to draw 3D line.



### openvr
This plugin supports OpenVR (https://github.com/ValveSoftware/openvr).
See [openvr/README.md](openvr/README.md) for details.



### (**Experimental**) recording
This plugin is to load texture or framebuffer to CPU and save it.



### rpflex
This plugin runs NVIDIA FleX library (https://developer.nvidia.com/flex)
which is particle based physics engine.

This plugin uses NVIDIA FleX library and some codes using FleX use the demo example.
(See `rpflex/NVIDIA-FLEX-LICENSE.txt` file.)

But it does NOT have binary files (ex, DLL) and SDK files (ex, headers).

If you want to use this plugin, you can get the SDK from
NVIDIA FleX site (https://developer.nvidia.com/flex).




### (**Experimental**) vivesr
This plugin supports ViveSR (SRWorks) (https://developer.vive.com/resources/)



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
