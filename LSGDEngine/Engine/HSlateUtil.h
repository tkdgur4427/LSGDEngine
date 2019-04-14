#pragma once

/*
	note that this project is for studying UE4 architecture
	you can notice that most of features for UI are similar to UE4's slate architecture 
*/

namespace RequiredArgs {
	
	template <typename... ArgTypes>
	struct HRequiredArgs
	{
		HRequiredArgs(ArgTypes&&... InArguments) 
		{
			// make arguments with variadic parameter to tuple data
			Arguments = HMakeTuple(forward<ArgTypes>(InArguments)...);
		}

		template <class WidgetType>
		void CallConstruct(const shared_ptr<WidgetType>& OnWidget, const typename WidgetType::HArguments& WithNamedArgs) const
		{
			// your widget must implement void Construct(const FArguments& InArgs)
			OnWidget->Construct(Arguments, HMakeIntegerSequence<ArgTypes, sizeof...(ArgTypes)>());
		}

		HTuple<ArgTypes...> Arguments;
	};

	template <typename... ArgTypes>
	auto HMakeRequiredArgs(ArgTypes&&... InArguments) -> HRequiredArgs<ArgTypes...>
	{
		return HRequiredArgs(forward<ArgTypes>(InArguments)...);
	}
}

// normal widgets are allocated directly by the HDecl
template <typename WidgetType>
struct HWidgetAllocator
{
	static shared_ptr<WidgetType> PrivateAllocateWidget()
	{
		return make_shared<WidgetType>();
	}
};

/*
	utility class during widget instantiation
		- perform widget allocation and construction ensures that debug info is set correctly return TSharedRef to widget
*/
template <class WidgetType, typename RequiredArgsPayloadType>
struct HDecl
{
	HDecl(const char* InType, const char* InFile, int32 Online, RequiredArgsPayloadType&& InRequiredArgs)
		: Widget(HWidgetAllocator<WidgetType>::PrivateAllocateWidget())
		, RequiredArgs(InRequiredArgs)
	{}

	// complete widget construction from InArgs
	shared_ptr<WidgetType> operator<<=(const typename WidgetType::HArgument& InArgs) const
	{
		RequiredArgs.CallConstruct(Widget, InArgs);
		return Widget;
	}

	const shared_ptr<WidgetType> Widget;
	RequiredArgsPayloadType& RequiredArgs;
};

template <typename WidgetType, typename RequiredArgsPayloadType>
HDecl<WidgetType, RequiredArgsPayloadType> MakeHDecl(const char* InType, const char* InFile, int32 InLine, RequiredArgsPayloadType&& InRequiredArgs)
{
	return HDecl<WidgetType, RequiredArgsPayloadType>(InType, InFile, InLine, forward<RequiredArgsPayloadType>(InRequiredArgs);
}

// base class for named arguments; provides setting necessary for all widgets
template <typename WidgetType>
struct HSlateBaseNamedArgs
{
	using WidgetArgsType = typename WidgetType::HArguments;

	// used by the named argument pattern as a safe way to return *this for call-chaining purpose
	WidgetArgsType& Me()
	{
		return *(static_cast<WidgetArgsType*)(this));
	}

	//...
};

// a widget reference that is always a valid pointer
struct HAlwaysValidWidget
{
	shared_ptr<class HSWidget> Widget;
};

/*
	we want to be able to do:
		SNew(ContainerWidget).SomeContentArea()
		[
			// child widgets go here
		]
	NamedSlotProperty is a helper that will be returned by SomeContentArea()
*/
template <class DeclarationType>
struct NamedSlotProperty
{
	NamedSlotProperty(DeclarationType& InOwnerDeclaration, HAlwaysValidWidget& ContentToSet)
		: OwnerDeclaration(InOwnerDeclaration)
		, SlotContent(ContentToSet)
	{}

	DeclarationType& operator[](const shared_ptr<HSWidget>& InChild)
	{
		SlotContent.Widget = InChild;
		return OwnerDeclaration;

	DeclarationType& OwnerDeclaration;
	HAlwaysValidWidget& SlotContent;
};

/*
	widget authors can use SLATE_BEGIN_ARGS and SLATE_END_ARGS to add support 
	for widget construction via SNew and SAssignNew
	e.g.
		TSharedRef<SButton> MyButton = SNew(SButton);
			or
		TSharedPtr<SButton> MyButton;
		SAssignNew(MyButton, SButton);
	using SNew and SAssignNew ensures that widgets are populated
*/

// HSlateNew
#define HSNew(WidgetType, ...) \
	MakeHDecl<WidgetType>(#WidgetType, __FILE__, __LINE__, RequiredArgs::HMakeRequiredArgs(__VA_ARGS__)) <<= WidgetType::HArguments()

/*
	widget authors can use SLATE_BEGIN_ARGS and SLATE_END_ARGS to add support for widget
	construction via SNew and SAssignNew
	e.g.
		SLATE_BEGIN_ARGS(SMyWidget)
			, _PreferredWidth(...)
			, _ForegroundColor(...)
			{}

			SLATE_ATTRIBUTE(float, PreferredWidth)
			SLATE_ATTRIBUTE(FSlateColor, ForegroundColor)
		SLATE_END_ARGS()
*/
#define HSLATE_BEGIN_ARGS(WidgetType) \
	public: \
	struct HArguments : public HSlateBaseNamedArgs<WidgetType> \
	{ \
		using WidgetArgsType = HArguments; \
		HArguments()

/*
	just like SLATE_BEGIN_ARGS but requires the user to implement the New() method in the .cpp
	allows for widget implementation details to be entirely reside in the .cpp file

	e.g.
		SMyWidget.h:
		-----
		SLATE_USER_ARGS(SMyWidget)
		{}
		SLATE_END_ARGS()

		virtual void DoStuff() = nullptr;

		SMyWidget.cpp:
		-----
		namespace Implementation {
			class SMyWidget : public SMyWidget {
				void Construct(const FArguments& InArgs)
				{
					SUserWidget::Construct(SUserWidget::FArgument()
					[
						SNew(STextBlock).Text("implementation details!")
					]);
				}

				virtual void DoStuff() override {}

				FReply OnButtonClicked {}

				TSharedPtr<SButton> MyButton;
			};
		}
*/
#define HSLATE_USER_ARGS(WidgetType) \
	public: \
	static shared_ptr<WidgetType> New(); \
	struct HArguments; \
	struct HArguments : public HSlateBaseNamedArgs<WidgetType> \
	{ \
		using WidgetArgType = HArguments; \
		HArguments()

#define HSLATE_END_ARGS() \
	};

/*
	use this macro to add an attribute to the declaration of your widget
	and attribute can be a value or a function
*/
#define HSLATE_ATTRIBUTE(AttrType, AttrName) \
	HAttribute<AttrType> _##AttrName; \
	WidgetArgsType& AttrName(const HAttribute<AttrType>& InAttribute) \
	{ \
		_##AttrName = InAttribute; \
		return this->Me(); \
	}

/*
	use this macro to declare a slate argument
	arguments differ from attributes in that they can only be values
*/
#define HSLATE_ARGUMENT(ArgType, ArgName) \
	ArgType _##ArgName; \
	WidgetArgsType& ArgName(ArgType InArg) \
	{ \
		_##ArgName = InArg; \
		return this->Me(); \
	}

/*
	use this macro to add support for named slot properties such as Content and Header
	see NameSlotProperty for more details

	NOTE: if you're using within a widget class that is templated, then you might
	have to specify a full name for the declaration
	e.g. SLATE_NAMED_SLOT(typename SSuperWidget<T>::Declaration, Content)
*/

#define HSLATE_NAMED_SLOT(DeclarationType, SlotName) \
	NamedSlotProperty<Declaration> SlotName() \
	{ \
		return NamedSlotProperty<DeclarationType>(*this, __SlotName); \
	} \
	HAlwaysValidWidget __##SlotName;

#define HSLATE_DEFAULT_SLOT(DeclarationType, SlotName) \
	HSLATE_NAMED_SLOT(DeclarationType, SlotName) \
	DeclarationType& operator[](const shared_ptr<HSWidget> InChild) \
	{ \
		_##SlotName.Widget = InChild; \
		return *this; \
	}
	