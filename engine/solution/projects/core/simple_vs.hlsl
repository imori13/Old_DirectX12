struct vs_input
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct vs_output
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer transform : register(b0)
{
    float4x4 world : packoffset(c0);
    float4x4 view : packoffset(c4);
    float4x4 proj : packoffset(c8);
}

vs_output main(vs_input input)
{
    vs_output output = (vs_output) 0;
    
    float4 local_pos = float4(input.position, 1.0f);
    float4 world_pos = mul(world, local_pos);
    float4 view_pos = mul(view, world_pos);
    float4 proj_pos = mul(proj, view_pos);
    
    output.position = proj_pos;
    output.color = input.color;
    
	return output;
}