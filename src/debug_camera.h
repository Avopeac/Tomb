#pragma once

#include "abstract_camera.h"

namespace graphics
{
	class DebugCamera : public AbstractCamera
	{
		
	public:

		DebugCamera(const std::string &name);

		~DebugCamera();

		virtual void Update(float delta_time) override;

		virtual void Render() override;
	};
}
