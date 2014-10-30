#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;    
varying vec2 v_texCoord;

uniform vec4 data;
uniform vec4 colorSrc;
uniform vec4 colorDest;

// data.xy -> resolution
// data.zw -> center

void main(void)
{
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y *= 640.0/540.0;
    vec2 p = (fragCoord.xy-data.xy)/min(data.y,data.x);
    
    p.x += data.z;
    p.y += data.w;
    
    float ratio = 1.0 - colorSrc.w * length(p);
    vec3 bcol = mix(colorDest.xyz, colorSrc.xyz, ratio);
    
    gl_FragColor = vec4(bcol * v_fragmentColor.rgb,1.0);
}