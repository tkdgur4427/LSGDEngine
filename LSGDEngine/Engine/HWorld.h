#pragma once

namespace lsgd
{	
	DECLARE_CLASS_TYPE1(HWorld, HObject)
	class HWorld : public HObject
	{
		GENERATE_CLASS_BODY(HWorld);

		HWorld() {}
		virtual ~HWorld() {}
	};
}