# X-Plane Python3 Plugin "XPPython3"

:warning:_CAUTION_: Work-in-progress:warning:

This is an updated version of Sandy Barbour's excellent Python Interface plugin, allowing you to create plugins for X-Plane using Python. Sandy's was restricted to Python2, this is the update to support python3.

It's called XPPython3:
1. "xpython" already exists (it's an all python version of python)
2. "3" to emphasize it is python3, which is not fully backward compatible with python2.

Key features:
* **X-Plane 11.50** _minimum_: There is no attempt to make this work on older versions of X-Plane. This plugin can co-exist with the older plugin, so if Python2 is still available it can be used with the older plugin. (Python2 is end-of-life.)
* **64-bit only**: X-Plane is now 64-bit only.
* **X-Plane SDK 303** _minimum_: This is base SDK version for 11.50. Vulkan/Metal and OpenGL are supported.
* **Python3 only**: This will not work with python2 programs which are not compatible with Python3. Note that this will 
support _any_ Python3 minor version (greater than 3.3). Unlike previous python plugins, you won't need one plugin version for Python37 and a different one for Python38, etc.

## Building
See [README_BUILD](XPython/README_BUILD.md) for build instructions. 

- Linux, tested on Ubuntu18 with gnu compiler,
- Mac, developed and tested on Catalina 10.15 with Xcode compiler (command line only), 
- Windows, tested on Windows 10 with mingw-w64 tools. Might be compilable with VisualStudio, but I don't care.

Ultimately, we'll be providing a set of pre-built binaries for distribution. (Separate user-facing website not set yet.)

## Installation
See [README_INSTALL](XPython/README_INSTALL.md) for user installation instructions. This will be part of the user-facing website.

## Development of Plugins
See [README_SDK](XPython/README_SDK.txt) for additional information on building your own plugins using Python. We'll include differences between the older Python2 plugin interface and this Python3 version.

### Python Stubs
Because the XPLM* modules are contained within a shared library, they are not useful for support tools such as pylint. For this reason, we've included a set of stubs. Include the stub directory as part of your python path (during development) and pylint will be useful. For example, include this in `~/.pylintrc`:

    [MASTER]                                                                                    
    init-hook="import sys;sys.path.extend(['.', '/path_to_stubs/PythonStubs'])"

### Examples
Python code exercising each interface is in [XPython/examples](XPython/examples) directory, organized primarily one example per module.
