# shore-backend-wtl

The Windows Template Library (WTL) peer backend for Shore. It requires the
backend-neutral `shore` core, WTL headers, and a Windows MSVC toolchain.

```cmake
find_package(shore-backend-wtl CONFIG REQUIRED)
target_link_libraries(app PRIVATE shore-backend-wtl::shore-backend-wtl)
```

Construct the app with `shore::backend::wtl::CreateBackend()`. Set `WTL_ROOT`
when configuring CMake if `atlapp.h` is not discoverable. The repository
contains a vcpkg port under `ports/shore-backend-wtl` and a Windows Actions build.
