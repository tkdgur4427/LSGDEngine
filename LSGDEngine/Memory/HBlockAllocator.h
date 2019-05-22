#pragma once

/*
	Implementation NOTE...
		- we can change bitmap version
*/

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
	class HBlockAllocator
	{
	public:
		enum 
		{
			BlockSize = Policy::BlockSize,
			PageSize = Policy::PageSize,
		};

		struct HFreeMemoryBlockHeader
		{
			HFreeMemoryBlockHeader()
				: Next(nullptr)
			{}

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
				unsigned char Data[BlockSize];
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
				int32 MemoryBlockCountForPageInfo = (MemoryPageInfoSize + BlockSize) / BlockSize;

				// total counts to allocate
				int32 AvailableCounts = MemoryBlockCount - MemoryBlockCountForPageInfo;

				return AvailableCounts;
			}

			// construct free memory blocks
			void ConstructFreeLists(HMemoryPageInfo* InPageInfo)
			{
				int32 AvailableCounts = GetTotalAllocBlockNum();

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
			}

			// initialize memory page information using one HMemoryBlock
			void InitializeMemoryPageInfo()
			{
				HMemoryPageInfo* PageInfo = GetMemoryPageInfo();
				PageInfo->NextPage = nullptr;

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
				return GetMemoryPageInfo()->FreeMemoryBlockHead != nullptr;
			}

			HMemoryBlock* AllocateBlock()
			{
				HFreeMemoryBlockHeader*& NewBlockHeader = GetMemoryPageInfo()->FreeMemoryBlockHead;
				
				HMemoryBlock* NewBlock = (HMemoryBlock*)NewBlockHeader;

				// unlink the memory block
				NewBlockHeader = NewBlockHeader->Next;

				return NewBlock;
			}

			void DeallocateBlock(void* InPointer)
			{
				// validation checking
				check(IsInRange((unsigned char*)InPointer));
				
				// link to free block
				HFreeMemoryBlockHeader* NewFreeBlock = (HFreeMemoryBlockHeader*)InPointer;
				HFreeMemoryBlockHeader*& FreeBlockHead = GetMemoryPageInfo()->FreeMemoryBlockHead;
				NewFreeBlock->Next = FreeBlockHead;
				FreeBlockHead = NewFreeBlock;
			}

			// range check for validation
			bool IsInRange(unsigned char* InAddress) const
			{
				return (InAddress >= (unsigned char*)&MemoryBlocks[0])
					&& (InAddress <= (unsigned char*)&MemoryBlocks[GetTotalAllocBlockNum() - 1]);
			}

			// aliasing the memory page to proper type for easier interface
			union 
			{
				unsigned char Data[PageSize];
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

			check(MemoryPage.GetMemoryPageInfo()->NextPage == nullptr);
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
			CurrPage = (HMemoryPage*)MemoryPage.GetMemoryPageInfo()->NextPage;
			while (CurrPage != nullptr)
			{
				if (CurrPage->GetMemoryPageInfo()->NextPage == nullptr)
				{
					break;
				}
				CurrPage = (HMemoryPage*)CurrPage->GetMemoryPageInfo()->NextPage;
			}

			HMemoryPage* NewPage = AllocateMemoryPage();
			CurrPage->GetMemoryPageInfo()->NextPage = NewPage;

			return NewPage;
		}

		HMemoryPage* FindMemoryPageToDeallocate(void* InPointer)
		{
			unsigned char* MemoryAddress = (unsigned char*)InPointer;

			if (MemoryPage.IsInRange(MemoryAddress))
			{
				return &MemoryPage;
			}

			HMemoryPage* CurrPage = (HMemoryPage*)MemoryPage.GetMemoryPageInfo()->NextPage;
			while (CurrPage != nullptr)
			{
				if (MemoryPage.IsInRange(MemoryAddress))
				{
					return CurrPage;
				}

				CurrPage = (HMemoryPage*)CurrPage->GetMemoryPageInfo()->NextPage;
			}

			return nullptr;
		}

	public:
		void* AllocateMemoryBlock()
		{
			HMemoryPage* AllocatablePage = FindMemoryPageToAllocate();
			return (void*)AllocatablePage->AllocateBlock();
		}

		void DeallocateMemoryBlock(void* InPointer)
		{
			HMemoryPage* PageToDeallocate = FindMemoryPageToDeallocate(InPointer);
			check(PageToDeallocate != nullptr);

			PageToDeallocate->DeallocateBlock(InPointer);
		}
	};
}