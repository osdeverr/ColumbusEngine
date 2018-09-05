#pragma once

#include <Graphics/Buffer.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <Graphics/Mesh.h>
#include <Graphics/MeshInstanced.h>
#include <Graphics/Framebuffer.h>

namespace Columbus
{

	class Device
	{
	public:
		Device();

		virtual Buffer* CreateBuffer() const;

		virtual ShaderStage* CreateShaderStage() const;
		virtual ShaderProgram* CreateShaderProgram() const;
		
		virtual Texture* CreateTexture() const;

		virtual Mesh* CreateMesh() const;
		virtual MeshInstanced* CreateMeshInstanced() const;

		virtual Framebuffer* createFramebuffer() const;

		virtual ~Device();
	};

	extern Device* gDevice;

}










