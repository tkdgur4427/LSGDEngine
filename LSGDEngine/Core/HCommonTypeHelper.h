#pragma once

#include "HClassType.h"

using namespace lsgd;

namespace lsgd { namespace reflect {

	class HCommonTypeHelperInterface
	{
	public:
		virtual uint32 GetSize() const = 0;
		virtual void TriggerConstructor(void* InPtr) = 0;
		virtual void Reflect() {}
	};

	template <typename InType>
	class HCommonTypeHelper : public HCommonTypeHelperInterface
	{
	public:
		using Type = InType;

		virtual uint32 GetSize() const { return sizeof(Type); }
		virtual void TriggerConstructor(void* InPtr) { new (InPtr) Type(); }
	};

	template <typename InType>
	class HCommonClassTypeHelper : public HCommonTypeHelperInterface
	{
	public:
		using Type = InType;

		virtual uint32 GetSize() const { return sizeof(Type); }
		virtual void TriggerConstructor(void* InPtr) { new (InPtr) Type(); }
		virtual void Reflect() override { Type::Reflect(); }
	};
} }