struct vs_input
{
    float3 position : POSITION;
    float4 color : COLOR;

    float4 world_0 : WORLD0;
    float4 world_1 : WORLD1;
    float4 world_2 : WORLD2;
    float4 world_3 : WORLD3;
};

struct vs_output
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer transform : register(b0)
{
    float4x4 view : packoffset(c0);
    float4x4 proj : packoffset(c4);
}

vs_output main(vs_input input)
{
    vs_output output = (vs_output) 0;

    float4 local_pos = float4(input.position, 1.0f);
    float4 world_pos = mul(transpose(float4x4(input.world_0, input.world_1, input.world_2, input.world_3)), local_pos);
    float4 view_pos = mul(view, world_pos);
    float4 proj_pos = mul(proj, view_pos);
    
    output.position = proj_pos;
    output.color = input.color;
    
	return output;
}