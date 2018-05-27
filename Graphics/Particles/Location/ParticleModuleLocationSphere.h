#pragma once

#include <Graphics/Particles/Location/ParticleModuleLocationBase.h>

namespace Columbus
{

	class ParticleModuleLocationSphere : public ParticleModuleLocationBase
	{
	public:
		float Radius;
		bool EmitFromShell;

		ParticleModuleLocationSphere() :
			Radius(1.0f),
			EmitFromShell(false) {}
		/*
		* For determening module type
		*/
		Type GetType() const override { return Type::Location; }
		/*
		* Set parameter for particle
		*/
		void Spawn(Particle& OutParticle) override
		{
			float rad = Random::range(0.0, Radius);
			float phi = Random::range(0.0, 6.283185306);
			float tht = Random::range(0.0, 3.141592653);

			if (EmitFromShell) rad = Radius;

			OutParticle.startPos.X = rad * Math::Cos(phi) * Math::Sin(tht);
			OutParticle.startPos.Y = rad * Math::Sin(phi) * Math::Sin(tht);
			OutParticle.startPos.Z = rad * Math::Cos(tht);
		}

		~ParticleModuleLocationSphere() override {}
	};

}





