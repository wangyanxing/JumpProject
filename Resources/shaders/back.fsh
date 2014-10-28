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
    //float time = CC_Time[1];
    vec2 p = (2.0*gl_FragCoord.xy-data.xy)/min(data.y,data.x);
    
    p.x += data.z;
    p.y += data.w;
    
    float ratio = 1.0 - color.w * length(p);
    vec3 bcol = mix(colorDest.xyz, color.xyz, ratio);
    
    // background color
    //vec3 bcol = vec3(color.x, color.y, color.z/*-0.07*p.y*/)*(1.0-color.w*length(p));
    
    gl_FragColor = vec4(bcol,1.0);
}