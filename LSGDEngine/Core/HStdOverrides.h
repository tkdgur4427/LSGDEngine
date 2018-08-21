#pragma once

// include custom allocator
#include "../Memory/HAllocator.h"

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
	using array = std::vector<ValueType, HStdAllocator<ValueType>>;

	// hash_map
	template <class KeyType, class ValueType, class Hash = std::hash<KeyType>, class KeyEqual = std::equal_to<KeyType>>
	using hash_map = std::unordered_map<KeyType, ValueType, Hash, KeyEqual, HStdAllocator<std::pair<const KeyType, ValueType>>>;

	// pair
	template <class KeyType, class ValueType>
	using pair = std::pair<KeyType, ValueType>;
}