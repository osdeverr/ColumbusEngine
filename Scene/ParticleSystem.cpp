#include <Scene/ParticleSystem.h>

namespace Columbus
{

	//////////////////////////////////////////////////////////////////////////////
	C_ParticleSystem::C_ParticleSystem(C_ParticleEmitter* aEmitter) :
		mEmitter(aEmitter)
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	bool C_ParticleSystem::onCreate()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////
	bool C_ParticleSystem::onUpdate()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////
	void C_ParticleSystem::update(const float aTimeTick)
	{
		if (mEmitter != nullptr)
			mEmitter->update(aTimeTick);
	}
	//////////////////////////////////////////////////////////////////////////////
	void C_ParticleSystem::render(C_Transform& aTransform)
	{
		if (mEmitter != nullptr)
			mEmitter->draw();
	}
	//////////////////////////////////////////////////////////////////////////////
	std::string C_ParticleSystem::getType()
	{
		return "ParticleSystem";
	}
	//////////////////////////////////////////////////////////////////////////////
	C_ParticleEmitter* C_ParticleSystem::getEmitter() const
	{
		return mEmitter;
	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	C_ParticleSystem::~C_ParticleSystem()
	{

	}

}




