/************************************************
*              ParticleEmitter.h                *
*************************************************
*          This file is a part of:              *
*               COLUMBUS ENGINE                 *
*************************************************
*                Nika(Columbus) Red             *
*                   20.07.2017                  *
*************************************************/

#pragma once

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <sstream>
#include <algorithm>

#include <glm/glm.hpp>

#include <RenderAPI/Buffer.h>

#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <Graphics/Camera.h>
#include <Graphics/ParticleEffect.h>
#include <Graphics/Light.h>
#include <System/System.h>
#include <System/Timer.h>
#include <System/Random.h>
#include <Common/Noise/OctaveNoise.h>

namespace Columbus
{

	class C_Particle
	{
	private:
		size_t i;
	public:
		C_Vector4 color = C_Vector4(1, 1, 1, 1);
		C_Vector3 velocity = C_Vector3(0, 1, 0);
		C_Vector3 startPos = C_Vector3(0, 0, 0);
		C_Vector3 startEmitterPos = C_Vector3(0, 0, 0);
		C_Vector3 accel = C_Vector3(0, 0.1, 0);
		C_Vector3 pos;

		float rotation = 0.0;
		float rotationSpeed = 0.0;
		float age = 0.0;
		float TTL = 1.0;
		float cameraDistance = 1000.0;
		float noise[9];

		unsigned int frame = 0;

		bool active = false;

		void genCircle(const float aRadius, const bool aEmitFromShell)
		{
			float ang = C_Random::range(0.0, 6.283185306);
			float rad = C_Random::range(0.0, aRadius);

			if (aEmitFromShell) rad = aRadius;

			startPos.x = rad * cos(ang);
			startPos.y = 0.0;
			startPos.z = rad * sin(ang);
		}

		void genSphere(const float aRadius, const bool aEmitFromShell)
		{
			float rad = C_Random::range(0.0, aRadius);
			float phi = C_Random::range(0.0, 6.283185306);
			float tht = C_Random::range(0.0, 3.141592653);

			if (aEmitFromShell) rad = aRadius;

			startPos.x = rad * cos(phi) * sin(tht);
			startPos.y = rad * sin(phi) * sin(tht);
			startPos.z = rad * cos(tht);
		}

		void genCube(const C_Vector3 aSize, const bool aEmitFromShell)
		{
			if (aEmitFromShell)
			{
				switch (rand() % 3)
				{
				case 0:
					startPos.x = aSize.x / 2 * ((rand() % 2) ? -1 : 1);
					startPos.y = C_Random::range(-aSize.y / 2, aSize.y / 2);
					startPos.z = C_Random::range(-aSize.z / 2, aSize.z / 2);
					break;
				case 1:
					startPos.x = C_Random::range(-aSize.x / 2, aSize.x / 2);
					startPos.y = aSize.y / 2 * ((rand() % 2) ? -1 : 1);
					startPos.z = C_Random::range(-aSize.z / 2, aSize.z / 2);
					break;
				case 2:
					startPos.x = C_Random::range(-aSize.x / 2, aSize.x / 2);
					startPos.y = C_Random::range(-aSize.y / 2, aSize.y / 2);
					startPos.z = aSize.z / 2 * ((rand() % 2) ? -1 : 1);
					break;
				}
			} else
			{
				startPos.x = C_Random::range(-aSize.x / 2, aSize.x / 2);
				startPos.y = C_Random::range(-aSize.y / 2, aSize.y / 2);
				startPos.z = C_Random::range(-aSize.z / 2, aSize.z / 2);
			}
		}

		void update(const float aTimeTick, const vec3 aCamera, const vec3 aForce, const vec3 aNoise)
		{
			for (i = 0; i < 9; i++)
				noise[i] = fmod(noise[i] + 0.01, 256);

			pos = (velocity + aForce) * age + (accel * 0.5 * age * age);
			pos += startPos + startEmitterPos;
			pos += aNoise;

			rotation += rotationSpeed * aTimeTick;
			cameraDistance = pow(aCamera.x - pos.x, 2) + pow(aCamera.y - pos.y, 2) + pow(aCamera.z - pos.z, 2);
		}
	};

	struct C_ColorKey
	{
		C_Vector4 color = C_Vector4(1, 1, 1, 1);
		float key = 0.0;
	};

	class C_ParticleEmitter
	{
	private:
		C_ParticleEffect* mParticleEffect = nullptr;

		std::vector<C_Particle> mParticles;
		std::vector<C_Particle> mActiveParticles;
		std::vector<C_Light*> mLights;
		std::vector<C_ColorKey> mColorKeys;

		C_Buffer* mBuf = nullptr;
		C_Buffer* mTBuf = nullptr;
		C_Buffer* mCBuf = nullptr;
		C_Buffer* mPBuf = nullptr;
		C_Buffer* mLBuf = nullptr;

		C_OctaveNoise mNoise;

		float mLife = 0.0;
		float mMaxTTL = 0.0;

		C_Vector3 mCameraPos = C_Vector3(0, 0, 5);

		//Vertex buffer
		float vrts[18] =
		{
			1, 1, 0.0,
			-1, 1, 0.0,
			-1, -1, 0.0,
			-1, -1, 0.0,
			1, -1, 0.0,
			1, 1, 0.0
		};
		//UV buffer
		float uvs[12] =
		{
			1.0, 1.0,
			0.0, 1.0,
			0.0, 0.0,
			0.0, 0.0,
			1.0, 0.0,
			1.0, 1.0
		};
		
		void sort();
		void copyActive();
		void setBuffers();
		void setUniforms();
		void setShaderMaterial();
		void setShaderLightAndCamera();
		void calculateLights();
		void sortLights();
		void unbindAll();
		
		float mLightUniform[120];
		float* mVertData = nullptr;
		float* mUvData = nullptr;
		float* mColData = nullptr;
		float* mPosData = nullptr;
		float* mTimeData = nullptr;
		unsigned int mParticlesCount = 0;
	public:
		C_ParticleEmitter(const C_ParticleEffect* aParticleEffect);

		void setParticleEffect(const C_ParticleEffect* aParticleEffect);
		C_ParticleEffect* getParticleEffect() const;

		void update(const float aTimeTick);
		void draw();

		void setCameraPos(C_Vector3 aC);
		//Set light casters, which calculate to using in shaders
		void setLights(std::vector<C_Light*> aLights);

		~C_ParticleEmitter();
	};

}
