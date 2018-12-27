uniform vec4 hc1;
uniform vec4 hc2;
uniform vec4 vc1;
uniform float hw;
uniform float vw;

void main(){
  if (mod(gl_FragCoord.x, 2*vw) < vw){
    gl_FragColor = vc1;
  }else{
    if (mod(gl_FragCoord.y, 2*hw) < hw){
      gl_FragColor = hc1;
    }else{
      gl_FragColor = hc2;
    }
  }
  
}