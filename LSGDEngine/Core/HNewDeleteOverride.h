#pragma once

#include <new>

extern void* operator new  (std::size_t count);
extern void* operator new[](std::size_t count);
extern void* operator new  (std::size_t count, const std::nothrow_t& tag);
extern void* operator new[](std::size_t count, const std::nothrow_t& tag);

extern void operator delete  (void* ptr);
extern void operator delete[](void* ptr);
extern void operator delete  (void* ptr, const std::nothrow_t& tag);
extern void operator delete[](void* ptr, const std::nothrow_t& tag);
extern void operator delete  (void* ptr, std::size_t sz);
extern void operator delete[](void* ptr, std::size_t sz);