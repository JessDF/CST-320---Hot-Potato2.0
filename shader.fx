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

cbuffer ConstantBuffer: register(b0) {
  matrix world;
  matrix view;
  matrix projection;
};



//--------------------------------------------------------------------------------------
struct VS_INPUT {
  float4 Pos: POSITION;
  float2 Tex: TEXCOORD0;
};

struct PS_INPUT {
  float4 Pos: SV_POSITION;
  float2 Tex: TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input) {
  PS_INPUT output = (PS_INPUT)0;
  output.Pos = mul( input.Pos, world );
  output.Pos = mul( output.Pos, view );
  output.Pos = mul( output.Pos, projection );
  output.Tex = input.Tex;
    
  return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input): SV_Target {
   float4 color = txDiffuse.Sample( samLinear, input.Tex );
  float depth = saturate(input.Pos.z / input.Pos.w);
  //depth = pow(depth,0.97);
  //color = depth;// (depth*0.9 + 0.02);
  return color;
}
