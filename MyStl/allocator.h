#pragma once
#ifndef MYSTL_AllOCATOR_H
#define MYSTL_ALLOCATOR_H
#include "alloc.h"
#include "construct.h"
namespace MyStl{
    template <class T,class Alloc=__default_alloc>
    class simpleAlloc{
    public:
        using value=T;
        using pointer=T*;
        using const_pointer=const T*;
        using reference=T&;
        using const_reference=const T&;
        using size_type=size_t;
        using difference_type=ptrdiff_t;
    public:
        static T *allocate();
        static T *allocate(size_type n);
        static void deallocate(T *ptr);
        static void deallocate(T *ptr,size_type n);
        
        static void construct(T *ptr);
        static void construct(T *ptr,const T &value);
        static void destory(T *ptr);
        static void destory(T* first,T* last);
    };
    template <class T,class Alloc>
    T *simpleAlloc<T,Alloc>::allocate(){
        return static_cast<T*>(Alloc::allocate(sizeof(T)));
    }
    template <class T,class Alloc>
    T *simpleAlloc<T,Alloc>::allocate(size_type n){
        return static_cast<T*>(Alloc::allocate(n*sizeof(T)));
    }
    template <class T,class Alloc>
    void simpleAlloc<T,Alloc>::deallocate(T *prt)
    {
        Alloc::deallocate(reinterpret_cast<void*>(ptr),sizeof(T));
    }
    template <class T,class Alloc>
    void simpleAlloc<T,Alloc>::deallocate(T *ptr,size_type n)
    {
        Alloc::deallocate(reinterpret_cast<void*>(ptr),n*sizeof(T));
    }
    template <class T,class Alloc>
    void simpleAlloc<T,Alloc>::construct(T *ptr)
    {
        new (ptr) T();
    }
    template <class T,class Alloc>
    void simpleAlloc<T,Alloc>::construct(T *ptr,const T &value)
    {
        new (ptr) T(value);
    }
    template <class T,class Alloc>
    void simpleAlloc<T,Alloc>::destory(T *ptr)
    {
        ptr->~T();
    }
    template <class T,class Alloc>
    void simpleAlloc<T,Alloc>::destory(T *first,T *last)
    {
        for(;first!=last;first++)
        {
            first->~T();
        }
    }
}
#endif 
/*
Summary:
1.对alloc.h中的两种分配器进行进一步封装，形成统一的接口
2.弥补了SGI STL中的分配器没有实现的destory和construct等必要的函数，使得simplateAllocator符合标准STL分配器
3.对外提供关键的函数
    allocate
    deallocate
    construct
    destory
*/