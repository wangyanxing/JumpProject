
attribute vec3  a_position;
attribute vec2  a_texCoord;

uniform vec4 g_viewportSize;

varying vec2 vTexCoord0;

void main()
{
	gl_Position = CC_MVPMatrix * vec4(a_position.xy, 0.0, 1.0);
	
    vec2 scaledUV = a_texCoord.xy * 4.0;
	
	vTexCoord0.xy = scaledUV + g_viewportSize.zw;
    vTexCoord0.xy = scaledUV + vec2(g_viewportSize.z, -g_viewportSize.w);
    vTexCoord0.xy = scaledUV + vec2(-g_viewportSize.z, g_viewportSize.w);
    vTexCoord0.xy = scaledUV - g_viewportSize.zw;
}