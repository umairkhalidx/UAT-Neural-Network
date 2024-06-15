#!/bin/bash

# Compile the C++ files
g++ Output.cpp -o Output
g++ Layer.cpp -o Layer
g++ Input.cpp -o Input

# Run the final executable
./Input

