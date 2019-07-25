#pragma once

namespace Columbus
{

	const char* gScreenSpaceVertexShader = 
	R"(
	#version 130

	const vec2 Coord[6] = vec2[](
		vec2(-1, +1),
		vec2(+1, +1),
		vec2(+1, -1),
		vec2(-1, -1),
		vec2(-1, +1),
		vec2(+1, -1)
	);

	const vec2 TC[6] = vec2[](
		vec2(0, 1),
		vec2(1, 1),
		vec2(1, 0),
		vec2(0, 0),
		vec2(0, 1),
		vec2(1, 0)
	);

	out vec2 Texcoord;

	void main(void)
	{
		gl_Position = vec4(Coord[gl_VertexID], 0, 1);
		Texcoord = TC[gl_VertexID];
	}
	)";

	const char* gScreenSpaceFragmentShader =
	R"(
	#version 130
	out vec4 FragColor;
	in vec2 Texcoord;

	uniform sampler2D BaseTexture;

	void main(void)
	{
		FragColor = texture(BaseTexture, Texcoord);
	}
	)";

	const char* gAutoExposureFragmentShader =
	R"(
	#version 130
	out vec4 FragColor;
	in vec2 Texcoord;

	uniform sampler2D BaseTexture;
	uniform sampler2D Previous;
	uniform float Min;
	uniform float Max;
	uniform float SpeedUp;
	uniform float SpeedDown;
	uniform float DeltaTime;

	float luma(vec3 color)
	{
		return dot(color, vec3(0.2125, 0.7154, 0.0721));
	}

	void main(void)
	{
		float prev_exp = texture(Previous, vec2(0.5)).r;
		float curr_luma = luma(texture(BaseTexture, vec2(0.5)).rgb);
		float adapted = 0.5 / clamp(curr_luma, Min, Max);
		float exp = prev_exp;

		if (adapted >= prev_exp)
			exp += (adapted - prev_exp) * DeltaTime * SpeedUp;
		else
			exp += (adapted - prev_exp) * DeltaTime * SpeedDown;

		FragColor = vec4(exp);
	}
	)";

	const char* gTonemapFragmentShader = 
	R"(
	#version 130
	out vec4 FragColor;
	in vec2 Texcoord;

	uniform sampler2D BaseTexture;
	uniform sampler2D AutoExposureTexture;
	uniform float Exposure;
	uniform float Gamma;
	uniform int Type;
	uniform int AutoExposureEnable;

	vec3 TonemapSimple(vec3 HDR)
	{
		HDR = HDR / (1.0 + HDR);
		return pow(HDR, vec3(1.0 / Gamma));
	}

	vec3 TonemapFilmic(vec3 HDR)
	{
		HDR = max(vec3(0.0), HDR - 0.004);
		return (HDR * (6.2 * HDR + 0.5)) / (HDR * (6.2 * HDR + 1.7) + 0.06);
	}

	vec3 TonemapACES(vec3 HDR)
	{
		const float a = 2.51f;
		const float b = 0.03f;
		const float c = 2.43f;
		const float d = 0.59f;
		const float e = 0.14f;

		HDR = (HDR * (a * HDR + b))  /
		      (HDR * (c * HDR + d) + e);
		HDR = pow(HDR, 1.0 / vec3(Gamma));
		return HDR;
	}

	vec3 TonemapRomBinDaHouse(vec3 HDR)
	{
		HDR = exp(-1.0 / (2.72 * HDR + 0.15));
		return pow(HDR, vec3(1.0 / Gamma));
	}

	vec3 TonemapUncharted(vec3 HDR)
	{
		float A = 0.15;
		float B = 0.50;
		float C = 0.10;
		float D = 0.20;
		float E = 0.02;
		float F = 0.30;
		float W = 11.2;

		HDR = ((HDR * (A * HDR + C * B) + D * E) / (HDR * (A * HDR + B) + D * F)) - E / F;
		float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
		HDR /= white;
		HDR = pow(HDR, vec3(1.0 / Gamma));
		return HDR;
	}

	void main(void)
	{
		float E = Exposure;

		if (AutoExposureEnable == 1)
		{
			E = texture(AutoExposureTexture, vec2(0.5)).r;
		}

		vec3 HDR = clamp(texture(BaseTexture, Texcoord).rgb * E, 0, 50000);
		vec3 Mapped = vec3(0.0);

		switch (Type)
		{
		case 0: Mapped = TonemapSimple(HDR); break;
		case 1: Mapped = TonemapFilmic(HDR); break;
		case 2: Mapped = TonemapACES(HDR); break;
		case 3: Mapped = TonemapRomBinDaHouse(HDR); break;
		case 4: Mapped = TonemapUncharted(HDR); break;
		}

		FragColor = vec4(Mapped, 1.0);
	}
	)";

	const char* gResolveMSAAFragmentShader =
	R"(
	#version 150

	out vec4 FragColor;

	uniform sampler2DMS BaseTexture;
	uniform int Samples;

	in vec2 Texcoord;

	vec3 Tonemap(vec3 HDR)
	{
		return HDR / (1.0 + HDR);
	}

	vec3 InverseTonemap(vec3 LDR)
	{
		return LDR / (1.0 - LDR);
	}

	void main(void)
	{
		vec3 Color = vec3(0);

		for (int i = 0; i < Samples; i++)
		{
			Color += Tonemap(texelFetch(BaseTexture, ivec2(gl_FragCoord.xy), i).rgb);
		}

		Color = InverseTonemap(Color / Samples);

		FragColor = vec4(Color, 1.0);
	}
	)";

	const char* gGaussBlurFragmentShader = 
	R"(
	#version 130

	out vec4 FragColor;

	uniform sampler2D BaseTexture;
	uniform bool Horizontal;
	uniform float Radius;

	in vec2 Texcoord;

	uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

	void main()
	{
		vec2 tex_offset = 1.0 / textureSize(BaseTexture, 0) * Radius;
		vec3 result = texture(BaseTexture, Texcoord).rgb * weight[0];

		if (Horizontal)
		{
			for (int i = 1; i < 5; ++i)
			{
				result += texture(BaseTexture, Texcoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
				result += texture(BaseTexture, Texcoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			}
		}
		else
		{
			for (int i = 1; i < 5; ++i)
			{
				result += texture(BaseTexture, Texcoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
				result += texture(BaseTexture, Texcoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			}
		}

		FragColor = vec4(result, 1.0);
	}
	)";

	const char* gBloomBrightFragmentShader =
	R"(
	#version 130

	out vec4 FragColor;

	uniform sampler2D BaseTexture;
	uniform float Treshold;

	in vec2 Texcoord;

	float luma(vec3 color)
	{
		return dot(color, vec3(0.2125, 0.7154, 0.0721));
	}

	void main(void)
	{
		vec4 color = texture(BaseTexture, Texcoord);
		FragColor = luma(color.rgb) > Treshold ? color : vec4(0, 0, 0, 1);
	}
	)";

	const char* gBloomFragmentShader = 
	R"(
	#version 130

	out vec4 FragColor;

	uniform sampler2D BaseTexture;
	uniform sampler2D Blur;
	uniform float Intensity;

	in vec2 Texcoord;

	void main()
	{
		vec3 hdrColor = texture(BaseTexture, Texcoord).rgb;
		vec3 bloomColor = texture(Blur, Texcoord).rgb * Intensity;
		hdrColor += bloomColor;
		FragColor = vec4(hdrColor, 1.0);
	}
	)";

	const char* gVignetteFragmentShader =
	R"(
	#version 130

	in vec2 Texcoord;
	out vec4 FragColor;

	uniform vec3 Color;
	uniform vec2 Center;
	uniform float Intensity;
	uniform float Smoothness;
	uniform float Radius;

	void main(void)
	{
		float Diff = Radius - distance(Texcoord, Center);
		float Value = smoothstep(-Smoothness, Smoothness, Diff);
		FragColor = vec4(Color, (1.0 - clamp(Value, 0.0, 1.0)) * Intensity);
	}
	)";

	const char* gFXAAFragmentShader = 
	R"(
	#version 130

	out vec4 FragColor;

	uniform sampler2D BaseTexture;
	uniform vec2 Resolution;

	#define FXAA_SPAN_MAX 8.0
	#define FXAA_REDUCE_MUL   (1.0/FXAA_SPAN_MAX)
	#define FXAA_REDUCE_MIN   (1.0/128.0)
	#define FXAA_SUBPIX_SHIFT (1.0/4.0)

	vec4 fxaa(sampler2D tex, vec2 fragCoord, vec2 resolution,
		vec2 v_rgbNW, vec2 v_rgbNE, 
		vec2 v_rgbSW, vec2 v_rgbSE, 
		vec2 v_rgbM)
	{
		vec4 color;
		mediump vec2 inverseVP = vec2(1.0 / resolution.x, 1.0 / resolution.y);
		vec3 rgbNW = texture(tex, v_rgbNW).xyz;
		vec3 rgbNE = texture(tex, v_rgbNE).xyz;
		vec3 rgbSW = texture(tex, v_rgbSW).xyz;
		vec3 rgbSE = texture(tex, v_rgbSE).xyz;
		vec4 texColor = texture(tex, v_rgbM);
		vec3 rgbM  = texColor.xyz;
		vec3 luma = vec3(0.299, 0.587, 0.114);
		float lumaNW = dot(rgbNW, luma);
		float lumaNE = dot(rgbNE, luma);
		float lumaSW = dot(rgbSW, luma);
		float lumaSE = dot(rgbSE, luma);
		float lumaM  = dot(rgbM,  luma);
		float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
		float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

		mediump vec2 dir;
		dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
		dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

		float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *
				  (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

		float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
		dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
		max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
		dir * rcpDirMin)) * inverseVP;

		vec3 rgbA = 0.5 * (
		texture(tex, fragCoord * inverseVP + dir * (1.0 / 3.0 - 0.5)).xyz +
		texture(tex, fragCoord * inverseVP + dir * (2.0 / 3.0 - 0.5)).xyz);
		vec3 rgbB = rgbA * 0.5 + 0.25 * (
		texture(tex, fragCoord * inverseVP + dir * -0.5).xyz +
		texture(tex, fragCoord * inverseVP + dir * 0.5).xyz);

		float lumaB = dot(rgbB, luma);
		if ((lumaB < lumaMin) || (lumaB > lumaMax))
		color = vec4(rgbA, texColor.a);
		else
		color = vec4(rgbB, texColor.a);
		return color;
	}

	void texcoords(vec2 fragCoord, vec2 resolution,
			out vec2 v_rgbNW, out vec2 v_rgbNE,
			out vec2 v_rgbSW, out vec2 v_rgbSE,
			out vec2 v_rgbM)
	{
		vec2 inverseVP = 1.0 / resolution.xy;
		v_rgbNW = (fragCoord + vec2(-1.0, -1.0)) * inverseVP;
		v_rgbNE = (fragCoord + vec2(1.0, -1.0)) * inverseVP;
		v_rgbSW = (fragCoord + vec2(-1.0, 1.0)) * inverseVP;
		v_rgbSE = (fragCoord + vec2(1.0, 1.0)) * inverseVP;
		v_rgbM = vec2(fragCoord * inverseVP);
	}

	void main(void)
	{
		vec2 v_rgbNW;
		vec2 v_rgbNE;
		vec2 v_rgbSW;
		vec2 v_rgbSE;
		vec2 v_rgbM;

		texcoords(gl_FragCoord.xy, Resolution, v_rgbNW, v_rgbNE, v_rgbSW, v_rgbSE, v_rgbM);
		FragColor =  fxaa(BaseTexture, gl_FragCoord.xy, Resolution, v_rgbNW, v_rgbNE, v_rgbSW, v_rgbSE, v_rgbM);
	}
	)";

	const char* gIconVertexShader =
	R"(
	#version 130

	const vec2 Coord[6] = vec2[](
		vec2(-1, +1),
		vec2(+1, +1),
		vec2(+1, -1),
		vec2(-1, -1),
		vec2(-1, +1),
		vec2(+1, -1)
	);

	const vec2 TC[6] = vec2[](
		vec2(0, 1),
		vec2(1, 1),
		vec2(1, 0),
		vec2(0, 0),
		vec2(0, 1),
		vec2(1, 0)
	);

	uniform vec2 Pos;
	uniform vec2 Size;

	out vec2 Texcoord;

	void main(void)
	{
		gl_Position = vec4(Coord[gl_VertexID] * Size + Pos, 0, 1);
		Texcoord.x = TC[gl_VertexID].x;
		Texcoord.y = -TC[gl_VertexID].y;
	}
	)";

	const char* gIconFragmentShader =
	R"(
	#version 130

	out vec4 FragColor;

	uniform sampler2D Texture;

	in vec2 Texcoord;

	void main(void)
	{
		FragColor = texture(Texture, Texcoord);
	}
	)";

	const char* gEditorToolsVertexShader =
	R"(
	#version 130
	in vec3 Position;

	uniform mat4 ViewProjection;

	out vec3 varPos;

	void main()
	{
		gl_Position = ViewProjection * vec4(Position, 1.0);
		varPos = Position;
	}
	)";

	const char* gEditorToolsFragmentShader =
	R"(
	#version 130

	out vec4 FragColor;

	uniform vec4 Color;
	uniform vec3 CameraPos;

	in vec3 varPos;

	void main()
	{
		vec3 view_ray = normalize(CameraPos - varPos);
		float alpha = Color.a * abs(dot(view_ray, vec3(0, 1, 0)));
		alpha *= 1.0 - smoothstep(0.0, 100.0, distance(varPos, CameraPos));
		FragColor = vec4(Color.rgb, alpha);
	}
	)";

	const char* gSkyboxVertexShader = 
	R"(
	#version 130
	in vec3 Pos;

	out vec3 Texcoord;

	uniform mat4 ViewProjection;

	void main(void)
	{
		gl_Position = ViewProjection * vec4(Pos, 1.0);
		Texcoord = Pos;
	}
	)";

	const char* gSkyboxFragmentShader = 
	R"(
	#version 130
	out vec4 FragColor;
	in vec3 Texcoord;

	uniform samplerCube Skybox;

	void main(void)
	{
		//FragColor = vec4(pow(texture(Skybox, Texcoord).rgb, vec3(1.5)), 1);
		FragColor = vec4(texture(Skybox, Texcoord).rgb, 1.0);

		gl_FragDepth = 0x7FFFFFFF;
	}
	)";

	const char* gSkyboxCubemapGenerationVertexShader =
	R"(
	#version 130
	in vec3 Position;
	out vec3 Pos;

	uniform mat4 Projection;
	uniform mat4 View;

	void main()
	{
		gl_Position = Projection * View * vec4(Position, 1);
		Pos = Position;
		Pos .y = -Pos.y;
	}
	)";

	const char* gSkyboxCubemapGenerationFragmentShader =
	R"(
	#version 130
	out vec4 FragColor;
	in vec3 Pos;

	uniform sampler2D BaseMap;

	const vec2 invAtan = vec2(0.1591, 0.3183);
	vec2 SampleSphericalMap(vec3 v)
	{
		vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
		uv *= invAtan;
		uv += 0.5;
		return uv;
	}

	void main()
	{		
		vec2 uv = SampleSphericalMap(normalize(Pos)); 
		vec3 color = texture(BaseMap, uv).rgb;

		FragColor = vec4(clamp(color, 0, 50000), 1.0);
	}
	)";

	const char* gIrradianceGenerationVertexShader = 
	R"(
	#version 130
	in vec3 Position;
	out vec3 Pos;

	uniform mat4 Projection;
	uniform mat4 View;

	void main()
	{
		gl_Position = Projection * View * vec4(Position, 1);
		Pos = Position;
	}
	)";

	const char* gIrradianceGenerationFragmentShader =
	R"(
	#version 130
	out vec4 FragColor;

	in vec3 Pos;

	uniform samplerCube EnvironmentMap;

	const float PI = 3.14159265359;

	void main()
	{
		vec3 Normal = normalize(Pos);  
		vec3 Irradiance = vec3(0.0);

		vec3 Up    = vec3(0.0, 1.0, 0.0);
		vec3 Right = cross(Up, Normal);
		Up         = cross(Normal, Right);

		float SampleDelta = 0.025;
		float NumSamples = 0.0;

		for(float Phi = 0.0; Phi < 2.0 * PI; Phi += SampleDelta)
		{
			for(float Theta = 0.0; Theta < 0.5 * PI; Theta += SampleDelta)
			{
				vec3 TangentSample = vec3(sin(Theta) * cos(Phi), sin(Theta) * sin(Phi), cos(Theta));
				vec3 SampleVec = TangentSample.x * Right + TangentSample.y * Up + TangentSample.z * Normal; 

				Irradiance += texture(EnvironmentMap, SampleVec).rgb * cos(Theta) * sin(Theta);
				NumSamples += 1.0;
			}
		}

		FragColor = vec4(PI * Irradiance * (1.0 / NumSamples), 1.0);
	}
	)";

	const char* gPrefilterGenerationVertexShader = 
	R"(
	#version 130
	in vec3 Position;
	out vec3 Pos;

	uniform mat4 Projection;
	uniform mat4 View;

	void main()
	{
		gl_Position = Projection * View * vec4(Position, 1);
		Pos = Position;
	}
	)";

	const char* gPrefilterGenerationFragmentShader = 
	R"(
	#version 130
	out vec4 FragColor;
	in vec3 Pos;

	uniform float Roughness;
	uniform samplerCube EnvironmentMap;

	const float PI = 3.14159265359;

	float RadicalInverse(uint n, uint base)
	{
		float InvBase = 1.0 / float(base);
		float Denom   = 1.0;
		float Result  = 0.0;

		for(uint i = 0u; i < 32u; ++i)
		{
			if(n > 0u)
			{
				Denom   = mod(float(n), 2.0);
				Result += Denom * InvBase;
				InvBase = InvBase / 2.0;
				n       = uint(float(n) / 2.0);
			}
		}

		return Result;
	}

	vec2 Hammersley(uint i, uint N)
	{
		return vec2(float(i) / float(N), RadicalInverse(i, 2u));
	}

	vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
	{
		float a = roughness * roughness;

		float Phi = 2.0 * PI * Xi.x;
		float CosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
		float SinTheta = sqrt(1.0 - CosTheta * CosTheta);

		vec3 H;
		H.x = cos(Phi) * SinTheta;
		H.y = sin(Phi) * SinTheta;
		H.z = CosTheta;

		vec3 Up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
		vec3 Tangent   = normalize(cross(Up, N));
		vec3 Bitangent = cross(N, Tangent);

		return normalize(Tangent * H.x + Bitangent * H.y + N * H.z);
	}

	void main()
	{		
		vec3 N = normalize(Pos);
		vec3 R = N;
		vec3 V = R;

		const uint SAMPLE_COUNT = 1024u;
		float TotalWeight = 0.0;   
		vec3 PrefilteredColor = vec3(0.0);

		for(uint i = 0u; i < SAMPLE_COUNT; ++i)
		{
			vec2 Xi = Hammersley(i, SAMPLE_COUNT);
			vec3 H  = ImportanceSampleGGX(Xi, N, Roughness);
			vec3 L  = normalize(2.0 * dot(V, H) * H - V);

			float NdotL = max(dot(N, L), 0.0);

			if(NdotL > 0.0)
			{
				PrefilteredColor += texture(EnvironmentMap, L).rgb * NdotL;
				TotalWeight      += NdotL;
			}
		}

		FragColor = vec4(PrefilteredColor / TotalWeight, 1.0);
	}
	)";

	const char* gIntegrationGenerationFragmentShader = 
	R"(
	#version 130
	
	out vec4 FragColor;
	in vec2 Texcoord;

	const float PI = 3.14159265359;

	float GeometrySchlickGGX(float NdotV, float roughness)
	{
		float a = roughness;
		float k = (a * a) / 2.0;

		float Nom   = NdotV;
		float Denom = NdotV * (1.0 - k) + k;

		return Nom / Denom;
	}

	float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
	{
		float NdotV = max(dot(N, V), 0.0);
		float NdotL = max(dot(N, L), 0.0);
		float ggx2 = GeometrySchlickGGX(NdotV, roughness);
		float ggx1 = GeometrySchlickGGX(NdotL, roughness);

		return ggx1 * ggx2;
	}

	vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
	{
		float a = roughness * roughness;

		float Phi = 2.0 * PI * Xi.x;
		float CosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
		float SinTheta = sqrt(1.0 - CosTheta * CosTheta);

		vec3 H;
		H.x = cos(Phi) * SinTheta;
		H.y = sin(Phi) * SinTheta;
		H.z = CosTheta;

		vec3 Up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
		vec3 Tangent   = normalize(cross(Up, N));
		vec3 Bitangent = cross(N, Tangent);

		return normalize(Tangent * H.x + Bitangent * H.y + N * H.z);
	}

	float RadicalInverse(uint n, uint base)
	{
		float InvBase = 1.0 / float(base);
		float Denom   = 1.0;
		float Result  = 0.0;

		for(uint i = 0u; i < 32u; ++i)
		{
			if(n > 0u)
			{
				Denom   = mod(float(n), 2.0);
				Result += Denom * InvBase;
				InvBase = InvBase / 2.0;
				n       = uint(float(n) / 2.0);
			}
		}

		return Result;
	}

	vec2 Hammersley(uint i, uint N)
	{
		return vec2(float(i) / float(N), RadicalInverse(i, 2u));
	}

	vec2 IntegrateBRDF(float NdotV, float roughness)
	{
		vec3 V;
		V.x = sqrt(1.0 - NdotV * NdotV);
		V.y = 0.0;
		V.z = NdotV;

		float A = 0.0;
		float B = 0.0;

		vec3 N = vec3(0.0, 0.0, 1.0);

		const uint SAMPLE_COUNT = 1024u;

		for(uint i = 0u; i < SAMPLE_COUNT; ++i)
		{
			vec2 Xi = Hammersley(i, SAMPLE_COUNT);
			vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
			vec3 L  = normalize(2.0 * dot(V, H) * H - V);

			float NdotL = max(L.z, 0.0);
			float NdotH = max(H.z, 0.0);
			float VdotH = max(dot(V, H), 0.0);

			if(NdotL > 0.0)
			{
				float G = GeometrySmith(N, V, L, roughness);
				float G_Vis = (G * VdotH) / (NdotH * NdotV);
				float Fc = pow(1.0 - VdotH, 5.0);

				A += (1.0 - Fc) * G_Vis;
				B += Fc * G_Vis;
			}
		}

		return vec2(A, B) / float(SAMPLE_COUNT);
	}

	void main() 
	{
		vec2 IntegratedBRDF = IntegrateBRDF(Texcoord.x, Texcoord.y);
		FragColor = vec4(IntegratedBRDF, 1, 1);
	}
	)";

}


