#pragma once

template <class ClassType, class... BaseClassTypes>
struct HClassTypeHelper
{
	static bool IsClassType() { return false; }
	static lsgd::HString GetClassName() { return lsgd::HString(); }
};
