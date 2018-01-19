// =============================================================================
// == helperMEX.h
// == --------------------------------------------------------------------------
// == Functions to help in MEX interfaces
// == --------------------------------------------------------------------------
// == Copyright 2011. MIT. All Rights Reserved.
// == Written by Jason Chang 06-13-2011
// =============================================================================

#ifndef HELPERNP
#define HELPERNP
enum inputType {SCALAR, VECTOR, MATRIX, MATRIX3, NSCALAR, LIST, DICT};

#include <math.h>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

namespace bp = boost::python;
namespace bn = boost::python::numpy;

#define arr(type)    type*


template <typename T>
T* allocate_memory(unsigned int length)
{
  return (T*)malloc(length*sizeof(T));
}

template <typename T>
T* allocate_memory(unsigned int length, T value)
{
  T* ptr = (T*)malloc(length*sizeof(T));
  for (int i=length-1; i>=0; i--)
      ptr[i] = value;
  return ptr;
}

template <typename T>
void deallocate_memory(T* ptr)
{
  free(ptr);
}

template <typename T>
T* getArrayInput(const bn::ndarray & a)
{
  //std::cout << "Could cast" << std::endl ;
  return reinterpret_cast<T*>(a.get_data());
}

template <typename T>
T min(T v1, T v2)
{
   return (v1<v2 ? v1 : v2);
}

template <typename T>
T max(T v1, T v2)
{
   return (v1>v2 ? v1 : v2);
}

template <typename T>
T getInput(const bn::ndarray &pm)
{
  //if (pm==NULL)
  //  std::cerr << ("Getting input of NULL!") << std::endl;

   return *reinterpret_cast<T*>(pm.get_data());
}


inline void dbl_memset(double* ptr, double val, int N)
{
   for (int i=N-1; i>=0; i--)
      ptr[i] = val;
}

inline bool is_ndarray(const bp::object &obj){

  bp::extract<bn::ndarray> e_ndarray(obj);

  return e_ndarray.check();
}


inline bool is_list(const bp::object &obj){

  bp::extract<bn::ndarray> e_list(obj);

  return e_list.check();
}

inline bool is_dict(const bp::object &obj){

  bp::extract<bn::ndarray> e_dict(obj);

  return e_dict.check();
}


inline int get_n_elems(const bn::ndarray &a){
  //std::cout << "Get_size" << std::endl;
  int nd = a.get_nd();
  int n_elem = 1;

  std::vector<int> shape;
  for(int i=0;i<nd;i++)
          shape.push_back(a.get_shape()[i]);

  for(size_t i=0;i<shape.size();i++)
          n_elem = n_elem*shape[i];

  return n_elem;
}

inline std::vector<int> get_shape(const bn::ndarray &a){
  int nd = a.get_nd();

  std::vector<int> shape;
	for(int i=0;i<nd;i++)
		shape.push_back(a.get_shape()[i]);

  return shape;
}

// --------------------------------------------------------------------------
// -- findCoordinates
// --   converts 1D coordinates to 2D coordinates
// --
// --   parameters:
// --     - (X,Y) : the dimensions
// --     - index : the 1D coordinate
// --
// --   return parameters:
// --     - (x,y) : the 2D coordinates
// --------------------------------------------------------------------------

inline void findCoordinates(int X, int Y, int index, int &x, int &y)
{
   // index first does x's then does y's
   x = index%Y;
   y = index/Y;
}

template <typename T>
inline
bn::ndarray *npWriteStack(int width,int height, int depth, T *var)
{

  bp::tuple shape = bp::make_tuple(height,width,depth);
  bp::tuple stride = bp::make_tuple(width*sizeof(T),
                                    sizeof(T),
                                    width*height*sizeof(T));
  bn::dtype dtype = bn::dtype::get_builtin<T>();

  bp::object *result = new bp::object(bn::from_data(var,dtype,shape,stride,bp::object()));

  bn::ndarray *out_array = reinterpret_cast<bn::ndarray*>(result);

  return out_array;
}

template <typename T>
inline
bn::ndarray toNumpy(int width,int height, int depth, T *var){

  std::cout << "toNumpy" << std::endl;
  std::cout << "width: " << width << std::endl;
  std::cout << "height: " << height << std::endl;
  std::cout << "depth: " << depth << std::endl;
  int n_elems = width*height*depth;
  bp::tuple shape = bp::make_tuple(n_elems);
  bn::dtype dtype = bn::dtype::get_builtin<T>();
  T out_vec[n_elems];
  const int sz2 = width*height;

  int i;
  for(int z = 0, ii = 0; z < depth; z++)
  {
    for(int y = 0; y < height; y++)
    {
      for(int x = 0; x < width; x++)
      {
          i = z*sz2 + y*width + x;
          out_vec[ii] = var[i];
          ii++;
      }
    }
  }

  bp::tuple mat_shape = bp::make_tuple(height*width*depth);

  bn::ndarray out_mat = bn::from_data(out_vec,
                                      bn::dtype::get_builtin<T>(),
                                      mat_shape,
                                      bp::make_tuple(sizeof(T)),
                                      bp::object());

  //out_mat.reshape(bp::make_tuple(height,width,depth));

  return out_mat;
}

// --------------------------------------------------------------------------
// -- findCoordinates
// --   converts 2D coordinates to 1D coordinate
// --
// --   parameters:
// --     - (X,Y) : the dimensions
// --     - (x,y) : the 2D coordinates
// --
// --   return value:
// --     - index : the 1D coordinates
// --------------------------------------------------------------------------
inline int findIndex(int X, int Y, int x, int y)
{
   return y*X + x;
}
// --------------------------------------------------------------------------
// -- findCoordinates
// --   converts 3D coordinates to 1D coordinate
// --
// --   parameters:
// --     - (X,Y,Z) : the dimensions
// --     - (x,y,z) : the 3D coordinates
// --
// --   return value:
// --     - index : the 1D coordinates
// --------------------------------------------------------------------------
inline int findIndex(int X, int Y, int Z, int x, int y, int z)
{
   return z*X*Y + y*X + x;
}


#endif
