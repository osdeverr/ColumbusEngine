#include <Graphics/RenderState.h>
#include <Graphics/OpenGL/ShaderOpenGL.h>
#include <Graphics/Device.h>
#include <GL/glew.h>

namespace Columbus
{

	struct MaterialRenderData
	{
		int32 Model;
		int32 ViewProjection;

		int32 TexturesIDs[11];

		int32 HasAlbedoMap;
		int32 HasNormalMap;
		int32 HasRoughnessMap;
		int32 HasMetallicMap;
		int32 HasOcclusionMap;
		int32 HasDetailAlbedoMap;
		int32 HasDetailNormalMap;

		int32 Tiling;
		int32 DetailTiling;
		int32 Albedo;
		int32 Roughness;
		int32 Metallic;
		int32 EmissionStrength;
		int32 Transparent;

		int32 CameraPosition;

		int32 Lighting;
	};

	void RenderState::Clear()
	{
		//SetCulling(Material::Cull::No);
		//SetBlending(true);
		//SetDepthTesting(Material::DepthTest::LEqual);
		//SetDepthWriting(false);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);

		PreviousMaterial = Material();
		CurrentMaterial  = Material();

		PreviousShader = nullptr;
		CurrentShader  = nullptr;

		PreviousMesh = nullptr;
		CurrentMesh  = nullptr;

		PreviousCulling = Material::Cull::No;
		PreviousBlending = true;
		PreviousDepthTesting = Material::DepthTest::LEqual;
		PreviousDepthWriting = false;
	}

	void RenderState::SetCulling(Material::Cull Culling)
	{
		if (Culling != PreviousCulling)
		{
			switch (Culling)
			{
			case Material::Cull::No:           glDisable(GL_CULL_FACE); break;
			case Material::Cull::Front:        glEnable(GL_CULL_FACE);  glCullFace(GL_FRONT);          break;
			case Material::Cull::Back:         glEnable(GL_CULL_FACE);  glCullFace(GL_BACK);           break;
			case Material::Cull::FrontAndBack: glEnable(GL_CULL_FACE);  glCullFace(GL_FRONT_AND_BACK); break;
			}

			PreviousCulling = Culling;
		}
	}

	void RenderState::SetBlending(bool Blending)
	{
		if (Blending != PreviousBlending)
		{
			if (Blending)
			{
				glEnable(GL_BLEND);
			} else
			{
				glDisable(GL_BLEND);
			}

			PreviousBlending = Blending;
		}
	}

	void RenderState::SetDepthTesting(Material::DepthTest DepthTesting)
	{
		if (DepthTesting != PreviousDepthTesting)
		{
			switch (DepthTesting)
			{
			case Material::DepthTest::Less:     glDepthFunc(GL_LESS);     break;
			case Material::DepthTest::Greater:  glDepthFunc(GL_GREATER);  break;
			case Material::DepthTest::LEqual:   glDepthFunc(GL_LEQUAL);   break;
			case Material::DepthTest::GEqual:   glDepthFunc(GL_GEQUAL);   break;
			case Material::DepthTest::Equal:    glDepthFunc(GL_EQUAL);    break;
			case Material::DepthTest::NotEqual: glDepthFunc(GL_NOTEQUAL); break;
			case Material::DepthTest::Never:    glDepthFunc(GL_NEVER);    break;
			case Material::DepthTest::Always:   glDepthFunc(GL_ALWAYS);   break;
			}

			PreviousDepthTesting = DepthTesting;
		}
	}

	void RenderState::SetDepthWriting(bool DepthWriting)
	{
		if (DepthWriting != PreviousDepthWriting)
		{
			glDepthMask(DepthWriting ? GL_TRUE : GL_FALSE);
			PreviousDepthWriting = DepthWriting;
		}
	}

	void RenderState::SetMainCamera(const Camera& InMainCamera)
	{
		MainCamera = InMainCamera;
	}

	void RenderState::SetMaterial(const Material& InMaterial, const Matrix& ModelMatrix, Skybox* Sky)
	{
		PreviousMaterial = CurrentMaterial;
		CurrentMaterial = InMaterial;

		bool MaterialChanged = CurrentMaterial != PreviousMaterial;

		#define CheckShader() (CurrentShader != PreviousShader)
		#define CheckParameter(x) (CurrentMaterial.x != PreviousMaterial.x) || CheckShader()
		#define CheckHasParameter(x) MaterialChanged || CheckParameter(x)

		if (CurrentShader != nullptr)
		{
			Texture* Textures[11] =     { CurrentMaterial .AlbedoMap, CurrentMaterial .NormalMap, CurrentMaterial .RoughnessMap, CurrentMaterial .MetallicMap, CurrentMaterial .OcclusionMap, CurrentMaterial .EmissionMap, CurrentMaterial .DetailAlbedoMap, CurrentMaterial .DetailNormalMap, Sky->GetIrradianceMap(), Sky->GetPrefilterMap(), gDevice->GetDefaultTextures()->IntegrationLUT };
			Texture* LastTextures[11] = { PreviousMaterial.AlbedoMap, PreviousMaterial.NormalMap, PreviousMaterial.RoughnessMap, PreviousMaterial.MetallicMap, PreviousMaterial.OcclusionMap, PreviousMaterial.EmissionMap, PreviousMaterial.DetailAlbedoMap, PreviousMaterial.DetailNormalMap, nullptr, nullptr, nullptr };

			static constexpr const char* Names[11] =
			{ "AlbedoMap",
			  "NormalMap",
			  "RoughnessMap",
			  "MetallicMap",
			  "OcclusionMap",
			  "EmissionMap",
			  "DetailAlbedoMap",
			  "DetailNormalMap",
			  "IrradianceMap",
			  "EnvironmentMap",
			  "IntegrationMap" };

			ShaderProgramOpenGL* Shader = (ShaderProgramOpenGL*)CurrentMaterial.GetShader();
			MaterialRenderData* RenderData = (MaterialRenderData*)Shader->RenderData;

			if (RenderData == nullptr)
			{
				MaterialRenderData* NewRenderData = new MaterialRenderData();

				for (int i = 0; i < 11; i++)
				{
					NewRenderData->TexturesIDs[i] = Shader->GetFastUniform(Names[i]);
				}

				NewRenderData->Model          = Shader->GetFastUniform("uModel");
				NewRenderData->ViewProjection = Shader->GetFastUniform("uViewProjection");

				NewRenderData->HasAlbedoMap       = Shader->GetFastUniform("HasAlbedoMap");
				NewRenderData->HasNormalMap       = Shader->GetFastUniform("HasNormalMap");
				NewRenderData->HasRoughnessMap    = Shader->GetFastUniform("HasRoughnessMap");
				NewRenderData->HasMetallicMap     = Shader->GetFastUniform("HasMetallicMap");
				NewRenderData->HasOcclusionMap    = Shader->GetFastUniform("HasOcclusionMap");
				NewRenderData->HasDetailAlbedoMap = Shader->GetFastUniform("HasDetailAlbedoMap");
				NewRenderData->HasDetailNormalMap = Shader->GetFastUniform("HasDetailNormalMap");

				NewRenderData->Tiling           = Shader->GetFastUniform("Tiling");
				NewRenderData->DetailTiling     = Shader->GetFastUniform("DetailTiling");
				NewRenderData->Albedo           = Shader->GetFastUniform("Albedo");
				NewRenderData->Roughness        = Shader->GetFastUniform("Roughness");
				NewRenderData->Metallic         = Shader->GetFastUniform("Metallic");
				NewRenderData->EmissionStrength = Shader->GetFastUniform("EmissionStrength");
				NewRenderData->Transparent      = Shader->GetFastUniform("Transparent");

				NewRenderData->CameraPosition = Shader->GetFastUniform("uCameraPosition");
				NewRenderData->Lighting       = Shader->GetFastUniform("uLighting");

				Shader->RenderData = NewRenderData;
				RenderData = NewRenderData;
			}

			for (int32 i = 0; i < 11; i++)
			{
				if ((Textures[i] != LastTextures[i] || CheckShader()) && Textures[i] != nullptr)
				{
					Shader->SetUniform(RenderData->TexturesIDs[i], (TextureOpenGL*)Textures[i], i);
				}
			}

			                   Shader->SetUniform(RenderData->Model, false, ModelMatrix);
			if (CheckShader()) Shader->SetUniform(RenderData->ViewProjection, false, MainCamera.GetViewProjection());

			if (CheckHasParameter(AlbedoMap))       Shader->SetUniform(RenderData->HasAlbedoMap,       CurrentMaterial.AlbedoMap != nullptr);
			if (CheckHasParameter(NormalMap))       Shader->SetUniform(RenderData->HasNormalMap,       CurrentMaterial.NormalMap != nullptr);
			if (CheckHasParameter(RoughnessMap))    Shader->SetUniform(RenderData->HasRoughnessMap,    CurrentMaterial.RoughnessMap != nullptr);
			if (CheckHasParameter(MetallicMap))     Shader->SetUniform(RenderData->HasMetallicMap,     CurrentMaterial.MetallicMap != nullptr);
			if (CheckHasParameter(OcclusionMap))    Shader->SetUniform(RenderData->HasOcclusionMap,    CurrentMaterial.OcclusionMap != nullptr);
			if (CheckHasParameter(DetailAlbedoMap)) Shader->SetUniform(RenderData->HasDetailAlbedoMap, CurrentMaterial.DetailAlbedoMap != nullptr);
			if (CheckHasParameter(DetailNormalMap)) Shader->SetUniform(RenderData->HasDetailNormalMap, CurrentMaterial.DetailNormalMap != nullptr);

			if (CheckParameter(Tiling))           Shader->SetUniform(RenderData->Tiling,           CurrentMaterial.Tiling);
			if (CheckParameter(DetailTiling))     Shader->SetUniform(RenderData->DetailTiling,     CurrentMaterial.DetailTiling);
			if (CheckParameter(Albedo))           Shader->SetUniform(RenderData->Albedo,           CurrentMaterial.Albedo);
			if (CheckParameter(Roughness))        Shader->SetUniform(RenderData->Roughness,        CurrentMaterial.Roughness);
			if (CheckParameter(Metallic))         Shader->SetUniform(RenderData->Metallic,         CurrentMaterial.Metallic);
			if (CheckParameter(EmissionStrength)) Shader->SetUniform(RenderData->EmissionStrength, CurrentMaterial.EmissionStrength);
			if (CheckParameter(Transparent))      Shader->SetUniform(RenderData->Transparent,      CurrentMaterial.Transparent);
			if (CheckShader())                    Shader->SetUniform(RenderData->CameraPosition,   MainCamera.Pos);
		}
	}

	void RenderState::SetLights(const std::vector<Light*>& InLights, const int32 LightIndices[4])
	{
		static constexpr int LightsCount = 4;
		static float Lights[13 * LightsCount];

		static Vector3 Color;

		if (CurrentShader != nullptr)
		{
			uint32 Counter = 0;

			for (uint32 i = 0; i < LightsCount; i++)
			{
				if (LightIndices[i] >= 0)
				{
					auto L = InLights[LightIndices[i]];

					uint32 Offset = Counter * 13;

					Color = L->Color * L->Energy;

					memcpy(Lights + Offset + 0, &Color,    sizeof(Color));
					memcpy(Lights + Offset + 3, &L->Pos,   sizeof(L->Pos));
					memcpy(Lights + Offset + 6, &L->Dir,   sizeof(L->Dir));

					Lights[Offset + 9] = (float)L->Type;
					Lights[Offset + 10] = L->Range;
					Lights[Offset + 11] = Math::Radians(L->InnerCutoff);
					Lights[Offset + 12] = Math::Radians(L->OuterCutoff);

					Counter++;
				}
			}

			for (; Counter < LightsCount; Counter++)
			{
				uint32 Offset = Counter * 13;
				Lights[Offset + 9] = -1;
			}

			ShaderProgramOpenGL* ShaderOGL = (ShaderProgramOpenGL*)CurrentShader;
			MaterialRenderData* RenderData = (MaterialRenderData*)ShaderOGL->RenderData;

			ShaderOGL->SetUniform(RenderData->Lighting, sizeof(Lights) / sizeof(float), Lights);
		}
	}

	void RenderState::SetShaderProgram(ShaderProgram* InShaderProgram)
	{
		PreviousShader = CurrentShader;
		CurrentShader = InShaderProgram;
		
		if (CurrentShader != PreviousShader && CurrentShader != nullptr)
		{
			if (!CurrentShader->IsError())
			{
				if (!CurrentShader->IsCompiled())
				{
					CurrentShader->Compile();
				}
			}

			((ShaderProgramOpenGL*)CurrentShader)->Bind();
		}
	}

	void RenderState::SetMesh(Mesh* InMesh)
	{
		PreviousMesh = CurrentMesh;
		CurrentMesh = InMesh;

		if (CurrentMesh != PreviousMesh && CurrentMesh != nullptr)
		{
			CurrentMesh->Bind();
		}
	}

}


