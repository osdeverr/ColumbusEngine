#include <Scene/ParticleSystem.h>

namespace Columbus
{

	//////////////////////////////////////////////////////////////////////////////
	ParticleSystem::ParticleSystem(ParticleEmitter* aEmitter) :
		mEmitter(aEmitter)
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	bool ParticleSystem::onCreate()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////
	bool ParticleSystem::onUpdate()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////
	void ParticleSystem::update(const float aTimeTick)
	{
		if (mEmitter != nullptr)
			mEmitter->update(aTimeTick);
	}
	//////////////////////////////////////////////////////////////////////////////
	void ParticleSystem::render(Transform& aTransform)
	{
		if (mEmitter != nullptr)
		{
			if (mEmitter->getParticleEffect() != nullptr)
				mEmitter->getParticleEffect()->setPos(aTransform.getPos());
			
			mEmitter->draw();
		}
	}
	//////////////////////////////////////////////////////////////////////////////
	std::string ParticleSystem::getType()
	{
		return "ParticleSystem";
	}
	//////////////////////////////////////////////////////////////////////////////
	ParticleEmitter* ParticleSystem::getEmitter() const
	{
		return mEmitter;
	}
	//////////////////////////////////////////////////////////////////////////////
	void ParticleSystem::setLights(std::vector<Light*> aLights)
	{
		if (mEmitter != nullptr)
			mEmitter->setLights(aLights);
	}
	//////////////////////////////////////////////////////////////////////////////
	void ParticleSystem::setCamera(Camera aCamera)
	{
		if (mEmitter != nullptr)
			mEmitter->setCameraPos(aCamera.getPos());
	}
	//////////////////////////////////////////////////////////////////////////////
	Shader* ParticleSystem::getShader() const
	{
		if (mEmitter != nullptr)
			return mEmitter->getParticleEffect()->getMaterial()->getShader();
	}
	//////////////////////////////////////////////////////////////////////////////
	void ParticleSystem::setShader(Shader* aShader)
	{
		if (mEmitter != nullptr)
			mEmitter->getParticleEffect()->getMaterial()->setShader(aShader);
	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	ParticleSystem::~ParticleSystem()
	{

	}

}




