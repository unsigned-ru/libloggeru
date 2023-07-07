# libloggeru
A well rounded c++ Logging solution

## Library Dependencies (installed as shared libraries on the system)
- std++20 or higher
- [fmt](https://github.com/fmtlib/fmt)
- [**WINDOWS ONLY**] [vcpkg](https://vcpkg.io/en/index.html)

> **Warning**
> 
> **For windows** it will use [vcpkg](https://vcpkg.io/en/index.html) to find it's dependencies. 
> If you do not have `vcpkg` installed in the default install location (`C:/Program Files (x86)/vcpkg`), <br>
> then you can specify it's path during cmake configuration step with the `--DVCPKG_PATH=<path_to_vcpkg_root>` variable
> 
> **For Linux** it expects these libraries to be installed on a system level using a package manager such as `apt`

## Installation
### 1. Clone repo
```
git clone https://github.com/unsigned-ru/libloggeru.git
```
### 2. Configure project
```
cd libloggeru && 
mkdir out &&
cd out &&
cmake .. -DCMAKE_BUILD_TYPE=<Debug/Release> -DCMAKE_C_COMPILER=gcc-13 -DCMAKE_CXX_COMPILER=g++-13 (windows only: -DVCPKG_PATH="<non-default path to VCPKG>)"
```
### 3. Build project
```
cmake --build .
```

### 4. Install project
```
cmake --install .
```

> **Warning**
> 
> **For windows** the installation location will be in {VCPKG_PATH}/installed/windows-{ARCHITECTURE:(x64/x86)}
>
> **For UNIX** the installation location will be in the default platform library installtion location.

## Coding standard
The library follows a near-identical coding standard to [dpp's coding standards](https://dpp.dev/coding-standards.html)

## Usage
### Initialize the logger by calling:
```cpp
loggeru::logger_t::initialize();
```

### Enable file logging by calling:
```cpp
loggeru::logger_t::start_file_logging();
```
> **Note** <br>
> default filename: `Log_{day}_{month}_{year}_{hours}_{minutes}_{seconds}.log`

OR 
```cpp
loggeru::logger_t::start_file_logging("filename.log");
```

### Stop file logging by calling:
```cpp
loggeru::logger_t::stop_file_logging();
```

### Log Messages by using macros:
```cpp
LOG_DEBUG("My debug message");
LOG_INFO("My info message");
LOG_WARNING("My warning message");
LOG_ERROR("My error message");
LOG_CRITICAL("My critical error message"); //Will throw std::exception&
```

Or use formatted equivalents by adding `_`<br>
eg. 
```cpp
std::string file_path = "some bad file path";
LOG_CRITICAL_("Tried loading config file but file at path {} could not be found", file_path);
```

### Cleanup the logger by calling:
```cpp
loggeru::logger_t::release();
```
