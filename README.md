# papyrus-vm

## Building


### The first launch
```batch
mkdir papyrus-vm-build
cd papyrus-vm-build
cmake ../papyrus-vm -DSKYRIM_DIR="C:\Program Files (x86)\Steam\steamapps\common\Skyrim"
```
CMake will generate `.sln` file in `papyrus-vm-build` directory.

### CMake re-generation
You need to re-generate project files when:

 1. New `.cpp` / `.h` files are added.
 2. New tests are added.

```batch
cmake ../papyrus-vm
```

### Running tests
```batch
cd papyrus-vm-build
ctest -C Debug -V
ctest -R IntTest -C Debug -V
```
See also [CTest docs](https://cmake.org/cmake/help/v3.15/manual/ctest.1.html)
