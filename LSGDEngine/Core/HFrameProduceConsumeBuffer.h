#pragma once

namespace lsgd { namespace containers {

	template <int32 BufferSize>
	class HFrameProduceConsumeBuffer
	{
	public:
		// @todo - only for shipping?
		struct HBufferHeader
		{
			int32 Size;
		};

		enum 
		{
			HeaderSize = sizeof(HBufferHeader),
		};

		HFrameProduceConsumeBuffer()
			: ProduceOffset(0)
			, ConsumeOffset(0)
			, FrameNumber(-1)
		{}

		~HFrameProduceConsumeBuffer() {}

		void Reset(int32 InFrameNumber)
		{
			// when frame number is same, just skip it
			if (FrameNumber == InFrameNumber)
			{
				return;
			}

			if (ProduceOffset == ConsumeOffset)
			{
				ProduceOffset = 0;
				ConsumeOffset = 0;
				return;
			}

			// no need to memory copy, just return
			if (ConsumeOffset == 0)
			{
				return;
			}

			// need to copy the data
			int32 SizeToCopy = ProduceOffset - ConsumeOffset;
			HGenericMemory::MemCopy(GetData(), GetData(ConsumeOffset), SizeToCopy);

			// reset the offset
			//	- note that produce offset should be size of data remained in the buffer
			ProduceOffset = SizeToCopy;
			ConsumeOffset = 0;
		}

		int32 GetRemainSize() const
		{
			return BufferSize - ProduceOffset;
		}

		uint8* GetData(int32 InOffset = 0)
		{
			return &Data[0] + InOffset;
		}

		bool IsEnough(int32 InRequestedSize) const
		{
			int32 RemainSize = GetRemainSize();
			return (RemainSize > InRequestedSize);
		}

		uint8* Produce(int32 InSize)
		{
			int32 TotalSize = HeaderSize + InSize;
			if (!IsEnough(TotalSize))
			{
				return nullptr;
			}

			// record the header
			HBufferHeader* Header = (HBufferHeader*)GetData(ProduceOffset);
			Header->Size = InSize;

			// update offset
			ProduceOffset += TotalSize;

			// get the result with offsetting header size
			uint8* Output = (uint8*)Header + HeaderSize;

			return Output;
		}

		// note that there is no deallocate method! you can only reset!
		uint8* Consume(int32 InSize)
		{
			HBufferHeader* Header = (HBufferHeader*)GetData(ConsumeOffset);
			check(Header->Size == InSize);

			uint8* Output = (uint8*)Header + HeaderSize;

			// update the consume offset
			ConsumeOffset += (InSize + HeaderSize);
			check(ConsumeOffset <= ProduceOffset);

			// for memory efficiency, when consume offset and produce offset is same, reset!
			//	- when these offsets are same, it means the buffer is empty
			if (ConsumeOffset == ProduceOffset)
			{
				ProduceOffset = 0;
				ConsumeOffset = 0;
			}			

			return Output;
		}

		// frame number
		int32 FrameNumber;

		// offset for produce and consume
		int32 ProduceOffset;
		int32 ConsumeOffset;

		// data
		uint8 Data[BufferSize];
	};
}
}