#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;    
varying vec2 v_texCoord;

void main(void) {
    
    vec4 texel = texture2D(CC_Texture0, v_texCoord);
    texel.rgb *= texel.a * v_fragmentColor.a;
    texel.rgb *= v_fragmentColor.rgb*2.0;
    gl_FragColor = texel;
}