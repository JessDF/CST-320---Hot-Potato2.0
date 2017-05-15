//--------------------------------------------------------------------------------------
// File: lecture 8.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse: register(t0);
Texture2D txDepth: register(t1);
SamplerState samLinear: register(s0);


cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    matrix LightView;
    float4 info;
    float4 CameraPos;
};


//--------------------------------------------------------------------------------------
struct VS_INPUT {
    float4 Pos: POSITION;
    float2 Tex: TEXCOORD0;
};

//struct PS_INPUT {
//  float4 Pos: SV_POSITION;
//  float2 Tex: TEXCOORD0;
//};


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float4 Norm : NORMAL0;
    float4 OPos : POSITION;
    float4 WorldPos : POSITION1;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
//float4 PS( PS_INPUT input): SV_Target {
//   float4 color = txDiffuse.Sample( samLinear, input.Tex );
//  float depth = saturate(input.Pos.z / input.Pos.w);
//  //depth = pow(depth,0.97);
//  //color = depth;// (depth*0.9 + 0.02);
//  return color;
//}

float4 PS(PS_INPUT input) : SV_Target
{
    //calculating shadows:

    float4 pos = input.Pos;
    float4 Opos = input.OPos;
    float pixeldepth = 0;



    float4 wpos = input.WorldPos;
    //input.OPos.w = 1;

    //wpos = mul(wpos, World);
    wpos = mul(wpos, LightView);
    wpos = mul(wpos, Projection);

    float shadowlight = 0.9;//1 .. no shadow


    pixeldepth = wpos.z / wpos.w;
    pixeldepth = pow(pixeldepth, 0.97);

    //pixeldepth = wpos.z / wpos.w;
    //pixeldepth = pos.z / pos.w;
    //return float4(pixeldepth, pixeldepth, pixeldepth, 1);
    float2 texdpos = wpos.xy / wpos.w;
    texdpos.x = texdpos.x*0.5 + 0.5;
    texdpos.y = texdpos.y* (-0.5) + 0.5;

    float4 depth = txDepth.SampleLevel(samLinear, texdpos,0);
    float d = depth.x / depth.y;
    if (pixeldepth > (d + 0.000001))
        shadowlight = 0;

    float4 texture_color = txDiffuse.Sample(samLinear, input.Tex);
    float4 color = texture_color;

    /*float3 LightPosition = float3(30, 100, 0);*/
    float3 LightPosition = float3(0, 0, -10);
    float3 lightDir = normalize(input.WorldPos - LightPosition);

    // Note: Non-uniform scaling not supported
    float diffuseLighting = saturate(dot(input.Norm, -lightDir)); // per pixel diffuse lighting
    float LightDistanceSquared = 15000;
    // Introduce fall-off of light intensity
    diffuseLighting *= (LightDistanceSquared / dot(LightPosition - input.WorldPos, LightPosition - input.WorldPos));

    // Using Blinn half angle modification for perofrmance over correctness
    float3 h = normalize(normalize(-CameraPos.xyz - input.WorldPos) - lightDir);
    float SpecularPower = 5; //15
    float specLighting = pow(saturate(dot(h, input.Norm)), SpecularPower);
    float3 AmbientLightColor = float3(1, 1, 1)*0.01;
    float3 SpecularColor = float3(1, 1, 1);
    color = (saturate(
        //AmbientLightColor +
        (texture_color *  diffuseLighting * 0.6) + // Use light diffuse vector as intensity multiplier
        (SpecularColor * specLighting * 0.5) // Use light specular vector as intensity multiplier
    ), 1);
    //color.rgb = diffuseLighting;
    color.rgb = texture_color + specLighting;// *diffuseLighting;
    color.rgb *= shadowlight;
    return float4(color.rgb, texture_color.a);
}
