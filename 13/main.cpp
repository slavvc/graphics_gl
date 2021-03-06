
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/GL.h>
#include <SOIL/SOIL.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <array>
#include <streambuf>

using namespace std;

const double PI = acos(-1);

static int w = 0, h = 0;

float camera_rotate_x = 0.f;
float camera_rotate_y = 0.f;
float camera_dist = 0.f;

float light_rotate_x = 0.f;
float light_rotate_y = 0.f;
float light_dist = 2.;


GLuint texture_id;

GLuint vshader, fshader;


float *points, *normals, *tex_coords, *vcolors;;
int* pindices;
GLuint vbo_points, vbo_normals, vbo_tex_coords, ibo_points, vbo_colors;

int color_texture = 0;

struct {
	vector<array<float, 3>> points, normals;
	vector<array<float, 2>> tex_coords;
	vector<array<int, 4>> faces, fnormals, ftex_coords;

} model;

void loadTextures() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	texture_id = SOIL_load_OGL_texture("./african_head_SSS.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
}

//void loadShaders(string vname, string fname) {
//	ifstream vsource("vshader.glsl");
//	ifstream fsource("fshader.glsl");
//	string vprog(istreambuf_iterator<char>(vsource), istreambuf_iterator<char>());
//	string fprog(istreambuf_iterator<char>(fsource), istreambuf_iterator<char>());
//
//	vshader = glCreateShader(GL_VERTEX_SHADER);
//	fshader = glCreateShader(GL_FRAGMENT_SHADER);
//
//	/*GLchar* vprogs = vprog.data();
//	char const* fprogs = fprog.data();*/
//
//	//glShaderSource(vshader, 1, &vprogs, NULL);
//	//glShaderSource(fshader, 1, &fprogs, NULL);
//
//
//}


void loadModel(string filename) {
	//ofstream ofs("reading.txt");


	model.points.clear();
	model.normals.clear();
	model.tex_coords.clear();
	model.faces.clear();
	model.fnormals.clear();
	model.ftex_coords.clear();

	ifstream ifs(filename);
	string w;
	bool r = (bool)(ifs >> w);
	while (r) {
		if (w == "v") {
			float x, y, z;
			ifs >> x >> y >> z;
					
			model.points.push_back({ x, y, z });
		}
		else if (w == "vn") {
			float x, y, z;
			ifs >> x >> y >> z;
			model.normals.push_back({ x, y, z });
		}
		else if (w == "vt") {
			float u, v;
			ifs >> u >> v;
			model.tex_coords.push_back({ u, v });
		}
		else if (w == "f") {
			model.faces.push_back({ -1, -1, -1, -1 });
			model.ftex_coords.push_back({ -1, -1, -1, -1 });
			model.fnormals.push_back({ -1, -1, -1, -1 });
			bool triangle = false;
			for (int i = 0; i < 4; ++i) {
				r = (bool)(ifs >> w);
				if (i == 3 && !isdigit(w[0])) {
					triangle = true;
					break;
				}

				istringstream iss(w);
				string n;
				int pt[3] = { -1, -1, -1 };
				for (int j = 0; j < 3; ++j) {
					getline(iss, n, '/');
					//ofs << n << "\n";
					try {
						pt[j] = stoi(n) - 1;
					}
					catch (...) {}
				}
				/*ofs << pt[0] << ' ' << pt[1] << ' ' << pt[2] << '\n';
				ofs << '\n'*/;
				model.faces[model.faces.size() - 1][i] = pt[0];
				model.ftex_coords[model.ftex_coords.size() - 1][i] = pt[1];
				model.fnormals[model.fnormals.size() - 1][i] = pt[2];
			}
			if (triangle) {
				continue;
			}
		}
		else {
			getline(ifs, w);
		}
		r = (bool)(ifs >> w);
	}



	points = new float[model.faces.size() * 4 * 3];
	normals = new float[model.faces.size() * 4 * 3];
	tex_coords = new float[model.faces.size() * 4 * 2];
	vcolors = new float[model.faces.size() * 4 * 3];

	for (int i = 0; i < model.faces.size(); ++i) {
		auto& v1 = model.points[model.faces[i][0]];
		auto& v2 = model.points[model.faces[i][1]];
		auto& v3 = model.points[model.faces[i][2]];
		auto& v4 = model.points[model.faces[i][3] != -1 ? model.faces[i][3] : model.faces[i][0]];
		auto& n1 = model.normals[model.fnormals[i][0]];
		auto& n2 = model.normals[model.fnormals[i][1]];
		auto& n3 = model.normals[model.fnormals[i][2]];
		auto& n4 = model.normals[model.fnormals[i][3] != -1 ? model.fnormals[i][3] : model.fnormals[i][0]];
		auto& t1 = model.tex_coords[model.ftex_coords[i][0]];
		auto& t2 = model.tex_coords[model.ftex_coords[i][1]];
		auto& t3 = model.tex_coords[model.ftex_coords[i][2]];
		auto& t4 = model.tex_coords[model.ftex_coords[i][3] != -1 ? model.ftex_coords[i][3] : model.ftex_coords[i][0]];

		points[i * 4 * 3 + 0] = v1[0];
		points[i * 4 * 3 + 1] = v1[1];
		points[i * 4 * 3 + 2] = v1[2];
		normals[i * 4 * 3 + 0] = n1[0];
		normals[i * 4 * 3 + 1] = n1[1];
		normals[i * 4 * 3 + 2] = n1[2];
		tex_coords[i * 4 * 2 + 0] = t1[0];
		tex_coords[i * 4 * 2 + 1] = t1[1];
		vcolors[i * 4 * 3 + 0] = (double)rand() / RAND_MAX;
		vcolors[i * 4 * 3 + 1] = (double)rand() / RAND_MAX;
		vcolors[i * 4 * 3 + 2] = (double)rand() / RAND_MAX;

		points[i * 4 * 3 + 3] = v2[0];
		points[i * 4 * 3 + 4] = v2[1];
		points[i * 4 * 3 + 5] = v2[2];
		normals[i * 4 * 3 + 3] = n2[0];
		normals[i * 4 * 3 + 4] = n2[1];
		normals[i * 4 * 3 + 5] = n2[2];
		tex_coords[i * 4 * 2 + 2] = t2[0];
		tex_coords[i * 4 * 2 + 3] = t2[1];
		vcolors[i * 4 * 3 + 3] = (double)rand() / RAND_MAX;
		vcolors[i * 4 * 3 + 4] = (double)rand() / RAND_MAX;
		vcolors[i * 4 * 3 + 5] = (double)rand() / RAND_MAX;

		points[i * 4 * 3 + 6] = v3[0];
		points[i * 4 * 3 + 7] = v3[1];
		points[i * 4 * 3 + 8] = v3[2];
		normals[i * 4 * 3 + 6] = n3[0];
		normals[i * 4 * 3 + 7] = n3[1];
		normals[i * 4 * 3 + 8] = n3[2];
		tex_coords[i * 4 * 2 + 4] = t3[0];
		tex_coords[i * 4 * 2 + 5] = t3[1];
		vcolors[i * 4 * 3 + 6] = (double)rand() / RAND_MAX;
		vcolors[i * 4 * 3 + 7] = (double)rand() / RAND_MAX;
		vcolors[i * 4 * 3 + 8] = (double)rand() / RAND_MAX;
		
		points[i * 4 * 3 + 9] = v4[0];
		points[i * 4 * 3 + 10] = v4[1];
		points[i * 4 * 3 + 11] = v4[2];
		normals[i * 4 * 3 + 9] = n4[0];
		normals[i * 4 * 3 + 10] = n4[1];
		normals[i * 4 * 3 + 11] = n4[2];
		tex_coords[i * 4 * 2 + 6] = t4[0];
		tex_coords[i * 4 * 2 + 7] = t4[1];
		vcolors[i * 4 * 3 + 9] = (double)rand() / RAND_MAX;
		vcolors[i * 4 * 3 + 10] = (double)rand() / RAND_MAX;
		vcolors[i * 4 * 3 + 11] = (double)rand() / RAND_MAX;
	}

	pindices = new int[model.faces.size() * 4];
	for (int i = 0; i < model.faces.size() * 4; ++i) {
		pindices[i] = i;
	}

	glGenBuffers(1, &vbo_points);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_points);
	glBufferData(GL_ARRAY_BUFFER, model.faces.size() * 4 * 3 * sizeof(float), points, GL_STATIC_DRAW);


	glGenBuffers(1, &vbo_tex_coords);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_tex_coords);
	glBufferData(GL_ARRAY_BUFFER, model.faces.size() * 4 * 2 * sizeof(float), tex_coords, GL_STATIC_DRAW);

	glGenBuffers(1, &vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, model.faces.size() * 4 * 3 * sizeof(float), normals, GL_STATIC_DRAW);

	glGenBuffers(1, &vbo_colors);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
	glBufferData(GL_ARRAY_BUFFER, model.faces.size() * 4 * 3 * sizeof(float), normals, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo_points);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_points);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.faces.size() * 4 * sizeof(int), pindices, GL_STATIC_DRAW);

}


void drawModel() {

	glPushMatrix();

	if (color_texture == 1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_id);
	}


	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_points);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (color_texture == 1) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_tex_coords);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
	}
	else {
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glColorPointer(3, GL_FLOAT, 0, 0);
	}


	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glNormalPointer(GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_points);

	glDrawElements(GL_QUADS, model.faces.size() * 4, GL_UNSIGNED_INT, 0);

	if (color_texture == 1){
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else {
		glDisableClientState(GL_COLOR_ARRAY);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glPopMatrix();
}

void init() {
	glClearColor(0, 0.f, 0, 1);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	
	glewInit();

	const GLfloat light_diffuse[] = { 10.0, 10.0, 10.0, 1.0 };
	const GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	loadTextures();

	// camera light
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001f);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.f);

	// headlights
	glLightfv(GL_LIGHT5, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT5, GL_SPECULAR, light_specular);
	glLightf(GL_LIGHT5, GL_QUADRATIC_ATTENUATION, 0.f);
	glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, 40.f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void uninit() {
	delete points;
	delete normals;
	delete tex_coords;
	delete pindices;

	glDeleteBuffers(1, &vbo_points);
	glDeleteBuffers(1, &vbo_tex_coords);
	glDeleteBuffers(1, &vbo_normals);
	glDeleteBuffers(1, &ibo_points);
	glDeleteBuffers(1, &vbo_colors);
}

void drawFloor() {

	glBegin(GL_QUADS);
	bool rotated = false;
	float step = 0.25f;
	for (float x = -10.f; x < 10.f; x += step) {
		for (float z = -10.f; z < 10.f; z += step) {
			if (rotated)
				glTexCoord2f(0, 0);
			else
				glTexCoord2f(0, 1);
			glNormal3f(0, 1, 0); glVertex3f(x, 0, z);
			if (rotated)
				glTexCoord2f(0, 1);
			else
				glTexCoord2f(1, 1);
			glNormal3f(0, 1, 0); glVertex3f(x, 0, z + step);
			if (rotated)
				glTexCoord2f(1, 1);
			else
				glTexCoord2f(1, 0);
			glNormal3f(0, 1, 0); glVertex3f(x + step, 0, z + step);
			if (rotated)
				glTexCoord2f(1, 0);
			else
				glTexCoord2f(0, 0);
			glNormal3f(0, 1, 0); glVertex3f(x + step, 0, z);
			rotated = !rotated;
		}
		rotated = !rotated;
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
}


void drawLight() {
	glPushMatrix();
	glRotatef(light_rotate_y, 0, 1, 0);
	glRotatef(light_rotate_x, 0, 0, 1);
	glTranslatef(light_dist, 0., 0.);

	const GLfloat headlight_pos[4] = { 0.f, 0.f, 0.f, 1.f };
	const GLfloat headlight_spot_direction[4] = { -1, 0, 0, 1 };

	glLightfv(GL_LIGHT5, GL_POSITION, headlight_pos);
	glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, headlight_spot_direction);
	glutWireCube(0.5f);
	glPopMatrix();
}

void update() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	drawLight();
	drawModel();

	glFlush();
	glutSwapBuffers();
}

void setCameraLight() {
	GLint viewport[4];
	GLdouble modelview_mat[16];
	GLdouble projection_mat[16];
	GLdouble camera_posd[3];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview_mat);
	glGetDoublev(GL_PROJECTION_MATRIX, projection_mat);

	gluUnProject(double(viewport[2] - viewport[0]) / 2, double(viewport[3] - viewport[1]) / 2, 0.,
		modelview_mat, projection_mat, viewport,
		&camera_posd[0], &camera_posd[1], &camera_posd[2]);

	GLfloat camera_pos[4]{ 0.f, 0.f, 0.f, 1.f };
	for (int i = 0; i < 3; ++i)
		camera_pos[i] = (GLfloat)camera_posd[i];

	GLfloat spot_direction[3];
	for (int i = 0; i < 3; ++i)
		spot_direction[i] = -camera_pos[i];

	double len = sqrt(spot_direction[0] * spot_direction[0] +
		spot_direction[1] * spot_direction[1] +
		spot_direction[2] * spot_direction[2]);
	for (int i = 0; i < 3; ++i)
		spot_direction[i] /= len;

	glLightfv(GL_LIGHT0, GL_POSITION, camera_pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
}

void updateCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.f, (float)w / h, 0.1f, 1000.f);
	gluLookAt(0., 5., -20, 0., 0., 0., 0., 1., 0.);
	glTranslatef(0.f, -5 * camera_dist, 20 * camera_dist);
	glRotatef(camera_rotate_x, 1, 0, 0);
	glRotatef(camera_rotate_y, 0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	setCameraLight();
}

void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
			if (glIsEnabled(GL_LIGHT0))
				glDisable(GL_LIGHT0);
			else
				glEnable(GL_LIGHT0);
			break;
		case GLUT_RIGHT_BUTTON:
			if (glIsEnabled(GL_LIGHT5)) {
				glDisable(GL_LIGHT5);
			}
			else {
				glEnable(GL_LIGHT5);
			}
			break;
		default:
			if (button == 3)
				camera_dist -= 0.1f;
			else if (button == 4)
				camera_dist += 0.1f;
			else return;

			updateCamera();
		}
	}
	glutPostRedisplay();
}

void driving(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		light_rotate_x += 5.f;
		break;
	case GLUT_KEY_DOWN:
		light_rotate_x -= 5.f;
		break;

	case GLUT_KEY_RIGHT:
		light_rotate_y += 5.f;
		break;
	case GLUT_KEY_LEFT:
		light_rotate_y -= 5.f;
		break;
	case GLUT_KEY_ALT_R:
		light_dist += 0.5f;
		break;
	case GLUT_KEY_CTRL_R:
		light_dist -= 0.5f;
		break;
	default:
		return;
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		camera_rotate_y += 5;
		if (camera_rotate_y > 360)
			camera_rotate_y -= 360;
		updateCamera();
		break;
	case 'd':
		camera_rotate_y -= 5;
		if (camera_rotate_y < -360)
			camera_rotate_y += 360;
		updateCamera();
		break;
	case 'w':
		camera_rotate_x -= 2;
		updateCamera();
		break;
	case 's':
		camera_rotate_x += 2;
		updateCamera();
		break;
	case '1':
		color_texture = 1;
		break;
	case '2':
		color_texture = 0;
		break;
	case '9':
		camera_dist += 0.1f;
		updateCamera();
		break;
	case '0':
		camera_dist -= 0.1f;
		updateCamera();
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void reshape(int width, int height) {
	w = width;
	h = height;

	glViewport(0, 0, w, h);
	updateCamera();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("texture and lighting");
	init();

	loadModel("../../obj/Wolf_obj.obj");
	/*loadModel("../../obj/african_head.obj");*/

	glutReshapeFunc(reshape);
	glutDisplayFunc(update);
	glutSpecialFunc(driving);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glutMainLoop();
	uninit();
	return 0;
}