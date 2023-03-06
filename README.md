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

Initialize the logger by:

Log Messages by:

Cleanup the logger by:
