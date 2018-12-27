uniform float alpha;
uniform sampler2D tex1;
uniform sampler2D tex2;
varying vec2 tex_coord;

void main(){
  gl_FragColor = mix(texture(tex1, tex_coord), texture(tex2, tex_coord), alpha);
}
