#pragma once

#include "glm/glm.hpp"

namespace graphics
{
	class Sprite
	{

		glm::mat4 transform_;

		size_t layer_ = 0;

	public:

		Sprite(glm::mat4 transform, size_t layer);

		Sprite(const Sprite &);

		Sprite(Sprite &&);

		~Sprite();

		Sprite &operator=(const Sprite &other)
		{
			transform_ = other.transform_;
			layer_ = other.layer_;
		}

		Sprite &operator=(Sprite &&other)
		{
			transform_ = other.transform_;
			layer_ = other.layer_;
		}

		inline void SetLayer(size_t layer) { layer_ = layer; }

		inline void SetTransform(const glm::mat4 &transform) { transform_ = transform; }

		inline size_t GetLayer() const { return layer_; }

		inline const glm::mat4 &GetTransform() const { return transform_; }

		inline glm::mat4 GetTransformCopy() const { return transform_; }

	};

}