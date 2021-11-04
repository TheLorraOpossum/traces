# traces

![](https://raw.githubusercontent.com/TheLorraOpossum/traces/main/preview.gif)

Inspired by https://mrdoob.com/#/111/branching , I wanted to re-write it in C++.

Build requirements:
  * GLFW3 (development package, libglfw3-dev on Ubuntu)
  * GLEW 2.1 (development package, libglew-dev on Ubuntu)
  * cmake >= 3.16
  * a C++17-capable compiler

How to build:

```Bash
$ git clone https://github.com/TheLorraOpossum/traces
$ cd traces
$ git checkout main
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --build . # on Linux you can speed up with cmake --build . -- -j
$ cmake --install . --config Release --prefix <your destination> # optional
```

If you want to use this graphics in your own program, you need to have 2 files: traces_render.h and libtraces_render.a (or .lib if you are on Windows probably), which you will find in your build directory after you built or, if you have run the install step above, under the include and lib directories respectively below the --prefix you have specified.

When linking the library, you need also to link the standard C++ library (stdc++, -lstdc++ on Linux with gcc, c++, -lc++ with clang, probably valid on Linux and macOS).

Among the build artifacts is the program **traces**, which can be run standalone and an example program using libtraces_render.a (src/testMain.c), which compiles to the executable **test**.
