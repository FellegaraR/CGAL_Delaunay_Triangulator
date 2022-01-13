# CGAL_Delaunay_Triangulator
Basic 2D triangulator based on CGAL library.

## requirements
	CGAL library
## how to install CGAL in ubuntu
run the following command in the terminal
	sudo apt install libcgal-dev

## how to compile
1. open the terminal in the source folder
2. then, run the following command in the terminal
	'make'
3. a file named 'dtri' should be generated if the compilation process ends correctly

## how to execute
1. open the terminal in the folder where the executable file and the input files are located
2. then, run the following command in the terminal
	./dtri <file_name>

'file_name' is a txt file in which each line correspond to a point.
The point coordinates must be separated either by spaces, commas, tabs, colons and semicolons.

The Delaunay triangulation is generated considering only the x and y coordinates.
If more field values are provided (like elevation.. temperature.. etc.) these are then appended
to each point in the final .off file containing the Delaunay triangulation
