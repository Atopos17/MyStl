#pragma once
#ifndef MYSTL_ALLOC_H
#define MYSTL_ALLOC_H
#include "alloc.h"
#include "construct.h"
//二级的分配器，allocated，alloctor,是默认的分配器。
#include <string>
#include <new>
#include <cstdlib>
namespace MyStl{
    class __malloc_alloc{
        public:
            using malloc_handler=void(*)();
        private:
            static void *oom_malloc(size_t);
            static void *oom_realloc(void*,size_t);
            static malloc_handler __malloc_alloc_oom_handler;
        public: 
            static void *allocate(size_t n)
            {
                void *result=malloc(n);
                if(result==0)
                {
                    result=oom_malloc(n);
                }
                return result;
            }
            static void *deallocate(void *p,size_t /*n*/)
            {
                free(p);
            }
            static void *reallocate(void *p,size_t /*old_sz*/,size_t new_sz)
            {
                void *result=oom_realloc(p,new_sz);
                if(result==0)
                {
                    result=oom_malloc(new_sz);
                }
                return result;
            }
            //以下是仿真C++的set_new_handler()函数。换句话说，你可以通过它指定你自己的out-of-memory handler.
            static malloc_handler set_malloc_handler(malloc_handler f)
            {
                malloc_handler old=__malloc_alloc_oom_handler;
                __malloc_alloc_oom_handler=f;
                return old;
            }
    };
    //初始值为0,有待客户端设定。
    typename __malloc_alloc::malloc_handler
        __malloc_alloc::__malloc_alloc_oom_handler=nullptr;

    void * __malloc_alloc::oom_malloc(size_t n)
    {
        malloc_handler new_alloc_handler;
        void *result;
        for(;;)
        {
            new_alloc_handler=__malloc_alloc_oom_handler;
            if(!new_alloc_handler)
            {
                throw std::bad_alloc();
            }
            (*new_alloc_handler)();
            result=malloc(n);
            if(result) return result;
        }
    }
    void *__malloc_alloc::oom_realloc(void *p,size_t n)
    {
        malloc_handler new_alloc_handler;
        void *result;
        for(;;)
        {
            new_alloc_handler=__malloc_alloc_oom_handler;
            if(!new_alloc_handler)
            {
                throw std::bad_alloc();
            }
            (*new_alloc_handler)();
            result=realloc(p,n);
            if(result) return result;
        }       
    }
    //alisa
    using malloc_alloc = __malloc_alloc;

    enum __freelist_setting{
        __ALIGN=8,
        __MAX_BYTES=128,
        __NFREELISTS=__MAX_BYTES/__ALIGN
    };
    class __default_alloc{
        private:
            static  size_t ROUND_UP(size_t bytes)
            {
                return ((bytes)+static_cast<size_t>(__ALIGN-1)&~static_cast<size_t>(__ALIGN-1));
            }
        private:
            union obj{
                union obj *free_list_link;
                char client_data[1];//用于存放对象
            };
        private:
            static obj * volatile free_list[__NFREELISTS];//volatile告诉编译器这个变量可能随时都被修改不要用寄存器保存
            static size_t FREELIST_INDEX(size_t bytes)
            {
                return (bytes+static_cast<size_t>(__ALIGN)-1)/static_cast<size_t>(__ALIGN)-1;
            }
        private:
            static void *refill(size_t n);
            static char *chunk_alloc(size_t size,int &nobjs);
            //chunk allocation state
            static char *start_free;
            static char *end_free;
            static size_t heap_size;
        public:
            //n must be>0
            static void* allocate(size_t n);//详述在后边
            static void deallocate(void* p,size_t n);
            static void* reallocate(void* p,size_t old_sz,size_t  new_sz);
    };
    char *__default_alloc::start_free=nullptr;
    char *__default_alloc::end_free=nullptr;
    size_t __default_alloc::heap_size=0;
    __default_alloc::obj *volatile __default_alloc::free_list[__NFREELISTS]={
        nullptr,nullptr,nullptr,nullptr,
        nullptr,nullptr,nullptr,nullptr,
        nullptr,nullptr,nullptr,nullptr,
        nullptr,nullptr,nullptr,nullptr
        };
    void *__default_alloc::refill(size_t n)
    {
        int nobjs=20;
        char *chunk=chunk_alloc(n,nobjs);
        obj *volatile *my_free_list;
        obj *result;
        obj *current_obj,*next_obj;
        //如果只获取了一个区块就直接分配给调用者，不加入free_list
        if(nobjs==1)
        {
            return chunk;
        }
        my_free_list=free_list+FREELIST_INDEX(n);
        result=reinterpret_cast<obj *>(chunk);
        *my_free_list=next_obj=reinterpret_cast<obj *>(chunk+n);
        for(int i=1;;++i)
        {
            current_obj=next_obj;
            next_obj=reinterpret_cast<obj *>(reinterpret_cast<char *>(next_obj)+n);
            if(i==nobjs-1)
            {
                current_obj->free_list_link=nullptr;
                break;
            }
            else
            {
                current_obj->free_list_link=next_obj;
            }
        }
        return result;
    }
    //默认为size为8的整数倍
    char *__default_alloc::chunk_alloc(size_t size,int &nobjs)
    {
        char* result;
        size_t total_bytes=size*nobjs;
        size_t bytes_left=end_free-start_free;
        if(bytes_left>=total_bytes)
        {
            result=start_free;
            start_free+=total_bytes;
            return result;
        }
        else if(bytes_left>size)
        {
            nobjs=static_cast<int>(bytes_left/size);
            total_bytes=size*nobjs;
            result=start_free;
            start_free+=total_bytes;
            return result;
        }
        else
        {
            size_t bytes_to_get=2*total_bytes+ROUND_UP(heap_size>>4);
            if(bytes_left>0)
            {
               obj *volatile *my_free_list=free_list+FREELIST_INDEX(bytes_left);
               reinterpret_cast<obj *>(start_free)->free_list_link=*my_free_list;
               *my_free_list=reinterpret_cast<obj *>(start_free);
            }
            start_free=reinterpret_cast<char *>(malloc(bytes_to_get));
            if(!start_free)
            {
                //heap空间不足，malloc失败
                obj *volatile *my_free_list;
                obj *p;
                for(size_t i=size;i<=static_cast<size_t>(__MAX_BYTES);i+=static_cast<size_t>(__ALIGN))
                {
                    my_free_list=free_list+FREELIST_INDEX(i);
                    p=*my_free_list;
                    if(p)
                    {
                        *my_free_list=p->free_list_link;
                        start_free=reinterpret_cast<char*>(p);
                        end_free=start_free+i;
                        return (chunk_alloc(size,nobjs));
                    }
                }
                end_free=nullptr;
                start_free=reinterpret_cast<char*>(malloc_alloc::allocate(bytes_to_get));
            }
            heap_size=bytes_to_get;
            return chunk_alloc(size,nobjs);
            
        }
    } 
    void *__default_alloc::allocate(size_t n)
    {
        obj * volatile *my_free_list;
        obj *result;
        if(n>__MAX_BYTES)
        {
            return (malloc_alloc::allocate(n));
        }
        my_free_list=free_list+FREELIST_INDEX(n);
        result=*my_free_list;
        if(result==nullptr)
        {
            void* r=refill(ROUND_UP(n));
            return r;
        }
    }
    void __default_alloc::deallocate(void *p,size_t n)
    {
        if(n>static_cast<size_t>(__MAX_BYTES))
        {
            malloc_alloc::deallocate(p,n);
        }
        else
        {
            obj *volatile *my_free_list=free_list+FREELIST_INDEX(n);
            obj *q=reinterpret_cast<obj *>(p);
            //调整free_list,取回区块
            q->free_list_link=*my_free_list;
            *my_free_list=q;
        }
    }
    void *__default_alloc::reallocate(void *p,size_t old_sz,size_t new_sz)
    {
        void *result;
        size_t copy_sz;
        if(old_sz>static_cast<size_t>(__MAX_BYTES)&&new_sz>static_cast<size_t>(__MAX_BYTES))
            return realloc(p,new_sz);
        if(ROUND_UP(old_sz)==ROUND_UP(new_sz)) return p;
        result=allocate(new_sz);
        copy_sz=new_sz>old_sz?old_sz:new_sz;
        memcpy(result,p,copy_sz);
        deallocate(p,old_sz);
        return result;
    }

}//namespace MyStl
#endif // MYSTL_ALLOC_H
