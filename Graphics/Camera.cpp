/************************************************
*                  Camera.cpp                   *
*************************************************
*          This file is a part of:              *
*               COLUMBUS ENGINE                 *
*************************************************
*             Nikolay(Columbus) Red             *
*                   20.07.2017                  *
*************************************************/

#include <Graphics/Camera.h>

namespace C
{

	static glm::mat4 PROJECTION_MATRIX;
	static glm::mat4 VIEW_MATRIX;

	//////////////////////////////////////////////////////////////////////////////
	//Return projection matrix
	glm::mat4 C_GetProjectionMatrix()
	{
		return PROJECTION_MATRIX;
	}
	//////////////////////////////////////////////////////////////////////////////
	//Return view matrix
	glm::mat4 C_GetViewMatrix()
	{
		return VIEW_MATRIX;
	}
	//////////////////////////////////////////////////////////////////////////////
	//Set perspective matrix
	void C_SetPerspective(float aFOV, float aAspect, float aN, float aF)
	{
		PROJECTION_MATRIX = glm::perspective(C_DegToRads(aFOV), aAspect, aN, aF);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Set orthographic matrix
	void C_SetOrtho(float aL, float aR, float aB, float aT, float aN, float aF)
	{
		PROJECTION_MATRIX = glm::ortho(aL, aR, aB, aT, aN, aF);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Set view matrix
	void C_SetCamera(C_Vector3 aEye, C_Vector3 aRef)
	{
		VIEW_MATRIX = glm::lookAt(aEye.toGLM(), aRef.toGLM(), glm::vec3(0, 1, 0));
	}
	//////////////////////////////////////////////////////////////////////////////
	//Constructor
	C_Camera::C_Camera()
	{
		mPos = glm::vec3(0, 0, 5);
		mRot = glm::vec3(0, 0, 0);
		mTarget = glm::vec3(0, 0, 4);
		mCameraDirection = glm::vec3(0, 0, -1);
		mCameraRight = glm::vec3(1, 0, 0);
		mCameraUp = glm::vec3(0, 1, 0);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Set camera posisition
	void C_Camera::setPos(C_Vector3 aPos)
	{
		mPos = aPos.toGLM();
	}
	//////////////////////////////////////////////////////////////////////////////
	//Set camera rotation
	void C_Camera::setRot(C_Vector3 aRot)
	{
		mRot = aRot.toGLM();
	}
	//////////////////////////////////////////////////////////////////////////////
	//Add position to current
	void C_Camera::addPos(C_Vector3 aPos)
	{
		mPos += aPos.toGLM();
	}
	//////////////////////////////////////////////////////////////////////////////
	//Add rotation to current
	void C_Camera::addRot(C_Vector3 aRot)
	{
		mRot += aRot.toGLM();
		C_Vector3 tmp;
		tmp.fromGLM(mRot);
		setRot(tmp);
	}
	//////////////////////////////////////////////////////////////////////////////
	//Set camera target
	void C_Camera::setTarget(C_Vector3 aTarget)
	{
		mTarget = aTarget.toGLM();
	}
	//////////////////////////////////////////////////////////////////////////////
	//Return camera direction
	C_Vector3 C_Camera::direction()
	{
		C_Vector3 tmp;
		tmp.fromGLM(mCameraDirection);
		return tmp;
	}
	//////////////////////////////////////////////////////////////////////////////
	//Return camera right direction
	C_Vector3 C_Camera::right()
	{
		C_Vector3 tmp;
		tmp.fromGLM(-mCameraRight);
		return tmp;
	}
	//////////////////////////////////////////////////////////////////////////////
	//Return camera up direction
	C_Vector3 C_Camera::up()
	{
		C_Vector3 tmp;
		tmp.fromGLM(mCameraUp);
		return tmp;
	}
	//////////////////////////////////////////////////////////////////////////////
	//Return camera position
	C_Vector3 C_Camera::pos()
	{
		C_Vector3 tmp;
		tmp.fromGLM(mPos);
		return tmp;
	}
	//////////////////////////////////////////////////////////////////////////////
	//Update camera
	void C_Camera::update()
	{
		/*glm::vec3 y = glm::vec3(0, 0, -1);
		C_Vector3 rot;
		rot.x = C_DegToRads(mRot.x);
		rot.y = C_DegToRads(mRot.y);
		rot.z = C_DegToRads(mRot.z);
		y = glm::rotateY(y, rot.y);
		y = glm::rotateX(y, rot.x);
		y = glm::rotateZ(y, rot.z);
		mTarget = y + mPos;*/

		glm::vec3 front;
		front.z = cos(glm::radians(mRot.x)) * cos(glm::radians(mRot.y));
		front.y = sin(glm::radians(mRot.x));
		front.x = cos(glm::radians(mRot.x)) * sin(glm::radians(mRot.y));
		mCameraDirection = glm::normalize(-front);

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		mCameraRight = glm::normalize(glm::cross(up, mCameraDirection));

		mCameraUp = glm::cross(mCameraDirection, mCameraRight);

		VIEW_MATRIX = glm::lookAt(mPos, mCameraDirection + mPos, mCameraUp);

		preTargeted = false;
	}
	//////////////////////////////////////////////////////////////////////////////
	//Destructor
	C_Camera::~C_Camera()
	{

	}

}