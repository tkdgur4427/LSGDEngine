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
//#define HSlateNew(WidgetType, ...) \
//	MakeTDecl<WidgetType>(#WidgetType, __FILE__, __LINE__, )
	