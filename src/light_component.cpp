#include "light_component.h"

#include "data_pipe_hub.h"

using namespace scene;

LightComponent::LightComponent(bool shadow) :
	shadow_(shadow)
{
}

LightComponent::~LightComponent()
{
}

void LightComponent::Initialize()
{
}

void LightComponent::Update(float delta_time)
{
	graphics::LightData light_data;
	light_data.color = color_;
	light_data.direction = direction_;
	light_data.position = position_;
	light_data.shadow = shadow_;

	auto &hub = graphics::DataPipeHub::Get().GetLightDataPipe();
	hub.Push(light_data);
}
