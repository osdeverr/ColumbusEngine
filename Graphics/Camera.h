/************************************************
*                  Camera.h                     *
*************************************************
*          This file is a part of:              *
*               COLUMBUS ENGINE                 *
*************************************************
*                Nika(Columbus) Red             *
*                   20.07.2017                  *
*************************************************/

#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/closest_point.hpp>

#include <System/System.h>
#include <Math/Matrix4.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>

namespace Columbus
{

	class Camera
	{
	private:
		Vector3 mPos = Vector3(0, 0, 5);
		Vector3 mRot = Vector3(0, 0, 0);
		Vector3 mTarget = Vector3(0, 0, 4);
		Vector3 mCameraDirection = Vector3(0, 0, -1);
		Vector3 mCameraRight = Vector3(1, 0, 0);
		Vector3 mCameraUp = Vector3(0, 1, 0);
		Matrix4 mProjectionMatrix;
		Matrix4 mViewMatrix;

		bool rotMode = false;
		bool preTargeted = false;
	public:
		Camera();
		
		void update();
		
		void setPos(const Vector3 aPos);
		void addPos(const Vector3 aPos);
		Vector3 getPos() const;

		void setRot(const Vector3 aRot);
		void addRot(const Vector3 aRot);
		Vector3 getRot() const;

		void setTarget(const Vector3 aTarget);
		void addTarget(const Vector3 aTarget);
		Vector3 getTarget() const;
		
		Vector3 direction() const;
		Vector3 right() const;
		Vector3 up() const;

		void perspective(const float aFOV, const float aAspect, const float aNear, const float aFar);
		//void ortho();

		Matrix4 getProjectionMatrix() const;
		Matrix4 getViewMatrix() const;
		
		~Camera();
	};

}
