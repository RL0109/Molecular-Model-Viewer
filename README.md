# Molecular-Model-Viewer

A molecular model viewer written in C++ using the raylib libraries. 
Currently, can take a leucine molecule and render it as spheres and 
lines. Can render a full protein but requires tweaking the parsing 
logic. Leucine file is provided but can be found here: 
https://www.rcsb.org/ligand/LEU.

## Features
- PDB file parsing: applies a custom parser to take atomic coordinates from PDB/CIF files
- 3D rendering pipeline: Using sphere and cylinder primitives, creates renders atoms and bonds using raylib
- Interactive Controls: Using affine transforms allows users to rotate, translate, and scale the rendered atom.

## Interactive Controls
- Press R to click the screen and drag the mouse to rotate the molecule in any direction.
- Press T to allow mouse dragging to move the molecule in any direction.
- Use the scroll wheel to zoom in or out on the molecule.

## Required raylib files
The following raylib files need to be in the repo for the application to compile 
correctly: 

- raylib.h
- raymath.h
- rlgl.h  

These files can be found on the raylib website: https://www.raylib.com/

When compiling via GCC or MinGW you can do the following: g++ main.cpp -L "location\of\raylib\binaries" -lraylib -lgdi32 -lwinmm -o app.exe.
