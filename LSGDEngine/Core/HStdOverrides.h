#pragma once

// include custom allocator
#include "../Memory/HAllocator.h"
#include "../Memory/HDeleter.h"
#include "../Memory/HMakeMethods.h"

// include std libraries
#include <string>
#include <array>
#include <vector>
#include <unordered_map>

#include <tuple>
#include <utility>

// override std libraries with lsgd:: by type aliasing
namespace lsgd
{
	// basic_string
	template <class CharType, class Traits = std::char_traits<CharType>>
	using basic_string = std::basic_string<CharType, Traits, HStdAllocator<char>>;

	// string
	using HString = std::basic_string<char>;

	// wstring
	using wstring = std::basic_string<wchar_t>;

	// HArray
	template <class ValueType>
	using HArray = std::vector<ValueType>;

	// array
	template <class ValueType, int Size>
	using HFixedArray = std::array<ValueType, Size>;

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

	template <typename BaseType, typename Type, typename... Arguments>
	unique_ptr<BaseType> make_unique(Arguments&&... InArguments)
	{
		return HMakeUnique<BaseType, Type>(InArguments...);
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

	template <typename BaseType, typename Type, typename... Arguments>
	shared_ptr<BaseType> make_shared(Arguments&&... InArguments)
	{
		return HMakeShared<BaseType, Type>(InArguments...);
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

	//-------------------------------------------------------------------------
	// after this override type aliasing should add prefix 'H' denoting LSGDEngine

	// tuple
	template <class... Types>
	using HTuple = std::tuple<Types...>;

	// tuple element
	template <uint32 Index, class Type>
	using HTupleElement = std::tuple_element<Index, Type>;
	
	// get
	template <uint32 Index, class... Types>
	typename HTupleElement<Index, HTuple<Types...> >::type&	HGet(HTuple<Types...>& InTuple)
	{
		return std::get<Index, Types...>(InTuple);
	}

	template <uint32 Index, class... Types>
	typename HTupleElement<Index, HTuple<Types...> >::type&& HGet(HTuple<Types...>&& InTuple)
	{
		return std::get<Index, Types...>(InTuple);
	}

	template <uint32 Index, class... Types>
	typename HTupleElement<Index, HTuple<Types...> >::type const& HGet(const HTuple<Types...>& InTuple)
	{
		return std::get<Index, Types...>(InTuple);
	}

	// make tuple
	template <class... Types>
	HTuple<Types...> HMakeTuple(Types&&... InArguments)
	{
		return std::make_tuple(std::forward<Types>(InArguments)...);
	}

	template <class Type, Type... Values>
	using HIntegerSequencer = std::integer_sequence<Type, Values...>;

	template <class Type, Type Num>
	using HMakeIndexSequence = std::make_index_sequence<Num>;

	//-------------------------------------------------------------------------
	// custom implementations for dependent std library above

	template <typename Type, uint32 Num, typename... Types, uint32... Indices>
	void TupleToFixedArray(HFixedArray<Type, Num>& OutFixedArray, HTuple<Types...>& InTuple, HIntegerSequencer<Indices...>)
	{
		OutFixedArray = { static_cast<Type>(HGet<Indices>(InTuple))... };
	}

	template <typename Type, typename... Types>
	auto ToFixedArray(HTuple<Types...>& InTuple) -> HFixedArray<Type, sizeof...(Types)>
	{
		HFixedArray<Type, sizeof...(Types)> Result;
		TupleToFixedArray(Result, InTuple, HMakeIndexSequence<sizeof...(Types)>());
		return Result;
	}
}