# Molecular-Model-Viewer

A molecular model viewer written in C++ using the raylib libraries. 
Currently, can take a leucine molecule and render it as spheres and 
lines. Can render a full protein but requires tweaking the parsing 
logic. Leucine file is provided but can be found here: 
https://www.rcsb.org/ligand/LEU.

## Features
- PDB file parsing: applies a custom parser to take atomic coordinates from pdb file
- 3D rendering pipeline: Using sphere and cylinder primitives, creates renders atoms and bonds using raylib
- Interactive Controls: Using affine transforms allows users to rotate, translate, and scale the rendered atom.

## Required raylib files
The following raylib files need to be in the repo for the application to compile 
correctly: 

- raylib.h
- raymath.h
- rlgl.h
These files can be found here: https://www.raylib.com/

When compiling make sure to have these commands 
compiler main.cpp -L "location\of\raylib\binaries" -lraylib -lgdi32 -lwinmm -o app.exe.
