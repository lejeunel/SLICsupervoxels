# SLIC supervoxel
Python/C++ implementation of the [SLIC supervoxel](http://ivrl.epfl.ch/research/superpixels) method.

This package is a port of a MATLAB/mex implementation by Radhakrishna Achanta available [here](http://ivrl.epfl.ch/files/content/sites/ivrg/files/supplementary_material/RK_SLICsuperpixels/SLIC_mex.zip).

## Prerequisites

This software requires the
[Boost](http://www.boost.org/) library to wrap C++ code for use in Python.

```
sudo apt-get install libboost-all-dev
```

## Installing

From the project's root, install the Python package with:
```
mkdir build
cd build
cmake ..
make
make install
```

## Getting started

The provided file demo.py indicates the typical steps to generate supervoxels from Python.
