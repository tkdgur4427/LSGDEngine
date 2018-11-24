#pragma once

#include <new>

extern void* operator new  (std::size_t count);
extern void* operator new[](std::size_t count);
extern void* operator new  (std::size_t count, std::nothrow_t const& tag);
extern void* operator new[](std::size_t count, std::nothrow_t const& tag);

extern void operator delete  (void* ptr);
extern void operator delete[](void* ptr);
extern void operator delete  (void* ptr, std::nothrow_t const& tag);
extern void operator delete[](void* ptr, std::nothrow_t const& tag);
extern void operator delete  (void* ptr, std::size_t sz);
extern void operator delete[](void* ptr, std::size_t sz);