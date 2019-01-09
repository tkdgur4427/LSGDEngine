#pragma once

namespace lsgd {

	/*
		Same as UE4, HPrimitiveSceneInfo is aligned to HPrimitiveComponent
			- state of primitivecomponent in render-world
	*/
	class HPrimitiveSceneInfo
	{
	public:
		HPrimitiveSceneInfo() {}
		~HPrimitiveSceneInfo() {}

		// primitivecomponent render-data
		shared_ptr<class HPrimitiveSceneProxy> PrimitiveSceneProxy;
	};

}