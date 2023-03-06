# loggeru
A Well rounded c++ Logging solution

## system dependencies
- (fmt)[https://github.com/fmtlib/fmt]
- std++17 or higher. 

> **Warning**
> 
> **For windows** it will use (vcpkg)[https://vcpkg.io/en/index.html] to find it's dependencies. 
> If you do not have `vcpkg` installed in the default install location (`C:/Program Files (x86)/vcpkg`), <br>
> then you can specify it's path during cmake configuration step with the `--DVCPKG_PATH=<path_to_vcpkg_root>` variable
> 
> **For Linux** it expects these libraries to be installed on a system level using a package manager such as `apt`

## Usage

### Initialize the logger by calling:
```cpp
Loggeru::Initialize();
```

### Enable file logging by calling:
```cpp
Loggeru::StartFileLogging();
```
> **Note** <br>
> default filename: `Log_{day}_{month}_{year}_{hours}_{minutes}_{seconds}.log`

OR 
```cpp
Loggeru::StartFileLogging("filename.log");
```

### Stop file logging by calling:
```cpp
Loggeru::StopFileLogging();
```

### Log Messages by using macros:
```cpp
LOG_DEBUG("My debug message");
LOG_INFO("My info message");
LOG_WARNING("My warning message");
LOG_ERROR("My error message");
LOG_CRITICAL("My critical error message");
```

Or use formatted equivalents by adding `_`<br>
eg. 
```cpp
std::string filepath = "some bad file path";
LOG_CRITICAL_("Tried loading config file but file at path {} could not be found", filepath);
```

## Cleanup the logger by calling:
```cpp
Loggeru::Release();
```
