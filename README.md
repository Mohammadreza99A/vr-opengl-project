# Virtual Reality course OpenGL project

> This project has been only ran and tested on Linux systems (Ubuntu and Arch Linux).

## How to compile and run

Project uses CMake as it's build system. 

In order to compile, run these commands in the root of the project:
```bash
mkdir build
cd build
cmake .. # Generates Makefile
make 
# or make -j ${number} where number is an integer based on number of processes to be used by make command
```

After compiling and building the project, the executable can be found in `build` directory. Run it by typing `./build/vr_opengl_project` from the root of the project. 

## Dependencies

This project requires these dependencies in order to be compiled and ran:
| Library | Version |
| :-----: | :-----: |
| glfw    | 3.3.8   |
| devil   | 1.8.0   |
| glm     | 0.9.9.8 |