#pragma once

#include <Graphics/Material.h>
#include <Scene/Transform.h>
#include <Scene/Component.h>
#include <Core/SmartPointer.h>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace Columbus
{

	class GameObject
	{
	protected:
		std::vector<SmartPointer<GameObject>> Children;
		std::vector<SmartPointer<Component>> Components;
	public:
		Transform transform;
		Material material;
		std::string Name;
		bool Enable = true;
	public:
		GameObject();
		GameObject(const GameObject&) = delete;
		GameObject(GameObject&&) = default;

		void AddChild(GameObject* Child);
		void AddComponent(Component* Component);

		void Update(float DeltaTime);
		void Render();

		/*template <typename T>
		T* GetComponent()
		{
			for (const auto& Comp : Components)
			{
				T* C = dynamic_cast<T*>(Comp.Get());
				if (C != nullptr) return C;
			}

			return nullptr;
		}

		template <typename T>
		bool DeleteComponent()
		{
			if (GetComponent<T>() != nullptr)
			{
				auto Pred = [](const SmartPointer<Component>& A)
				{
					return dynamic_cast<T*>(A.Get()) != nullptr;
				};
				auto Removed = std::remove_if(Components.begin(), Components.end(), Pred);
				Components.erase(Removed, Components.end());

				return true;
			}

			return false;
		}*/

		bool HasComponent(Component::Type Type);
		Component* GetComponent(Component::Type Type);

		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = default;

		~GameObject();
	};

}


