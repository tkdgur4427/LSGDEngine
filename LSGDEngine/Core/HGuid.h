#pragma once

namespace lsgd {

	class HGuid
	{
	public:
		static HGuid CreateGuid();

		// guid real object
		struct GuidInstance {};

		// methods
		string ToString();

	protected:		
		shared_ptr<GuidInstance> Value;

		explicit HGuid(shared_ptr<GuidInstance>& InValue)
			: Value(InValue)
		{}
	};
}