#pragma once

namespace lsgd { namespace reflect {

	class HCommonTypeHelperInterface
	{
	public:
		virtual uint32 GetSize() const = 0;
		virtual void TriggerConstructor(void* InPtr) = 0;
	};

	template <typename InType>
	class HCommonTypeHelper : public HCommonTypeHelperInterface
	{
	public:
		typedef InType Type;

		virtual uint32 GetSize() const { return sizeof(Type); }
		virtual void TriggerConstructor(void* InPtr) { new (InPtr) Type(); }
	};

} }