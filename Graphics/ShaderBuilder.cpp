#include <Graphics/ShaderBuilder.h>

namespace Columbus
{

	const std::string gVertexShaderHeader =
	"#version 130\n"
	"#define Position gl_Position\n"
	"#define VertexShader\n";
	
	const std::string gFragmentShaderHeader =
	"#version 130\n"
	"#define FragData gl_FragData\n"
	"#define FragmentShader\n"
	"out vec4 FragColor;\n";
	
	bool ShaderBuilder::Build(const std::string& InShader, ShaderType Type)
	{
		ShaderSource.clear();

		switch (Type)
		{
		case ShaderType::Vertex:
			ShaderSource = gVertexShaderHeader + "\n" + InShader + "\n";
			return true;
			break;

		case ShaderType::Fragment:
			ShaderSource = gFragmentShaderHeader + "\n" + InShader + "\n";
			return true;
			break;
		};

		return false;
	}

}


