### Main VCPKG project dependencies
* mondradiko-deps/

### Custom Portfiles
* wasmtime-prebuilt/ -- hybrid port (fetches/installs wasmtime binary release)
* gamenetworkingsockets/ -- updated version of https://github.com/ValveSoftware/GameNetworkingSockets/tree/master/vcpkg_ports/gamenetworkingsockets

### Vulkan portfiles

These portfiles should be maintained as a set to avoid missing symbols.
(Khronos' Vulkan source projects inter-depend on each other in subtle ways)

* vulkan-headers/ -- required
* vulkan-loader/ -- required
* spirv-headers/ -- optional (validation)
* spirv-tools/ -- optional (validation)
* vulkan-validation/ -- optional (validation)
* vulkan-sdk/ -- composite placeholder; currently supports `vulkan-sdk[headers,loader,validation]``
