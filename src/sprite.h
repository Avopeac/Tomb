#pragma once

#include "glm/glm.hpp"

namespace graphics
{
	class Sprite
	{

		glm::mat4 transform_;

		size_t layer_ = 0;

		size_t texture_hash_ = 0;

		size_t animation_hash_ = 0;

	public:

		Sprite(glm::mat4 transform);

		Sprite(const Sprite &);

		Sprite(Sprite &&);

		~Sprite();

		Sprite &operator=(const Sprite &other)
		{
			transform_ = other.transform_;
			layer_ = other.layer_;
			texture_hash_ = other.texture_hash_;
			animation_hash_ = other.animation_hash_;
		}

		Sprite &operator=(Sprite &&other)
		{
			transform_ = other.transform_;
			layer_ = other.layer_;
			texture_hash_ = other.texture_hash_;
			animation_hash_ = other.animation_hash_;
		}

		inline void SetLayer(size_t layer) { layer_ = layer; }

		inline void SetTexture(size_t texture_hash) { texture_hash_ = texture_hash; }

		inline void SetAnimation(size_t animation_hash) { animation_hash_ = animation_hash; }

		inline void SetTranform(const glm::mat4 &transform) { transform_ = transform; }

		inline size_t GetLayer() const { return layer_; }

		inline size_t GetTexture() const { return texture_hash_; }

		inline size_t GetAnimation() const { return animation_hash_; }

		inline const glm::mat4 &GetTransform() const { return transform_; }

		inline glm::mat4 GetTransformCopy() const { return transform_; }

	};

}