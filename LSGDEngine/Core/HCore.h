#pragma once

#include "../Memory/HMemoryPCH.h"

// type overrides
#include "HTypeOverrides.h"

// by default, currently use std libraries
#define USE_STD_LIBRARIES 1

#if USE_STD_LIBRARIES

// if we use standard library, define USE_STD_ALLOCATOR
#define USE_STD_ALLOCATOR 1

// override std libraries by attaching new namespace
#include "HStdOverrides.h"

#endif

#include "HAssert.h"

#include "HGuid.h"

// C++ reflections
#include "HReflect.h"

// type data base
#include "HTypeDatabase.h"

// generate reflection data for primitive type
#include "HGeneratePrimitiveReflect.h"