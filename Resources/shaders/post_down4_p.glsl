
#ifdef GL_ES
precision mediump float;
#endif

varying vec2 vTexCoord0;

void main()
{
	gl_FragColor = vec4(0.0);

	gl_FragColor += texture2D(CC_Texture0, vTexCoord0.xy);
	gl_FragColor += texture2D(CC_Texture0, vTexCoord0.xy);
	gl_FragColor += texture2D(CC_Texture0, vTexCoord0.xy);
	gl_FragColor += texture2D(CC_Texture0, vTexCoord0.xy);
	gl_FragColor *= 0.25;
}
