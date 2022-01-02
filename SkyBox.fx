//TextureCube skyMap : register(t0);
Texture2D skyMap : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

	float3 CameraPosition;
	float FogEnabled;

	float FogStart;
	float FogRange;

	float4 FogColour;
}

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 PosW : POSITION;
};

//--------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------

float4 CalculateFog(float4 currentColour, float distanceToEye)
{
	float4 resultColour = currentColour;

	if (FogEnabled == true)
	{
		float fogFactor = saturate((distanceToEye - FogStart) / FogRange);

		resultColour = lerp(resultColour, FogColour, fogFactor);
	}

	return resultColour;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float3 Pos : POSITION, float2 Tex : TEXCOORD0)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
 
	output.PosH = mul(float4(Pos, 1.0f), World);

	output.PosW = output.PosH.xyz;

	output.PosH = mul(output.PosH, View);
	output.PosH = mul(output.PosH, Projection);

    output.Tex = Tex;


	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 textureColour = skyMap.Sample(samLinear, input.Tex);

	float distanceToEye = length(input.PosW - CameraPosition);

	float4 finalColour;
	finalColour.rgb = CalculateFog(textureColour, distanceToEye).rgb;
	finalColour.a = textureColour.a;

	return finalColour;
}