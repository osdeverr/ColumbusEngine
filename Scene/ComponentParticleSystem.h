#pragma once

#include <Scene/Component.h>
#include <Graphics/Camera.h>
#include <Graphics/Light.h>
#include <Graphics/Particles/ParticleEmitter.h>

namespace Columbus
{

	class ComponentParticleSystem : public Component
	{
	private:
		ParticleEmitter* Emitter = nullptr;
	public:
		ComponentParticleSystem(ParticleEmitter* InEmitter) : Emitter(InEmitter) {}

		virtual void Render(Transform& Transform) override {}
		virtual void Update(float TimeTick, Transform& Trans) override
		{
			if (Emitter != nullptr)
			{
				Emitter->update(TimeTick);
			}
		}

		//This component methods
		virtual Type GetType() const override { return Component::Type::ParticleSystem; }
		ParticleEmitter* GetEmitter() const { return Emitter; }
		void SetLights(std::vector<Light*> Lights)
		{
			if (Emitter != nullptr)
			{
				Emitter->setLights(Lights);
			}
		}

		void SetCamera(Camera Camera)
		{
			if (Emitter != nullptr)
			{
				Emitter->setCamera(Camera);
			}
		}

		ShaderProgram* GetShader() const
		{
			if (Emitter != nullptr)
			{
				return Emitter->getParticleEffect()->getMaterial()->GetShader();
			}

			return nullptr;
		}

		void SetShader(ShaderProgram* Shader)
		{
			if (Emitter != nullptr)
			{
				Emitter->getParticleEffect()->getMaterial()->SetShader(Shader);
			}
		}

		virtual ~ComponentParticleSystem() override {}
	};

}







