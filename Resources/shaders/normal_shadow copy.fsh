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
#ifdef GL_ES
    vec2 p = (2.0*gl_FragCoord.xy-data.xy)/min(data.y,data.x);
#else
    vec2 p = (gl_FragCoord.xy-data.xy)/min(data.y,data.x);
#endif
    
    p.x += data.z;
    p.y += data.w;
    
    float ratio = 1.0 - colorSrc.w * length(p);
    vec3 bcol = mix(colorDest.xyz, colorSrc.xyz, ratio);
    
#if 1
    vec2 uv = gl_FragCoord.xy / data.xy - vec2(0.5);
    float darkness = 1.1;
    gl_FragColor = vec4( mix( bcol.rgb, vec3( 1.0 - darkness ), dot( uv, uv ) ), 1.0 );
    
    gl_FragColor.rgb *= v_fragmentColor.rgb;
#else
    gl_FragColor.rgb = v_fragmentColor.rgb * bcol;
    gl_FragColor.a = 1.0;
#endif
}