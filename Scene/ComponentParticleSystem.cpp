#include <Scene/ComponentParticleSystem.h>

namespace Columbus
{

	ComponentParticleSystem::ComponentParticleSystem(ParticleEmitter* InEmitter) :
		Emitter(InEmitter)
	{ }
	
	void ComponentParticleSystem::Update(const float TimeTick)
	{
		if (Emitter != nullptr)
		{
			Emitter->update(TimeTick);
		}
	}
	
	void ComponentParticleSystem::Render(Transform& Transform)
	{
		if (Emitter != nullptr)
		{
			if (Emitter->getParticleEffect() != nullptr)
			{
				Emitter->getParticleEffect()->setPos(Transform.GetPos());
			}
			
			Emitter->draw();
		}
	}
	
	Component::Type ComponentParticleSystem::GetType() const
	{
		return Component::Type::ParticleSystem;
	}
	
	ParticleEmitter* ComponentParticleSystem::GetEmitter() const
	{
		return Emitter;
	}
	
	void ComponentParticleSystem::SetLights(std::vector<Light*> Lights)
	{
		if (Emitter != nullptr)
		{
			Emitter->setLights(Lights);
		}
	}
	
	void ComponentParticleSystem::SetCamera(Camera Camera)
	{
		if (Emitter != nullptr)
		{
			Emitter->setCamera(Camera);
		}
	}
	
	ShaderProgram* ComponentParticleSystem::GetShader() const
	{
		if (Emitter != nullptr)
		{
			return Emitter->getParticleEffect()->getMaterial()->GetShader();
		}

		return nullptr;
	}
	
	void ComponentParticleSystem::SetShader(ShaderProgram* Shader)
	{
		if (Emitter != nullptr)
		{
			Emitter->getParticleEffect()->getMaterial()->SetShader(Shader);
		}
	}
	
	ComponentParticleSystem::~ComponentParticleSystem()
	{
		delete Emitter;
	}

}




