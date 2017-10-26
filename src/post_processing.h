#pragma once

#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <GL/glew.h>

#include "graphics.h"
#include "resource_manager.h"

namespace graphics
{
	class PostProcessEffect
	{

	public:

		PostProcessEffect() {};

		virtual ~PostProcessEffect() {};

		virtual void Init() = 0;

		virtual void Apply() = 0;

		static void Init(const GraphicsBase &graphics_base);

		static void Render();

	protected:

		static const GraphicsBase * graphics_base_;

	};

	class PostProcessing
	{		
		GraphicsBase &graphics_base_;

		std::vector<std::unique_ptr<PostProcessEffect>> effects_;

	public:
		
		PostProcessing(GraphicsBase &graphics_base);

		~PostProcessing();

		void Add(std::unique_ptr<PostProcessEffect> effect);

		void Process();

	};
}
