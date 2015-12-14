
uniform vec2 darkness;
varying vec2 v_texCoord;

void main() {
    vec4 texel = texture2D(CC_Texture0, v_texCoord);
#if 1
    vec2 uv = v_texCoord - vec2( 0.5 );
    gl_FragColor = vec4( mix( texel.rgb, vec3( 1.0 - darkness.x ), dot( uv, uv ) ), texel.a );
    gl_FragColor.rgb *= darkness.y;
#else
    gl_FragColor.rgb = texel.rgb * darkness.y;
    gl_FragColor.a = texel.a;
#endif
}