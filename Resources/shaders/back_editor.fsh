#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;    
varying vec2 v_texCoord;

uniform vec4 data;
uniform vec4 color;
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
    
    float ratio = 1.0 - color.w * length(p);
    vec3 bcol = mix(colorDest.xyz, color.xyz, ratio);
    
#if 0
    float darkness = 1.2;
    vec2 uv = v_texCoord - vec2( 0.5 );
    gl_FragColor = vec4( mix( bcol.rgb, vec3( 1.0 - darkness ), dot( uv, uv ) ), 1.0 );
#else
    gl_FragColor = vec4(bcol,1.0);
#endif
}