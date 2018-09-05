#include <Scene/Transform.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Columbus
{

	Transform::Transform() :
		ModelMatrix(1.0f) {}

	Transform::Transform(Vector3 Pos) :
		Position(Pos),
		ModelMatrix(1.0f) {}

	Transform::Transform(Vector3 Pos, Vector3 Rot) :
		Position(Pos),
		Rotation(Rot),
		ModelMatrix(1.0f) {}

	Transform::Transform(Vector3 Pos, Vector3 Rot, Vector3 Scale) :
		Position(Pos),
		Rotation(Rot),
		Scale(Scale),
		ModelMatrix(1.0f)
	{
		SetRot(Rot);
	}

	void Transform::SetPos(Vector3 Pos)
	{
		this->Position = Pos;
	}
	
	void Transform::AddPos(Vector3 Pos)
	{
		this->Position += Pos;
	}
	
	Vector3 Transform::GetPos() const
	{
		return Position;
	}
	
	void Transform::SetRot(Vector3 Rot)
	{
		this->Rotation = Rot;
		SetRot(glm::quat(glm::vec3(Math::Radians(Rot.X), Math::Radians(Rot.Y), Math::Radians(Rot.Z))));
	}

	void Transform::SetRot(glm::quat InRotation)
	{
		RotationQuaternion = InRotation;
	}
	
	void Transform::AddRot(Vector3 Rot)
	{
		this->Rotation += Rot;
	}
	
	Vector3 Transform::GetRot() const
	{
		return Rotation;
	}
	
	void Transform::SetScale(Vector3 Scale)
	{
		this->Scale = Scale;
	}
	
	void Transform::AddScale(Vector3 Scale)
	{
		this->Scale += Scale;
	}
	
	Vector3 Transform::GetScale() const
	{
		return Scale;
	}
	
	void Transform::Update()
	{
		glm::mat4 RotationMatrix = glm::mat4_cast(RotationQuaternion);
		float* Value = glm::value_ptr(RotationMatrix);

		Matrix tRotationMatrix;
		memcpy(&tRotationMatrix.M[0][0], Value, 64);

		ModelMatrix.SetIdentity();
		ModelMatrix.Scale(Scale);
		//ModelMatrix.Rotate(Vector3(1, 0, 0), Rotation.X);
		//ModelMatrix.Rotate(Vector3(0, 1, 0), Rotation.Y);
		//ModelMatrix.Rotate(Vector3(0, 0, 1), Rotation.Z);
		ModelMatrix = ModelMatrix * tRotationMatrix;
		ModelMatrix.Translate(Position);
	}
	
	void Transform::SetMatrix(Matrix InMatrix)
	{
		this->ModelMatrix = InMatrix;
	}
	
	const Matrix& Transform::GetMatrix() const
	{
		return ModelMatrix;
	}
	
	Transform::~Transform()
	{

	}

}


