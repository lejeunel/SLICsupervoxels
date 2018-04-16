#ifndef PYSVX_H
#define PYSVX_H

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include "slicsupervoxel.h"

namespace bp = boost::python;
namespace bn = boost::python::numpy;
using namespace boost;

struct svx {
  static shared_ptr<svx> create () { return shared_ptr<svx>(new svx); }
  std::string hello() { return "Just nod if you can hear me!"; }

  bp::tuple run(bn::ndarray stack,
                int numReqdSupervoxels,
                double compactness);
};

#endif
