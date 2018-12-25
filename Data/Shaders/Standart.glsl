#ifdef VertexShader

	//@Attribute aPos  0
	//@Attribute aUV   1
	//@Attribute aNorm 2
	//@Attribute aTang 3

	in vec3 aPos;
	in vec2 aUV;
	in vec3 aNorm;
	in vec3 aTang;

	out vec3 varPos;
	out vec2 varUV;
	out vec3 varNormal;
	out mat3 varTBN;

	//@Uniform uModel
	//@Uniform uViewProjection
	//@Uniform uView
	//@Uniform uProjection

	uniform mat4 uModel;
	uniform mat4 uViewProjection;
	uniform mat4 uView;
	uniform mat4 uProjection;

	void main()
	{
		vec3 normal = normalize(vec3(vec4(aNorm, 0.0) * uModel));
		vec3 tangent = normalize(vec3(vec4(aTang, 0.0) * uModel));
		vec3 binormal = cross(normal, tangent);

		varPos = vec3(vec4(aPos, 1.0) * uModel);
		varUV = aUV;
		varNormal = normal;
		varTBN = transpose(mat3(tangent, binormal, normal));

		Position = uViewProjection * vec4(varPos, 1);
	}

#endif

#ifdef FragmentShader

	#define PI 3.141592653
	#define LAMBERTIAN 0.318310 // 1.0 / PI
	#define COLOR_EXP  0.454545 //1.0 / 2.2
	#define LIGHT_NUM 4

	in vec3 varPos;
	in vec2 varUV;
	in vec3 varNormal;
	in mat3 varTBN;

	struct Material
	{
		sampler2D DiffuseMap;
		sampler2D NormalMap;
		sampler2D RoughnessMap;
		sampler2D MetallicMap;
		sampler2D OcclusionMap;
		sampler2D EmissionMap;
		sampler2D DetailDiffuseMap;
		sampler2D DetailNormalMap;
		samplerCube ReflectionMap;

		vec2 Tiling;
		vec2 DetailTiling;

		vec4 Color;
		float Roughness;
		float Metallic;
		float EmissionStrength;

		bool Transparent;
	};

	struct Camera
	{
		vec3 Position;
	};

	//@Uniform uMaterial.DiffuseMap
	//@Uniform uMaterial.NormalMap
	//@Uniform uMaterial.RoughnessMap
	//@Uniform uMaterial.MetallicMap
	//@Uniform uMaterial.OcclusionMap
	//@Uniform uMaterial.EmissionMap
	//@Uniform uMaterial.DetailDiffuseMap
	//@Uniform uMaterial.DetailNormalMap
	//@Uniform uMaterial.ReflectionMap
	//@Uniform uMaterial.Tiling
	//@Uniform uMaterial.DetailTiling
	//@Uniform uMaterial.Color
	//@Uniform uMaterial.Roughness
	//@Uniform uMaterial.Metallic
	//@Uniform uMaterial.EmissionStrength
	//@Uniform uMaterial.Transparent
	//@Uniform uLighting
	//@Uniform uCamera.Position

	uniform Material uMaterial;
	uniform float uLighting[15 * LIGHT_NUM];
	uniform Camera uCamera;

	vec4 DiffuseMap;
	vec4 NormalMap;
	vec3 RoughnessMap;
	vec3 MetallicMap;
	vec4 DetailDiffuseMap;
	vec4 DetailNormalMap;

	vec2 TiledUV;

	vec3 Normal;
	float Roughness;
	float Metallic;

	void Init(void);
	void Final(void);

	void main(void)
	{
		Init();
		Final();
	}

	vec3 NormalBlend(in vec4 n1, in vec4 n2)
	{
		//UDN
		vec3 c = vec3(2, 1, 0);
		vec3 r;
		r = n2.xyz * c.yyz + n1.xyz;
		r = r * c.xxx - c.xxy;
		return normalize(r);
	}

	void Init(void)
	{
		TiledUV = varUV * uMaterial.Tiling;

		DiffuseMap = texture(uMaterial.DiffuseMap, TiledUV);
		NormalMap = texture(uMaterial.NormalMap, TiledUV);
		RoughnessMap = texture(uMaterial.RoughnessMap, TiledUV).rgb;
		MetallicMap = texture(uMaterial.RoughnessMap, TiledUV).rgb;
		DetailDiffuseMap = texture(uMaterial.DetailDiffuseMap, varUV * uMaterial.DetailTiling);
		DetailNormalMap = texture(uMaterial.DetailNormalMap, varUV * uMaterial.DetailTiling);

		Normal = varNormal;
		Roughness = uMaterial.Roughness;
		Metallic = uMaterial.Metallic;

		if (textureSize(uMaterial.DiffuseMap, 0).x > 1 && textureSize(uMaterial.DetailDiffuseMap, 0).x > 1)
		{
			DiffuseMap = vec4(DiffuseMap.rgb * DetailDiffuseMap.rgb * 1.8f, DiffuseMap.a);
		}
		else if (textureSize(uMaterial.DetailDiffuseMap, 0).x > 1)
		{
			DiffuseMap = DetailDiffuseMap;
		}

		if (textureSize(uMaterial.NormalMap, 0).x > 1 && textureSize(uMaterial.DetailNormalMap, 0).x > 1)
		{
			Normal = NormalBlend(NormalMap, DetailNormalMap) * varTBN;
		}
		else if (textureSize(uMaterial.DetailNormalMap, 0).x > 1)
		{
			Normal = normalize(DetailNormalMap.rgb * 2.0 - 1.0) * varTBN;
		}

		if (textureSize(uMaterial.RoughnessMap, 0).x > 1)
		{
			Roughness = texture(uMaterial.RoughnessMap, TiledUV).r;
		}

		if (textureSize(uMaterial.MetallicMap, 0).x > 1)
		{
			Metallic = texture(uMaterial.MetallicMap, TiledUV).r;
		}
	}

	vec3 LambertDiffuseBRDF(in vec3 color)
	{
		//return color / PI;
		return color * LAMBERTIAN;
	}

	float DistributionGGX(vec3 N, vec3 H, float roughness)
	{
		float a      = roughness * roughness;
		float a2     = a * a;
		float NdotH  = max(dot(N, H), 0.0);
		float NdotH2 = NdotH * NdotH;

		float num   = a2;
		float denom = (NdotH2 * (a2 - 1.0) + 1.0);
		denom = PI * denom * denom;

		return num / max(denom, 0.001f);
	}

	float GeometryGGX(float a, float ndotl, float ndotv)
	{
		float a2 = max(a * a, 0.001f);

		float gv = ndotl * sqrt(a2 + (1.0f - a2) * ndotv * ndotv);
		float gl = ndotv * sqrt(a2 + (1.0f - a2) * ndotl * ndotl);

		return 0.5f / max(gv + gl, 0.001f);
	}

	vec3 Fresnel(in float cosTheta, in vec3 F0)
	{
		return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
	}

	vec3 CookTorranceSpecularBRDF(in vec3 N, in vec3 L, in vec3 V, in vec3 H, out vec3 F)
	{
		vec3 F0 = vec3(0.04); 
		F0 = mix(F0, vec3(1), Metallic);

		float NdotV = max(0, dot(N, V));
		float NdotL = max(0, dot(N, L));

		float D = DistributionGGX(N, H, Roughness);
		      F = Fresnel(NdotV, F0);
		float G = GeometryGGX(Roughness * Roughness, NdotL, NdotV);

		return D * F * G * LAMBERTIAN * NdotL;
	}

	vec3 LightCalc(int id)
	{
		int Offset = id * 15;

		vec3 LightColor = vec3(uLighting[Offset + 0], uLighting[Offset + 1], uLighting[Offset + 2]);
		vec3 LightPos = vec3(uLighting[Offset + 3], uLighting[Offset + 4], uLighting[Offset + 5]);
		vec3 LightDir = vec3(uLighting[Offset + 6], uLighting[Offset + 7], uLighting[Offset + 8]);
		float LightType = uLighting[Offset + 9];
		float LightConstant = uLighting[Offset + 10];
		float LightLinear = uLighting[Offset + 11];
		float LightQuadratic = uLighting[Offset + 12];
		//float LightInnerCutoff = uLighting[Offset + 13];
		//float LightOuterCutoff = uLighting[Offset + 14];

		if (LightColor == vec3(0)) return vec3(0);

		float AO = 1.0; if (textureSize(uMaterial.OcclusionMap, 0).x > 1) AO = texture(uMaterial.OcclusionMap, varUV * uMaterial.Tiling).r;
		float Distance = length(LightPos - varPos);
		float Attenuation = 1.0; if (int(LightType) != 0) Attenuation = 1.0 / (1.0 + LightLinear * Distance + LightQuadratic * Distance * Distance);

		vec3 N, L, V, H, F;
		N = normalize(Normal);
		L = normalize(LightPos - varPos); if (int(LightType) == 0) L = normalize(-LightDir);
		V = normalize(uCamera.Position - varPos);
		H = normalize(V + L);

		float NdotL = max(0, dot(N, L));

		vec3 DiffuseBRDF = LambertDiffuseBRDF(uMaterial.Color.rgb) * AO;
		vec3 SpecularBRDF = CookTorranceSpecularBRDF(N, L, V, H, F);

		float Factor = 1.0 - Metallic;

		return (Factor * DiffuseBRDF + SpecularBRDF) * NdotL * LightColor * Attenuation;
	}

	vec3 Lights()
	{
		vec3 BRDF = vec3(0);

		BRDF += LightCalc(0);
		BRDF += LightCalc(1);
		BRDF += LightCalc(2);
		BRDF += LightCalc(3);

		BRDF = BRDF / (BRDF + vec3(1.0));
		BRDF = pow(BRDF, vec3(COLOR_EXP));

		return BRDF;
	}

	vec2 EncodeNormal(in vec3 n)
	{
		return vec2(n.xy / sqrt(8.0 * n.z + 8.0) + 0.5);
	}

	void Final(void)
	{
		vec4 Color = vec4(Lights(), uMaterial.Color.a);

		if (textureSize(uMaterial.DiffuseMap, 0).x > 1)
		{
			Color *= DiffuseMap;
		}

		Color += vec4(texture(uMaterial.EmissionMap, varUV * uMaterial.Tiling).rgb * uMaterial.EmissionStrength, 0);

		FragData[0] = Color;
		FragData[1] = vec4(Normal, 1);

		if (!uMaterial.Transparent && Color.a < 1.0) discard;
		if (uMaterial.Transparent && Color.a == 1.0) discard;
	}

#endif












