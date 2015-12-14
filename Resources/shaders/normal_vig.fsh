#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;    
varying vec2 v_texCoord;

uniform vec3 resolution;

// data.xy -> resolution
// data.zw -> center

void main(void) {
    
    vec4 texel = texture2D(CC_Texture0, v_texCoord);
    
    vec2 uv = gl_FragCoord.xy / resolution.xy - vec2(0.5);
    float darkness = resolution.z;
    
    gl_FragColor = vec4( mix( texel.rgb, vec3( 1.0 - darkness ), dot( uv, uv ) ), texel.a );
    gl_FragColor.rgb *= v_fragmentColor.rgb;
}