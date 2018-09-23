#pragma once

namespace lsgd { namespace container {

	// util container (data structure) to hold variadic 
	template <int32 StackSize = 1024>
	struct HVariadicStack
	{
		HVariadicStack()
			: CurrOffset(0)
			, Count(0)
		{
			HGenericMemory::MemZero(&Storage[0], 0, sizeof(uint8)*StackSize);
		}

		uint8 Storage[StackSize];
		int16 CurrOffset;
		int16 Count;

		// helper methods
		template <typename... Types>
		void PushByTypeMultiple(Types&&... InValues);

		// template (generic) methods
		template <typename Type>
		void PushByType(const Type& InValue);

		template <typename Type>
		Type PopByType();
		
		// pushing raw pointer to the stack (name it as reference)
		void PushReference(uint8*& InData);
		void PopReference(uint8*& OutData);
		
		// general data push/pop
		void Push(uint8* InData, int16 DataSize);
		void Pop(uint8* OutData, int16 DataSize);

		uint16 GetTopOffset() const { return CurrOffset; }
		const uint8* GetData() const { return &Storage[0]; }

	protected:
		template <typename Type, typename... Types>
		void PushByTypeRecursive(Type& InValue, Types&&... InValues);

		template <typename Type>
		void PushByTypeRecursive(Type& InValue);
	};

	template <int32 StackSize>
	template <typename Type>
	void HVariadicStack<StackSize>::PushByType(const Type& InValue)
	{
		Push((uint8*)&InValue, sizeof(Type));
	}

	template <int32 StackSize>
	template <typename Type>
	Type HVariadicStack<StackSize>::PopByType()
	{
		Type Result;
		Pop((uint8*)&Result, sizeof(Type));
		return Result;
	}

	template <int32 StackSize>
	void HVariadicStack<StackSize>::PushReference(uint8*& InData)
	{
		int16 DataSize = sizeof(uint8*);
		check(CurrOffset + DataSize <= StackSize);

		//@todo - need to find fancy way (because its size can be different depending on 32/64 bits)
		uintptr_t CastedData = (uintptr_t)InData;
		Push((uint8*)&CastedData, DataSize);
	}

	template <int32 StackSize>
	void HVariadicStack<StackSize>::PopReference(uint8*& OutData)
	{
		//@todo - need to find fancy way (because its size can be different depending on 32/64 bits)
		uintptr_t CastedData;
		Pop((uint8*)&CastedData, sizeof(uintptr_t));
		OutData = (uint8*)CastedData;
	}

	template <int32 StackSize>
	void HVariadicStack<StackSize>::Push(uint8* InData, int16 DataSize)
	{
		check(CurrOffset + DataSize <= StackSize);
		HGenericMemory::MemCopy(&Storage[CurrOffset], InData, DataSize);
		CurrOffset += DataSize;
		Count++;
	}

	template <int32 StackSize>
	void HVariadicStack<StackSize>::Pop(uint8* OutData, int16 DataSize)
	{
		check(CurrOffset - DataSize >= 0);
		CurrOffset -= DataSize;
		Count--;
		HGenericMemory::MemCopy(OutData, &Storage[CurrOffset], DataSize);
	}

	template <int32 StackSize>
	template <typename... Types>
	void HVariadicStack<StackSize>::PushByTypeMultiple(Types&&... InValues)
	{
		// call recursive push method
		PushByTypeRecursive(InValues...);
	}

	template <int32 StackSize>
	template <typename Type, typename... Types>
	void HVariadicStack<StackSize>::PushByTypeRecursive(Type& InValue, Types&&... InValues)
	{
		// call base function
		PushByTypeRecursive(InValue);

		// call recursive function
		PushByTypeRecursive(InValues...);
	}

	template <int32 StackSize>
	template <typename Type>
	void HVariadicStack<StackSize>::PushByTypeRecursive(Type& InValue)
	{
		//@todo need to support reference and value push separately
		PushByType(InValue);
	}
} }