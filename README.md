# trace-lite

A light raytracer that raytraces light.

Built as a learning project for practicing modern C++, and basic rendering
techniques.

## Usage

trace-lite is capable of rendering Assimp compatible scene files. Run the
following command to perform a basic render:

```
$ trace-lite /path/to/scene-file
```

For additional options, see:

```
$ trace-lite --help
```

## Building

### Dependencies

Download the source for [Assimp 4.1.0](http://www.assimp.org/index.php/downloads),
copying the directory assimp-4.1.0 into the deps directory.

The process of installing dependencies will be streamlined in the future.

### CMake

Move to your build directory, and run the following command to generate build
files, specifying a generator if necessary:

```
$ cmake /path/to/project/root
```

For more information about using CMake, see the
[CMake Documentation](https://cmake.org/documentation/).

Lastly, use the appropriate build command for your generator. For Makefiles,
this is:

```
$ make
```
