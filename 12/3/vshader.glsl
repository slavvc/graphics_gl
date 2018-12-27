attribute vec3 position;
attribute vec2 tex_c;
uniform mat4 scale;
uniform mat4 rotx;
uniform mat4 roty;
uniform mat4 rotz;
uniform mat4 proj;

varying vec2 tex_coord;

void main(){
  tex_coord = tex_c;
  gl_Position = scale * proj * rotx * roty * rotz * vec4(position, 1.0);
  
}