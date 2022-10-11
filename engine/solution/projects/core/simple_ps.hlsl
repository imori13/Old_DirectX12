struct vs_output
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct ps_output
{
    float4 color : SV_Target0;
};

ps_output main(vs_output input)
{
    ps_output output = (ps_output) 0;
    
    output.color = input.color;
    
    return output;
}