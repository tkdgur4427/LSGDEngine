#pragma once

// include custom allocator
#include "../Memory/HAllocator.h"
#include "../Memory/HMakeMethods.h"

// include std libraries
#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <queue>
#include <list>

#include <tuple>
#include <atomic>
#include <functional>

#include <cstdio>
#include <cstdarg>

#include "HReflectionContext.h"

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
	using HArray = std::vector<ValueType, HStdAllocator<ValueType>>;

	// array
	template <class ValueType, int Size>
	using HFixedArray = std::array<ValueType, Size>;

	// hash_map
	template <class KeyType, class ValueType, class Hash = std::hash<KeyType>, class KeyEqual = std::equal_to<KeyType>>
	using HHashMap = std::unordered_map<KeyType, ValueType, Hash, KeyEqual, HStdAllocator<std::pair<const KeyType, ValueType>>>;

	// queue
	template <class Type>
	using HQueue = std::queue<Type>;

	// list
	template <class Type>
	using HList = std::list<Type>;

	// pair
	template <class KeyType, class ValueType>
	using pair = std::pair<KeyType, ValueType>;

	// HArray utility functions
	template <typename Type>
	void HAddUnique(HArray<Type>& OutArray, const Type& InValue)
	{
		// early out for unique element exists
		for (auto& Element : OutArray)
		{
			if (Element == InValue)
			{
				return;
			}
		}

		OutArray.push_back(InValue);
	}

	// unique_ptr
	template <class Type>
	using unique_ptr = std::unique_ptr<Type>;
		
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

	/*
		2019.01.07 - I tested std::function is successfully get into overrided new() operator in HNewDeleteOverride.h
	*/
	template <class FunctionType>
	using HFunctionObject = std::function<FunctionType>;

	// enable_shared_from_this
	template <class Type>
	using enable_shared_from_this = std::enable_shared_from_this<Type>;

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

	// forward
	template<class Type>
	constexpr Type&& forward(typename HRemoveReference<Type>::type& Arg)
	{	
		return (Type&&)(Arg);
	}

	// move
	template<class Type>
	constexpr typename HRemoveReference<Type>::type&& HMove(Type&& Arg) noexcept
	{	
		return (static_cast<typename HRemoveReference<Type>::type&&>(Arg));
	}

	//-------------------------------------------------------------------------
	// after this override type aliasing should add prefix 'H' denoting LSGDEngine

	// tuple
	template <class... Types>
	using HTuple = std::tuple<Types...>;

	// tuple element
	template <size_t Index, class Type>
	using HTupleElement = std::tuple_element<Index, Type>;
	
	// get
	template <size_t Index, class... Types>
	typename HTupleElement<Index, HTuple<Types...> >::type&	HGet(HTuple<Types...>& InTuple)
	{
		return std::get<Index, Types...>(InTuple);
	}

	template <size_t Index, class... Types>
	typename HTupleElement<Index, HTuple<Types...> >::type&& HGet(HTuple<Types...>&& InTuple)
	{
		return std::get<Index, Types...>(InTuple);
	}

	template <size_t Index, class... Types>
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

	template <typename Type, Type... Values>
	using HIntegerSequence = std::integer_sequence<Type, Values...>;

	template <typename Type, Type Num>
	using HMakeIntegerSequence = std::make_integer_sequence<Type, Num>;

	template <typename Type>
	HString ToString(Type InVal)
	{
		return std::to_string(InVal);
	}

	template <typename Type>
	void HSwap(Type& InValA, Type& InValB)
	{
		std::swap(InValA, InValB);
	}

	//-------------------------------------------------------------------------
	// custom implementations for dependent std library above

	template <typename Type, size_t Num, typename... Types, size_t... Indices>
	void TupleToFixedArray(HFixedArray<Type, Num>& OutFixedArray, HTuple<Types...>& InTuple, HIntegerSequence<size_t, Indices...>)
	{
		OutFixedArray = { *(static_cast<Type*>(HGet<Indices>(InTuple)))... };
	}

	template <typename Type, typename... Types>
	auto ToFixedArray(HTuple<Types...>& InTuple) -> HFixedArray<Type, sizeof...(Types)>
	{
		HFixedArray<Type, sizeof...(Types)> Result;
		TupleToFixedArray(Result, InTuple, HMakeIntegerSequence<size_t, sizeof...(Types)>());
		return Result;
	}
		
	template <typename Type>
	using HAtomic = std::atomic<Type>;
}

// macro for defining std::hash override as GetTypeHash()
//	- to override it, need to implement 'friend inline uint32 GetTypeHash(Type& InValue)'
#define USE_HASH_OVERRIDE(Type) \
	namespace std \
	{ \
		template <> \
		struct hash<Type> \
		{ \
			typedef Type argument_type; \
			typedef std::size_t result_type; \
			result_type operator()(argument_type const& InValue) const \
			{ \
				return (std::size_t)GetTypeHash(InValue); \
			} \
		}; \
	}

// reflection context declarations

// forward declaration
namespace lsgd {
	class HPackageTOC;
	class HObjectExport;
	class HObjectImport;
}

// helper methods
extern bool IsLoading(lsgd::reflect::HReflectionContext& InContext);
extern bool IsSaving(lsgd::reflect::HReflectionContext& InContext);

// operator << overloading for HReflectionContext
extern lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HString& Value);

// @todo um... how to resolve this
extern lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HPackageTOC& InTOC);
extern lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HObjectExport& InExport);
extern lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HObjectImport& InImport);

template <class KeyType, class ValueType>
lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::pair<KeyType, ValueType>& Value)
{
	KeyType KeyInstance;
	ValueType ValueInstance;
	if (InContext.IsSaving())
	{
		KeyInstance = Value.first;
		ValueInstance = Value.second;
		InContext << KeyInstance << ValueInstance;
	}
	else
	{
		InContext << KeyInstance << ValueInstance;
		Value.first = KeyInstance;
		Value.second = ValueInstance;
	}

	return InContext;
}

template <class ElementType, uint32 Count>
lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HFixedArray<ElementType, Count>& Value)
{
	uint32 Num = Value.size();
	InContext << Num;
	check(Num <= Count);

	for (auto& Element : Value)
	{
		InContext << Element;
	}

	return InContext;
}

template <class ElementType>
lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HArray<ElementType>& Value)
{
	uint32 Num = Value.size();
	InContext << Num;

	if (IsLoading(InContext))
	{
		Value.resize(Num);
	}

	for (auto& Element : Value)
	{
		InContext << Element;
	}

	return InContext;
}

template <class KeyType, class ValueType>
lsgd::reflect::HReflectionContext& operator<<(lsgd::reflect::HReflectionContext& InContext, lsgd::HHashMap<KeyType, ValueType>& Value)
{
	uint32 Num = Value.size();
	InContext << Num;

	lsgd::pair<KeyType, ValueType> PairInstance;
	if (IsLoading(InContext))
	{
		for (uint32 Index = 0; Index < Num; ++Index)
		{
			InContext << PairInstance;

			// add to hash_map
			Value.insert(PairInstance);
		}
	}
	else
	{
		for (auto& Element : Value)
		{
			PairInstance.first = Element.first;
			PairInstance.second = Element.second;

			InContext << PairInstance;
		}
	}

	return InContext;
}

// string printf utility functions
extern lsgd::HString HStringPrintf(const char* Format, ...);
extern bool HStringCompare(const char* Src0, const char* Src1);
extern void HStringCopy(char* Dest, const char* Src);
extern const char* HStrchr(const char* Str, int32 Char);
extern bool HStringNCompare(const char* Str1, const char* Str2, int32 NumCharacterToCompare);
