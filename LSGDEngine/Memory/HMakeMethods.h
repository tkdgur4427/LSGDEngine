#pragma once

#include "HGenericMemory.h"
#include "HDeleter.h"

template <typename Type, typename... Arguments>
std::unique_ptr<Type, HStdDeleter<Type>> HMakeUnique(Arguments&&... InArguments)
{
	Type* Pointer = (Type*)HGenericMemory::Allocate(sizeof(Type));
	return std::unique_ptr<Type, HStdDeleter<Type>>(new (Pointer)Type(std::forward<Arguments>(InArguments)...));
}

// note that std::make_shared doesn't supports overriding custom deleter or allocator!
template <typename Type, typename... Arguments>
std::shared_ptr<Type> HMakeShared(Arguments&&... InArguments)
{
	Type* Pointer = (Type*)HGenericMemory::Allocate(sizeof(Type));
	return std::shared_ptr<Type>(new (Pointer)Type(std::forward<Arguments>(InArguments)...), HStdDeleter<Type>());
}
