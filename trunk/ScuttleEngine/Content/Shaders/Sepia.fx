///Sepia Sprite Shader
///Authors: Matt Sutherlin

float4x4 WorldViewProj : WorldViewProjection;
///float4 color = {1,1,1,1};
texture texture0;

sampler Sampler0
{
    Texture = texture0;
    MipFilter = Linear;
    MinFilter = Linear;
    MagFilter = Linear;
    AddressU = Clamp;
    AddressV = Clamp;
};


struct VS_INPUT
{
    float3 position	: POSITION;
	float2 tex0 : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 screenposition : POSITION;
	float2 tex0  : TEXCOORD0;
};

VS_OUTPUT VertexShader0( VS_INPUT IN )
{
	VS_OUTPUT OUT;
	OUT.screenposition = mul( float4(IN.position, 1) , WorldViewProj);
	OUT.tex0 = IN.tex0;
	return OUT;
}

float4 PixelShader0( VS_OUTPUT IN ) : COLOR
{	
	float3 lumConst = float3(0.30, 0.59, 0.11);
	float3 light = float3(1, 0.9, 0.5);
	float3 dark = float3(0.2, 0.05, 0);
	
	float3 color = float3(1, 1, 1);
	
	float luminance = dot(color, lumConst);
	float3 sepia = lerp(dark, light, luminance);
	float3 halfColor = lerp(color, luminance, 0.5);
	float3 final = lerp(halfColor, sepia, 0.5);
	
	float4 colorFinal = float4(final, 1);
   
	return tex2D( Sampler0 , IN.tex0 ) * colorFinal;	
}

technique Technique0
{
	pass Pass0
	{
		Lighting = FALSE;
		AlphaBlendEnable = TRUE;
		VertexShader = compile vs_2_0 VertexShader0();
		PixelShader  = compile ps_2_0 PixelShader0();
	}
}