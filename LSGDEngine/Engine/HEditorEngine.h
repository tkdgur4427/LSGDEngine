#pragma once

#include "HBaseEngine.h"
#include "HImGuiPlatform.h"

namespace lsgd {

	class HEditorEngine : public HBaseEngine
	{
	public:
		HEditorEngine() {}
		virtual ~HEditorEngine() {}

		// @todo - think about how to refactoring this layer
		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual void Tick(float DeltaTime) override;

		unique_ptr<HImGuiPlatform> ImGuiPlatform;
	};

}