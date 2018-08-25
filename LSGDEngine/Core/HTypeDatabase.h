#pragma once

#include "HReflect.h"

namespace lsgd { namespace reflect {

	// primitive types
	template <class Type>
	class HPrimitiveType
	{
	public:
	};


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
			check(!ExistClass(InName));

			// declare the class instance
			unique_ptr<HClass> NewClass = lsgd::make_unique<HClass>(InName);
			Classes.push_back(move(NewClass));

			// generate class data for post-processing
			HPostProcessClassData NewPostprocessClassData;
			NewPostprocessClassData.ClassName = InName;
			NewPostprocessClassData.SuperClassName = InSuperClassName;

			PostProcessClassDataList.push_back(NewPostprocessClassData);
		}

		bool ExistClass(const string& InClassName)
		{
			for (auto& Class : Classes)
			{
				if (Class->Name == InClassName)
				{
					return true;
				}
			}
			return false;
		}

		int32 GetClassIndex(const string& InClassName)
		{
			int32 Index = 0;
			for (auto& Class : Classes)
			{
				if (Class->Name == InClassName)
				{
					return Index;
				}

				Index++;
			}
			return -1;
		}

		template <class ClassType, class FieldType>
		void AddClassField(const string& InFieldName, FieldType ClassType::*InField)
		{
			string ClassName = ClassType::GetName();
			check(ExistClass(ClassName));

			int32 ClassIndex = GetClassIndex(ClassName);
			int32 FieldOffset = (int32)StructOffsetOf<ClassType, FieldType>(InField);
			int32 FieldSize = sizeof(FieldType);

			Classes[ClassIndex]->LinkProperty(FieldOffset, FieldSize);
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

#define DECLARE_CLASS_TYPE(ClassType, SuperClassType)	\
	static lsgd::string GetClassName() { return #ClassType; }	\

#define IMPLEMENT_CLASS_TYPE(ClassType, SuperClassType)