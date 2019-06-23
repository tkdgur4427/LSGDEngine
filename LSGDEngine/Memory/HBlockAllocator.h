#pragma once

#include "HBaseAllocator.h"

/*
	Implementation NOTE...
		- we can change bitmap version
*/

#define DEBUG_HBlockAllocator 1

namespace allocators
{
	// @todo - if we want to provide flexibility for block allocator, implement this block allocator policy
	template <int32 InBlockSize = 16, int32 InPageSize = 4 * 1024>
	class HBlockAllocatorPolicy
	{
	public:
		enum
		{
			BlockSize = InBlockSize,
			PageSize = InPageSize,
		};
	};

	template <typename Policy = HBlockAllocatorPolicy<>>
	class HBlockAllocator : public HBaseAllocator
	{
	public:
		enum 
		{
			BlockSize = Policy::BlockSize,
			PageSize = Policy::PageSize,
		};

		static const int64 SentinelExpectedValue = 0xdeadbeefdeadbeef;

		struct HFreeMemoryBlockHeader
		{
			HFreeMemoryBlockHeader()
				: Next(nullptr)
				, Sentinal(SentinelExpectedValue)
			{}
			
			int64 Sentinal;
			HFreeMemoryBlockHeader* Next;
		};

		struct HMemoryPageInfo
		{
			HMemoryPageInfo()
				: NextPage(nullptr)
				, FreeMemoryBlockHead(nullptr)
			{}

			// currently total 16 bytes allowed

			// next page address			- 8 bytes
			void* NextPage;

			// @todo - we can change this into bitmap version!
			// total HMemoryBlock Count		- 8 bytes
			HFreeMemoryBlockHeader* FreeMemoryBlockHead;
		};

		struct HMemoryBlock
		{
			union
			{
				HFreeMemoryBlockHeader FreeMemoryBlockHeader;
				uint8 Data[BlockSize];
			};
		};

		struct HMemoryPage
		{
			enum 
			{
				MemoryPageInfoSize = sizeof(HMemoryPageInfo),
				MemoryBlockCount = PageSize / BlockSize,
				UnusedMemorySize = (PageSize - (MemoryBlockCount * BlockSize)),
			};

			HMemoryPage() 
			{
				InitializeMemoryPageInfo();
			}

			~HMemoryPage() {}

			int32 GetTotalAllocBlockNum() const
			{
				// memory block count requested by HMemoryPageInfo
				int32 MemoryBlockCountForPageInfo = (MemoryPageInfoSize + BlockSize - 1) / BlockSize;

				// total counts to allocate
				int32 AvailableCounts = MemoryBlockCount - MemoryBlockCountForPageInfo;

				return AvailableCounts;
			}

			void Validate()
			{
#if DEBUG_HBlockAllocator
				int32 Count = 0;
				HFreeMemoryBlockHeader* CurrHeader = GetMemoryPageInfo()->FreeMemoryBlockHead;
				while (CurrHeader)
				{
					mcheck((int64)CurrHeader != 0xFFFFFFFFFFFFFFFF);
					mcheck(IsInRange((uint8*)CurrHeader));

					// check sentinel value for under/over run
					mcheck(CurrHeader->Sentinal == SentinelExpectedValue);

					// increase the memory count
					Count++;
					
					CurrHeader = CurrHeader->Next;
				}
#endif
			}

			// construct free memory blocks
			void ConstructFreeLists(HMemoryPageInfo* InPageInfo)
			{
				int32 AvailableCounts = GetTotalAllocBlockNum();

				// call constructor for free memory block header
				for (int32 Index = 0; Index < AvailableCounts; ++Index)
				{
					new (&(MemoryBlocks[Index].FreeMemoryBlockHeader)) HFreeMemoryBlockHeader();
				}

				// set the block0's header as head
				InPageInfo->FreeMemoryBlockHead = &MemoryBlocks[0].FreeMemoryBlockHeader;

				// construct the free lists
				HFreeMemoryBlockHeader* CurrHeader = InPageInfo->FreeMemoryBlockHead;
				for (int32 Index = 1; Index < AvailableCounts; ++Index)
				{
					// set the next free block
					CurrHeader->Next = &MemoryBlocks[Index].FreeMemoryBlockHeader;
					// update the current header
					CurrHeader = &MemoryBlocks[Index].FreeMemoryBlockHeader;
				}

				Validate();
			}

			// initialize memory page information using one HMemoryBlock
			void InitializeMemoryPageInfo()
			{
				HMemoryPageInfo* PageInfo = GetMemoryPageInfo();
				
				// call constructor
				new (PageInfo) HMemoryPageInfo();

				ConstructFreeLists(PageInfo);
			}

			HMemoryPageInfo* GetMemoryPageInfo()
			{
				return (HMemoryPageInfo*)&MemoryBlocks[MemoryBlockCount - 1];
			}

			const HMemoryPageInfo* GetMemoryPageInfo() const
			{
				return (const HMemoryPageInfo*)&MemoryBlocks[MemoryBlockCount - 1];
			}

			bool HasFreeBlock() const
			{
				const HMemoryPageInfo* MemoryPageInfo = GetMemoryPageInfo();
				return MemoryPageInfo->FreeMemoryBlockHead != nullptr;
			}

			HMemoryBlock* AllocateBlock()
			{
				HFreeMemoryBlockHeader*& NewBlockHeader = GetMemoryPageInfo()->FreeMemoryBlockHead;
				HMemoryBlock* NewBlock = (HMemoryBlock*)NewBlockHeader;

				// unlink the memory block
				NewBlockHeader = NewBlockHeader->Next;

				Validate();

				return NewBlock;
			}

			void DeallocateBlock(void* InPointer)
			{
				// validation checking
				mcheck(IsInRange((uint8*)InPointer));
				
				// link to free block
				HFreeMemoryBlockHeader* NewFreeBlock = (HFreeMemoryBlockHeader*)InPointer;
				HFreeMemoryBlockHeader*& FreeBlockHead = GetMemoryPageInfo()->FreeMemoryBlockHead;
				
				// initialize new free block
				NewFreeBlock->Next = FreeBlockHead;
				NewFreeBlock->Sentinal = SentinelExpectedValue;

				FreeBlockHead = NewFreeBlock;

				Validate();
			}

			// range check for validation
			bool IsInRange(uint8* InAddress) const
			{
				uint8* StartAddress = (uint8*)&MemoryBlocks[0];
				uint8* EndAddress = (uint8*)&MemoryBlocks[GetTotalAllocBlockNum()];

				return (InAddress >= StartAddress) && (InAddress < EndAddress);
			}

			// aliasing the memory page to proper type for easier interface
			union 
			{
				uint8 Data[PageSize];
				HMemoryBlock MemoryBlocks[MemoryBlockCount];
			};
		};

		// default page is contained in block allocator
		HMemoryPage MemoryPage;

		HBlockAllocator()
		{
			//static_assert(sizeof(HMemoryPageInfo) <= BlockSize);
			//static_assert(sizeof(HFreeMemoryBlockHeader) <= BlockSize);
			//static_assert(sizeof(HMemoryBlock) == BlockSize);
			//static_assert(sizeof(HMemoryPage) == PageSize);
		}

		~HBlockAllocator()
		{
			ReleaseAllMemoryPages();
		}

	protected:
		HMemoryPage* AllocateMemoryPage()
		{
			// allocate memory page
			HMemoryPage* NewPage = (HMemoryPage*)HGenericMemory::Allocate(PageSize);
			// call constructor
			new (NewPage) HMemoryPage();

			return NewPage;
		}

		void DeallocateMemoryPage(HMemoryPage* InMemoryPage)
		{
			// call destructor
			InMemoryPage->~HMemoryPage();
			// release the memory
			HGenericMemory::Deallocate(InMemoryPage);
		}

		void ReleaseAllMemoryPages()
		{
			// release the all memory page
			HMemoryPage* CurrPage = (HMemoryPage*)MemoryPage.GetMemoryPageInfo()->NextPage;
			while (CurrPage != nullptr)
			{
				HMemoryPage* PageToDelete = CurrPage;
				CurrPage = (HMemoryPage*)PageToDelete->GetMemoryPageInfo()->NextPage;

				// delete the page
				DeallocateMemoryPage(PageToDelete);
			}

			mcheck(MemoryPage.GetMemoryPageInfo()->NextPage == nullptr);
		}

		HMemoryPage* FindMemoryPageToAllocate()
		{
			if (MemoryPage.HasFreeBlock())
			{
				return &MemoryPage;
			}

			HMemoryPage* CurrPage = (HMemoryPage*)MemoryPage.GetMemoryPageInfo()->NextPage;
			while (CurrPage != nullptr)
			{
				if (CurrPage->HasFreeBlock())
				{
					return CurrPage;
				}

				CurrPage = (HMemoryPage*)CurrPage->GetMemoryPageInfo()->NextPage;
			}

			// need to allocate new page
			// so, get the last page
			CurrPage = &MemoryPage;
			while (CurrPage != nullptr)
			{
				if (CurrPage->GetMemoryPageInfo()->NextPage == nullptr)
				{
					break;
				}
				CurrPage = (HMemoryPage*)CurrPage->GetMemoryPageInfo()->NextPage;

				// note that it has no free blocks!
				mcheck(CurrPage->HasFreeBlock() == false);
			}

			HMemoryPage* NewPage = AllocateMemoryPage();
			HMemoryPageInfo* CurrPageInfo = CurrPage->GetMemoryPageInfo();
			CurrPageInfo->NextPage = NewPage;

			return NewPage;
		}

		HMemoryPage* FindMemoryPageToDeallocate(void* InPointer)
		{
			uint8* MemoryAddress = (unsigned char*)InPointer;

			if (MemoryPage.IsInRange(MemoryAddress))
			{
				return &MemoryPage;
			}

			HMemoryPageInfo* CurrPageInfo = MemoryPage.GetMemoryPageInfo();
			HMemoryPage* CurrPage = (HMemoryPage*)CurrPageInfo->NextPage;
			while (CurrPage != nullptr)
			{
				if (CurrPage->IsInRange(MemoryAddress))
				{
					return CurrPage;
				}

				CurrPage = (HMemoryPage*)CurrPage->GetMemoryPageInfo()->NextPage;
			}

			return nullptr;
		}

		void* AllocateMemoryBlock()
		{
			HMemoryPage* AllocatablePage = FindMemoryPageToAllocate();
			mcheck(AllocatablePage->HasFreeBlock());
			return (void*)AllocatablePage->AllocateBlock();
		}

		void DeallocateMemoryBlock(void* InPointer)
		{
			HMemoryPage* PageToDeallocate = FindMemoryPageToDeallocate(InPointer);
			mcheck(PageToDeallocate != nullptr);

			PageToDeallocate->DeallocateBlock(InPointer);
		}

	public:
		virtual void* Allocate(size_t InSize) override
		{
			(void*)InSize;
			return AllocateMemoryBlock();
		}

		virtual void Deallocate(void* InPointer) override
		{
			DeallocateMemoryBlock(InPointer);
		}
	};
}