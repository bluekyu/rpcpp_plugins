# OpenVR Plugin

## Requirements
- fmt
- Boost: filesystem
- OpenVR

### Guide
- Windows
  ```
  vcpkg install --triplet x64-windows fmt boost-filesystem openvr
  ```

#### OpenVR
You can also use a directory structure in OpenVR Project (https://github.com/ValveSoftware/openvr)
In CMake, set `OpenVR_ROOT` to OpenVR directory.



## Usage
Enable `openvr` plugin and enable stereo rendering in `pipeline.yaml`.

See documents and OpenVR samples in [rpcpp_samples](https://github.com/bluekyu/rpcpp_samples) repository.

### Documents
- [OpenVR](docs/openvr.md)
- [OpenVR Plugin Development](docs/openvr_plugin_dev.md)
