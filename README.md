# CGAL_Delaunay_Triangulator
Basic 2D triangulator based on CGAL library.

It supports both plain text (either txt or csv format are supported) and binary files as input and outputs TINs either in off or vtk format.

## requirements

CGAL and VTK libraries

### how to install CGAL in ubuntu
run the following command in the terminal: `sudo apt install libcgal-dev`

### how to install VTK in ubuntu 20.04
run the following command in the terminal: `sudo apt install libvtk7-dev`

Since vtk7 is a fairly old version of VTK it is common to compile VTK from source ([link](https://vtk.org/download/)). 
Build instruction can be found at the following [link](https://gitlab.kitware.com/vtk/vtk/-/blob/master/Documentation/docs/build_instructions/build.md).
In this latter case, you have to specify `VTK_DIR` before calling `cmake` command.

## how to compile

1. open the terminal in the source folder
2. then, run the following command in the terminal: `cmake .` or `cmake -DVTK_DIR=<path/to/the/install/folder/of/vtk> .`
3. if the generation is done, then type `make` for compiling the triangulator
4. a file named `dtri` should be generated if the compilation process ends correctly

## how to execute

1. open the terminal in the folder where the executable file and the input files are located
2. then, run the following command in the terminal: `./dtri <file_name> <format>`

`file_name` is a txt or binary file in which each line correspond to a point.

In a plain text file, point coordinates must be separated either by spaces, commas, tabs, colons and semicolons.

In a binary file, metadata on top of the file should be provided. These metadata specify the total number of point and the number of scalar fields each point has. Then, there is just a list of float values that it is then parsed by the reader.

`format` specifies the output format for the TIN, and it can be either `off` or `vtk`.

The Delaunay triangulation is generated considering only the x and y coordinates.
If more field values are provided (like elevation.. temperature.. etc.) these are then appended
to each point in the final `.off` or `vtk` file
