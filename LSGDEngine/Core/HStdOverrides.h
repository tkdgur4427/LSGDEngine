#pragma once

// include custom allocator
#include "../Memory/HAllocator.h"
#include "../Memory/HDeleter.h"
#include "../Memory/HMakeMethods.h"

// include std libraries
#include <string>
#include <vector>
#include <unordered_map>

// override std libraries with lsgd:: by type aliasing
namespace lsgd
{
	// basic_string
	template <class CharType, class Traits = std::char_traits<CharType>>
	using basic_string = std::basic_string<CharType, Traits, HStdAllocator<char>>;

	// string
	using string = std::basic_string<char>;

	// wstring
	using wstring = std::basic_string<wchar_t>;

	// array
	template <class ValueType>
	using vector = std::vector<ValueType, HStdAllocator<ValueType>>;

	// hash_map
	template <class KeyType, class ValueType, class Hash = std::hash<KeyType>, class KeyEqual = std::equal_to<KeyType>>
	using hash_map = std::unordered_map<KeyType, ValueType, Hash, KeyEqual, HStdAllocator<std::pair<const KeyType, ValueType>>>;

	// pair
	template <class KeyType, class ValueType>
	using pair = std::pair<KeyType, ValueType>;

	// unique_ptr
	template <class Type, class Deleter = HStdDeleter<Type>>
	using unique_ptr = std::unique_ptr<Type, Deleter>;
		
	// make_unique
	template <typename Type, typename... Arguments>
	unique_ptr<Type> make_unique(Arguments&&... InArguments)
	{
		/*
			study note
				- in this case : HMakeUnique<Type>(InArguments);
				- error C3520 : parameter pack must be expanded in this context 
		*/
		return HMakeUnique<Type>(InArguments...);
	}

	// shared_ptr
	template <class Type>
	using shared_ptr = std::shared_ptr<Type>;

	// make_shared
	template <typename Type, typename... Arguments>
	shared_ptr<Type> make_shared(Arguments&&... InArguments)
	{
		return HMakeShared<Type>(InArguments...);
	}

	// weak_ptr
	template <class Type>
	using weak_ptr = std::weak_ptr<Type>;

	// remove_reference
	template<class Type>
	using remove_reference = typename std::remove_reference_t<Type>::type;

	// forward
	template<class Type>
	constexpr Type&& forward(remove_reference<Type>& Arg)
	{	
		return (static_cast<Type&&>(Arg));
	}

	// move
	template<class Type>
	constexpr remove_reference<Type>&& move(Type&& Arg)
	{	
		return (static_cast<remove_reference<Type>&&>(Arg));
	}
}