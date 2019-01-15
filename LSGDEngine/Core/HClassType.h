#pragma once

template <class ClassType>
struct HClassTypeHelper
{
	static bool IsClassType() { return false; }
	static lsgd::HString GetClassName() { return lsgd::HString(); }
};
