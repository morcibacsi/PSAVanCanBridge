#!/bin/bash
doxygen ./Doxyfile
sphinx-build -M html sphinx_source ./