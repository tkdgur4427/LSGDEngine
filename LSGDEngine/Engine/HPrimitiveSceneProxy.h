#pragma once

namespace lsgd {

	/*
		Same as UE4, primitivesceneproxy is the data for primitivecomponent
			- note that HPrimitiveSceneInfo is the state, whereas HPrimitiveSceneProxy is the data!
			- e.g. class HStaticMeshSceneProxy : public HPrimitiveSceneProxy { ... };
	*/
	class HPrimitiveSceneProxy
	{
	public:
		HPrimitiveSceneProxy() {}
		virtual ~HPrimitiveSceneProxy() {}
	};

}