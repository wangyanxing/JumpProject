#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

void main(void) {
#if 1
  gl_FragColor = v_fragmentColor;
#else
  gl_FragColor = v_fragmentColor * smoothstep(0.0,
                                              length(fwidth(v_texCoord)),
                                              1.0 - length(v_texCoord));
#endif
}