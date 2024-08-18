# Getting Started

## Cloning
Clone using

```shell
git clone --recurse-submodules https://github.com/Carlyle-Foster/Image-Tool.git
```

## Building
To build the project simply run 

```shell
make
```

if you're on windows you may have to install [GNU make](https://www.gnu.org/software/make/) first.\
Note that the makefile requires [CMake](https://cmake.org/) to automatically build raylib, see below.
##### Building without CMake
If you don't have/want-to-use cmake open the makefile and replace the instructions for building libraylib.a with your preffered method,
just make sure that the resulting libraylib.a file still ends up in Build/Cache where it's expected. 

# what it is
a simple tool i made to help me embed metadata in the alpha channel of a PNG, maybe it can do more than that
