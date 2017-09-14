#pragma once

#include <string>

#include "glm/glm.hpp"

namespace graphics
{
	class AbstractCamera
	{

		std::string name_;

	protected:

		glm::vec4 position_;
		glm::vec4 up_;
		glm::vec4 forward_;

		glm::mat4 view_;
		glm::mat4 proj_;
		glm::mat4 viewproj_;
		glm::mat4 inv_view_;
		glm::mat4 inv_proj_;
		glm::mat4 inv_viewproj_;

	public:

		AbstractCamera(const std::string &name,
			const glm::vec4 &position,
			const glm::vec4 &forward,
			const glm::vec4 &up) :
			name_(name),
			position_(position),
			forward_(forward),
			up_(up)
		{

		}

		virtual ~AbstractCamera() {};

		virtual void Update(float delta_time) = 0;

		virtual void Render() = 0;

		const glm::mat4 &GetView() const { return view_; }

		const glm::mat4 &GetProj() const { return proj_; }

		const glm::mat4 &GetViewProj() const { return viewproj_; }

		const glm::mat4 &GetInvView() const { return inv_view_; }

		const glm::mat4 &GetInvProj() const { return inv_proj_; }

		const glm::mat4 &GetInvViewProj() const { return inv_viewproj_; }

		const std::string &GetName() const { return name_; }

		const glm::vec4 &GetForward() const { return forward_; }

		const glm::vec4 &GetPosition() const { return position_; }

		void SetPosition(const glm::vec4 &position) { position_ = position; }

		const glm::vec4 &GetUp() const { return up_; }
	};
}
