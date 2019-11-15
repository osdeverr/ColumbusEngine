#pragma once

#include <Scene/Component.h>
#include <Audio/AudioSource.h>
#include <Core/Assert.h>

namespace Columbus
{

	class ComponentAudioSource : public Component
	{
	private:
		std::shared_ptr<AudioSource> Source;

		friend class Scene;
	public:
		ComponentAudioSource(std::shared_ptr<AudioSource> InSource) : Source(InSource)
		{
			COLUMBUS_ASSERT((bool)Source);
		}

		virtual void Update(float TimeTick, Transform& Trans) override
		{
			COLUMBUS_ASSERT((bool)Source);
			Source->Position = Trans.Position;
		}

		//This component methods
		virtual Type GetType() const override { return Component::Type::AudioSource; }
		auto GetSource() const { return Source; }

		virtual ~ComponentAudioSource() override {}
	};

}


