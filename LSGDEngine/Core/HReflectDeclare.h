#pragma once

// to prevent include header order/circular problem

// register primitive types
#define DECLARE_PRIMITIVE_TYPE(PrimitiveType)	\
	class HStatic##PrimitiveType	\
	{	\
	public:	\
		HStatic##PrimitiveType();	\
	};	\
	extern HStatic##PrimitiveType Static##PrimitiveType; 

#define DECLARE_CLASS_TYPE(ClassType, SuperClassType)	\
	static lsgd::string GetClassName() { return #ClassType; }	\
