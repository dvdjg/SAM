#ifndef GET_TYPET_H
#define GET_TYPET_H

#include "typet.hpp"


struct generic_type     { template<typename T> generic_type (const T & o) {} generic_type() {}};
struct funda_type       { template<typename T> funda_type   (const T & o) {} funda_type()   {}};
struct ptr_type         { template<typename T> ptr_type     (const T & o) {} ptr_type()     {}};
struct ref_type         { template<typename T> ref_type     (const T & o) {} ref_type()     {}};
struct array_type       { template<typename T> array_type   (const T & o) {} array_type()   {}};
struct func_type        { template<typename T> func_type    (const T & o) {} func_type()    {}};
struct ptrmem_type      { template<typename T> ptrmem_type  (const T & o) {} ptrmem_type()  {}};
struct enum_type        { template<typename T> enum_type    (const T & o) {} enum_type()    {}};
struct class_type       { template<typename T> class_type   (const T & o) {} class_type()   {}};

template <typename T>
class OrTypeT {
  public:

    enum Type
         { FundaT  = 1 << 0,
           PtrT    = 1 << 1,
           RefT    = 1 << 2,
           ArrayT  = 1 << 3,
           FuncT   = 1 << 4,
           PtrMemT = 1 << 5,
           EnumT   = 1 << 6,
           ClassT  = 1 << 7,
           IsFundaT  = IsFundaT<T>::Yes         ? FundaT  : 0,
           IsPtrT    = CompoundT<T>::IsPtrT     ? PtrT    : 0,
           IsRefT    = CompoundT<T>::IsRefT     ? RefT    : 0,
           IsArrayT  = CompoundT<T>::IsArrayT   ? ArrayT  : 0,
           IsFuncT   = CompoundT<T>::IsFuncT    ? FuncT   : 0,
           IsPtrMemT = CompoundT<T>::IsPtrMemT  ? PtrMemT : 0,
           IsEnumT   = IsEnumT<T>::Yes          ? EnumT   : 0,
           IsClassT  = IsClassT<T>::Yes         ? ClassT  : 0,
           Combined  = IsFundaT|IsPtrT|IsRefT|IsArrayT|IsFuncT|IsPtrMemT|IsEnumT|IsClassT
         };
};


// primary template: yield second or third argument depending on first argument
template<typename T, int I = (int) OrTypeT<T>::Combined > // typename OrTypeT<T>::Type typ = OrTypeT<T>::Combined
class GetTypeT {
public:
  typedef generic_type ResultT;
};

template<typename T>
class GetTypeT<T, (int) OrTypeT<int>::FundaT> {
  public:
    typedef funda_type ResultT;
};

template<typename T>
class GetTypeT<T, (int) OrTypeT<int>::PtrT> {
  public:
    typedef ptr_type ResultT;
};

template<typename T>
class GetTypeT<T, (int) OrTypeT<int>::RefT> {
  public:
    typedef ref_type ResultT;
};

template<typename T>
class GetTypeT<T, (int) OrTypeT<int>::ArrayT> {
  public:
    typedef array_type ResultT;
};

template<typename T>
class GetTypeT<T, (int) OrTypeT<int>::FuncT> {
  public:
    typedef func_type ResultT;
};

template<typename T>
class GetTypeT<T, (int) OrTypeT<int>::PtrMemT> {
  public:
    typedef ptrmem_type ResultT;
};

template<typename T>
class GetTypeT<T, (int) OrTypeT<int>::EnumT> {
  public:
    typedef enum_type ResultT;
};

template<typename T>
class GetTypeT<T, (int) OrTypeT<int>::ClassT> {
  public:
    typedef class_type ResultT;
};



#endif // GET_TYPET_H
