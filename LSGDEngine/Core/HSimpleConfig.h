#pragma once

#include "HFileArchive.h"

namespace lsgd
{
	class HSimpleConfig : public fileIO::HFileArchiveRead
	{
	public:
		HSimpleConfig(const HString& InFileName)
			: fileIO::HFileArchiveRead(InFileName)
		{}

		virtual ~HSimpleConfig() {}

		void Load();

		// source class to dest class
		HHashMap<HString, HString> ClassRedirectors;
	};
}