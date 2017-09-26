#pragma once

#include <vector>

#include "glm/glm.hpp"

namespace graphics {

	struct LightData
	{
		bool shadow;
		glm::vec3 color;
		glm::vec3 position;
		glm::vec3 direction;
	};

	class LightDataPipe
	{
		std::vector<LightData> data_;

	public:

		LightDataPipe() {}

		~LightDataPipe() {}

		void Push(const LightData &light_data)
		{
			data_.push_back(light_data);
		}

		void Flush()
		{
			data_.clear();
		}

		inline const std::vector<LightData> &GetData() const { return data_; }
	};
}
