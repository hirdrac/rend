<img src="https://github.com/hirdrac/rend/blob/main/images/title.png" />

# rend
A simple 3d ray-tracing renderer.

## Features
* basic geometric object support: sphere, cube, cylinder, torus, and others
* constructive solid geometry operations: union, intersection, difference
* automatic bounding box hierarchy creation for faster rendering
* multi-threaded rendering support
* lisp-like scene description language
* optional interactive shell for performing rendering operations

## Platform Support
* Linux
* Cygwin/MinGW/Msys2 on Windows

##
To build, just run 'make' in the main directory.

To render the title image:
```
./rend scenes/title.sdl title.bmp -j
convert title.bmp title.png
```
(only BMP output is supported currently so ImageMagick's convert is used to produce the final PNG file)
