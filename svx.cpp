#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/shared_ptr.hpp>
#include "pysvx.h"

namespace bp = boost::python;
namespace bn = boost::python::numpy;
using namespace boost;

//Will create libsvx.so
BOOST_PYTHON_MODULE(libsvx)
{
  using namespace boost::python;
  Py_Initialize();
  bn::initialize();

  //from python:
  //from libsvx import *
  //svx.create()
  class_<svx, boost::shared_ptr<svx> >("svx",init<>())
    .def("create",&svx::create )
    .staticmethod("create")
    .def("hello",&svx::hello)
    .def("run",&svx::run)
    ;
}
