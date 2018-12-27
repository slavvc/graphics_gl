attribute vec3 position;
uniform mat4 scale;
uniform mat4 rotx;
uniform mat4 roty;
uniform mat4 rotz;
uniform mat4 proj;
void main(){

  gl_Position = scale * proj * rotx * roty * rotz * vec4(position, 1.0);
  /*mat4 i = mat4(1., 0., 0., 0.,
                0., 1., 0., 0.,
                0., 0., 1., 0.,
                0., 0., 0., 1.);*/
  //gl_Position = scale * proj * vec4(position, 1.0) + 0 * (proj * rotx * roty * rotz * scale * vec4(position, 1.0));
}