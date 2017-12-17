#include <Scene/LightComponent.h>

namespace Columbus
{

	//////////////////////////////////////////////////////////////////////////////
	C_LightComponent::C_LightComponent(C_Light* aLight) :
		mLight(aLight)
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	bool C_LightComponent::onCreate()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////
	bool C_LightComponent::onUpdate()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////
	void C_LightComponent::update(const float aTimeTick)
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	void C_LightComponent::render(C_Transform& aTransform)
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	std::string C_LightComponent::getType()
	{
		return "LightComponent";
	}
	//////////////////////////////////////////////////////////////////////////////
	C_Light* C_LightComponent::getLight() const
	{
		return mLight;
	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	C_LightComponent::~C_LightComponent()
	{

	}

}



