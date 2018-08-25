#pragma once

#include "HReflect.h"

namespace lsgd { namespace reflect {

	// post-process class reflection data
	struct HPostProcessClassData
	{	
		string ClassName;
		string SuperClassName;
	};

	class HTypeDatabase
	{
	public:
		static HTypeDatabase * GetSingleton()
		{
			static HTypeDatabase TypeDatabase;
			return &TypeDatabase;
		}

		void AddClassType(const string& InName, const string& InSuperClassName)
		{
			//@todo - assertion when Classes already have same type of class

			// declare the class instance
			unique_ptr<HClass> NewClass = lsgd::make_unique<HClass>(InName);
			Classes.push_back(move(NewClass));

			// generate class data for post-processing
			HPostProcessClassData NewPostprocessClassData;
			NewPostprocessClassData.ClassName = InName;
			NewPostprocessClassData.SuperClassName = InSuperClassName;

			PostProcessClassDataList.push_back(NewPostprocessClassData);
		}

		// generated class types
		vector<unique_ptr<HClass>> Classes;

		// temporary data - after processing class data, it will removes all data
		vector<HPostProcessClassData> PostProcessClassDataList;

	protected:
		HTypeDatabase() {}
	};
}
}

#define DECLARE_CLASS_TYPE(ClassType, SuperClassType)

#define IMPLEMENT_CLASS_TYPE(ClassType, SuperClassType)