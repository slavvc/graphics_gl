uniform sampler2D tex;
varying vec2 tex_coord;
void main(){
  gl_FragColor = texture(tex, tex_coord);
}