#pragma once

namespace lsgd { namespace container {

	// util container (data structure) to hold variadic 
	template <int32 StackSize = 1024>
	struct HVariadicStack
	{
		HVariadicStack()
			: CurrOffset(0)
		{
			HGenericMemory::MemZero(&Storage[0], 0, sizeof(uint8)*StackSize);
		}

		uint8 Storage[StackSize];
		int16 CurrOffset;

		// template (generic) methods
		template <typename Type>
		void PushByType(const Type& InValue);

		template <typename Type>
		Type PopByType();
		
		// pushing raw pointer to the stack (name it as reference)
		void PushReference(uint8*& InData);
		
		// general data push/pop
		void Push(uint8* InData, int16 DataSize);
		void Pop(uint8* OutData, int16 DataSize);

		uint16 GetTopOffset() const { return CurrOffset; }
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

		HGenericMemory::MemCopy(&Storage[CurrOffset], &CastedData, DataSize);
		CurrOffset += DataSize;
	}

	template <int32 StackSize>
	void HVariadicStack<StackSize>::Push(uint8* InData, int16 DataSize)
	{
		check(CurrOffset + DataSize <= StackSize);
		HGenericMemory::MemCopy(&Storage[CurrOffset], InData, DataSize);
		CurrOffset += DataSize;
	}

	template <int32 StackSize>
	void HVariadicStack<StackSize>::Pop(uint8* OutData, int16 DataSize)
	{
		check(CurrOffset - DataSize >= 0);
		CurrOffset -= DataSize;
		HGenericMemory::MemCopy(OutData, &Storage[CurrOffset], DataSize);
	}
} }