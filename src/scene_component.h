#pragma once

namespace scene {
	
	class SceneObject;

	class SceneComponent
	{

		SceneObject * object_ = nullptr;

	public:

		SceneComponent() {};

		virtual ~SceneComponent() {};

		virtual void Initialize() = 0;

		virtual void Update(float delta_time) = 0;

		void AssignToObject(SceneObject * object) { object_ = object; }
	};
}
