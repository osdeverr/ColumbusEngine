/************************************************
*                  Shader.cpp                   *
*************************************************
*          This file is a part of:              *
*               COLUMBUS ENGINE                 *
*************************************************
*                Nika(Columbus) Red             *
*                   20.07.2017                  *
*************************************************/

#include <Graphics/Shader.h>

namespace Columbus
{

	//////////////////////////////////////////////////////////////////////////////
	C_Shader::C_Shader(std::string aVert, std::string aFrag) :
		mLoaded(false),
		mCompiled(false)
	{
		load(aVert, aFrag);
	}
	//////////////////////////////////////////////////////////////////////////////
	C_Shader::C_Shader() :
		mLoaded(false),
		mCompiled(false)
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//Load shader from two files
	bool C_Shader::load(std::string aVert, std::string aFrag)
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////
	bool C_Shader::compile()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	bool C_Shader::isCompiled() const
	{
		return mCompiled;
	}
	//////////////////////////////////////////////////////////////////////////////
	void C_Shader::bind() const
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	void C_Shader::unbind() const
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	void C_Shader::addAttribute(std::string aName, const int aValue)
	{
		mAttributes.emplace_back(aName, aValue);
	}
	//////////////////////////////////////////////////////////////////////////////
	void C_Shader::setUniform1i(std::string aName, const int aValue) const
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	void C_Shader::setUniform1f(std::string aName, const float aValue) const
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	void C_Shader::setUniform2f(std::string aName, const C_Vector2 aValue) const
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	void C_Shader::setUniform3f(std::string aName, const C_Vector3 aValue) const
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	void C_Shader::setUniform4f(std::string aName, const C_Vector4 aValue) const
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	void C_Shader::setUniformMatrix(std::string aName, const float* aValue) const
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	void C_Shader::setUniformArrayf(std::string aName, const float aArray[], const size_t aSize) const
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	C_Shader::~C_Shader()
	{

	}

}
