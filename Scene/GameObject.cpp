/************************************************
*              	  GameObejct.cpp                *
*************************************************
*          This file is a part of:              *
*               COLUMBUS ENGINE                 *
*************************************************
*                Nika(Columbus) Red             *
*                   12.11.2017                  *
*************************************************/
#include <Scene/GameObject.h>

namespace Columbus
{

	C_GameObject::C_GameObject()
	{

	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	void C_GameObject::addChild(C_GameObject* aChild)
	{
		mChildren.push_back(aChild);
	}
	//////////////////////////////////////////////////////////////////////////////
	void C_GameObject::addComponent(C_Component* aComponent)
	{
		mComponents.push_back(aComponent);
	}
	//////////////////////////////////////////////////////////////////////////////
	void C_GameObject::setTransform(C_Transform aTransform)
	{
		mTransform = aTransform;
	}
	//////////////////////////////////////////////////////////////////////////////
	C_Transform C_GameObject::getTransform() const
	{
		return mTransform;
	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	void C_GameObject::update()
	{
		mTransform.update();

		for (auto Comp : mComponents)
			Comp->update();

		for (auto Child : mChildren)
			Child->update();
	}
	//////////////////////////////////////////////////////////////////////////////
	void C_GameObject::render()
	{
		for (auto Comp : mComponents)
			Comp->render(mTransform);

		for (auto Child : mChildren)
			Child->render();
	}
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	C_GameObject::~C_GameObject()
	{

	}

}

