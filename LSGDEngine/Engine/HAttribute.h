#pragma once

namespace lsgd
{
	template <typename ObjectType>
	class HAttribute
	{
	public:
		using HGetter = HFunctionObject<ObjectType()>;

		// default constructor
		HAttribute()
			: Value()
			, bIsSet(false)
			, Getter()
		{}

		// construct implicitly from an initial value
		template <typename OtherType>
		HAttribute(const OtherType& InInitialValue)
			: Value(InInitialValue)
			, bIsSet(true)
			, Getter()
		{}

		template <class SourceType, class SourceMethodType>
		HAttribute(SourceType* InUser, SourceMethodType MethodPtr)
			: Value()
			, bIsSet(true)
			, Getter([InUser, MethodPtr]() { return (InUser->*MethodPtr)(); })
		{
		}

		static HAttribute<ObjectType> Create(HGetter& InGetter)
		{
			return HAttribute(InGetter, true);
		}

		template <class SourceType, class SourceMethodType>
		static HAttribute<ObjectType> Create(SourceType* InUserObject, SourceMethodType InMethodPtr)
		{
			using MethodType = ObjectType(SourceType::*)();
			static_assert(HIsSameType<MethodType, SourceMethodType>::Value);

			return HAttribute(InUserObject, InMethodPtr);
		}

		// set the attribute value
		template <typename OtherType>
		void Set(const OtherType& InNewValue)
		{
			Getter = nullptr;
			Value = InNewValue;
			bIsSet = true;
		}

		bool IsSet() const { return bIsSet; }
		bool IsBound() const { return Getter != nullptr; }
		const HGetter& GetBinding() const { return Getter; }

		bool IdenticalTo(const HAttribute& InOther) const
		{
			const bool bIsBound = IsBound();
			if (bIsBound == InOther.IsBound())
			{
				if (bIsBound)
				{
					return Getter == InOther.Getter;
				}
				else
				{
					return IsSet() == InOther.IsSet() && Value == InOther.Value;
				}
			}

			return false;
		}

		const ObjectType& Get() const
		{
			if (Getter != nullptr)
			{
				Value = Getter();
			}
			return Value;
		}

		const ObjectType& Get(const ObjectType& DefaultValue) const
		{
			return bIsSet ? Get() : DefaultValue;
		}

		void Bind(const HGetter& InGetter)
		{
			bIsSet = true;
			Getter = InGetter;
		}

		template <class SourceType, class SourceMethodType>
		void Bind(SourceType* InUserObject, SourceMethodType InMethodPtr)
		{
			Getter = [InUserObject, InMethodPtr]() { return (InUserObject->*InMethodPtr)(); };
		}

	private:
		HAttribute(const HGetter& InGetter, bool bExplicitConstructor)
			: Value()
			, bIsSet(true)
			, Getter(InGetter)
		{}

		// we declare ourselves as a friend (templated using other type)
		//	- so we can access members as needed
		template <class OtherType> friend class HAttribute;

		mutable ObjectType Value;
		bool bIsSet;

		// our attribute's 'getter' delegate
		HGetter Getter;
	};
}