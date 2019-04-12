#pragma once

namespace lsgd
{
	class HSlateControlledConstruction
	{
	public:
		void* operator new (const size_t InSize)
		{
			return HGenericMemory::Allocate(InSize);
		}

		void* operator new (const size_t InSize, void* OutAddr)
		{
			return OutAddr;
		}

		void operator delete (void* InAddr)
		{
			return HGenericMemory::Deallocate(InAddr);
		}
	};

	/*
		abstract base class for slate widgets
		- DO NOT INHERIT DIRECTLY FROM WIDGET!

		Inheritance:
			Widget is not meant to be directly inherited;
				instead consider inheriting from LeafWidget or Panel
				which represent intended use cases and provide a succinct
				set of methods which to override
			SWidget is the base class for all interactive slate entities;
				SWidget's public interface describes everything that a Widget can do 
				and is fairly complex as a result

		Events:
			Events in Slate are implemented as virtual functions that the Slate
			system will call on a Widget in order to notify the Widget about an
			Importance occurence (e.g. a key press) or querying the Widget regarding
			some information (e.g. what mouse cursor should be displayed)

			Widget provides a default implementation for most events; the default
			implementation does nothing and does not handle the event

			some events are able to reply to the system by returning an FReply,
			FCursorReply, or similar object
	*/
	class HSWidget : public HSlateControlledConstruction, enable_shared_from_this<HSWidget>
	{
	public:
		bool bIsHovered : 1;
		bool bCanTick : 1;
		bool bCanSupportFocus : 1;

		/*
			can the widget ever support children? this will be false on SLeafWidgets,
			rather than setting this directly, you should probably inherit from SLeafWidget
		*/
		bool bCanHaveChildren : 1;

	};
}