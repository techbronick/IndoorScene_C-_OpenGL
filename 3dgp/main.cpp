#include <iostream>
#include "GL/glew.h"
#include "GL/3dgl.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Include GLM extensions
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

bool ambient, directional, blamp1, blamp2, specular; // Booleans used for switching the light 

//Defining the shader program object
C3dglProgram Program;

// 3D models
C3dglModel camera;
C3dglModel living;
C3dglModel teaPot;
C3dglModel table;
C3dglModel lamp1;
C3dglModel lamp2;
C3dglModel vase;
C3dglModel helmet;

//Textures
GLuint tableTex;
GLuint chairsTex;
GLuint marbleTex;
GLuint blankTex;
GLuint CubeTexVase;
GLuint CubeTexHelmet;

// camera position (for first person type camera navigation)

mat4 matrixView;			// The View Matrix

vec3 cam(0);				// Camera movement values

float angleTilt = 15;		// Tilt Angle
float angleRot = 0.1f;		// Camera orbiting angle

float vertices[] = {
			-4, 0, -4, 4, 0, -4, 0, 7, 0, -4, 0, 4, 4, 0, 4, 0, 7, 0,
			-4, 0, -4, -4, 0, 4, 0, 7, 0, 4, 0, -4, 4, 0, 4, 0, 7, 0,
			-4, 0, -4, -4, 0, 4, 4, 0, -4, 4, 0, 4 };
float normals[] = {
	0, 4, -7, 0, 4, -7, 0, 4, -7, 0, 4, 7, 0, 4, 7, 0, 4, 7,
	-7, 4, 0, -7, 4, 0, -7, 4, 0, 7, 4, 0, 7, 4, 0, 7, 4, 0,
	0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0 };
unsigned indices[] = {
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 13, 14, 15 };


// buffers names
unsigned vertexBuffer = 0;
unsigned normalBuffer = 0;
unsigned indexBuffer = 0;



bool init()
{
	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!


	// Initialise Shaders
	C3dglShader VertexShader;
	C3dglShader FragmentShader;

	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/basic.vert")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/basic.frag")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!Program.Create()) return false;
	if (!Program.Attach(VertexShader)) return false;
	if (!Program.Attach(FragmentShader)) return false;
	if (!Program.Link()) return false;
	if (!Program.Use(true)) return false;

	// prepare vertex data
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// prepare normal data
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	// prepare indices array
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// glut additional setup
	glutSetVertexAttribCoord3(Program.GetAttribLocation("aVertex"));
	glutSetVertexAttribNormal(Program.GetAttribLocation("aNormal"));


	// load your 3D models here!
	if (!camera.load("models\\camera.3ds")) return false;
	//if (!living.load("models\\LivingRoom.obj")) return false;
	if (!teaPot.load("models\\utah_teapot_hires.obj")) return false;
	if (!table.load("models\\table.obj")) return false;
	if (!lamp1.load("models\\lamp.obj")) return false;
	if (!lamp2.load("models\\lamp.obj")) return false;
	if (!vase.load("models\\vase.obj")) return false;
	if (!helmet.load("models\\Mandalorian.obj")) return false;


	// load Cube Maps
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &CubeTexVase);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeTexVase);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &CubeTexHelmet);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeTexHelmet);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//Sending the texture cube map uniform used for reflection to the shaders
	Program.SendUniform("textureCubeMap", 1);


	//Loading texture bitmaps

	C3dglBitmap bm; // BitMap Declaration

	//Table
	bm.Load("models/tableTexture.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tableTex);
	glBindTexture(GL_TEXTURE_2D, tableTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());

	//Chairs
	bm.Load("models/chairsTexture.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &chairsTex);
	glBindTexture(GL_TEXTURE_2D, chairsTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());


	//Marble
	bm.Load("models/marbleTexture.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &marbleTex);
	glBindTexture(GL_TEXTURE_2D, marbleTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());

	//Loading none (simple-white) texture
	glGenTextures(1, &blankTex);
	glBindTexture(GL_TEXTURE_2D, blankTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	BYTE bytes[] = { 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);

	// Send the texture info to the shaders
	Program.SendUniform("texture0", 0);

	//Initialise the light

	//Ambient Light
	Program.SendUniform("lightAmbient.on", 0);
	Program.SendUniform("lightAmbient.color", 0.13f, 0.13f, 0.13f);

	//Directional Light
	Program.SendUniform("lightDir.on", 0);
	Program.SendUniform("lightDir.direction", 1.0f, 0.5f, 1.0f);
	Program.SendUniform("lightDir.diffuse", 0.2f, 0.2f, 0.2f);

	//Point Light #1 
	Program.SendUniform("lightPoint1.on", 0);
	Program.SendUniform("bulbAmbient1.on", 0);
	Program.SendUniform("lightPoint1.diffuse", 0.5f, 0.5f, 0.5f);
	Program.SendUniform("lightPoint1.position", 1.65f, 4.35f, 0.6f);

	//Point Light #2
	Program.SendUniform("lightPoint2.on", 0);
	Program.SendUniform("bulbAmbient2.on", 0);
	Program.SendUniform("lightPoint2.diffuse", 0.5f, 0.5f, 0.5f);
	Program.SendUniform("lightPoint2.position", -1.3f, 4.35f, -1.2f);

	//Specular Light
	Program.SendUniform("materialSpecular", 0.6f, 0.6f, 1.0f);
	Program.SendUniform("shininess", 10);

	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));
	matrixView *= lookAt(
		vec3(0.0, 5.0, 5.0),
		vec3(0.0, 5.0, 0.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);   // deep grey background

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Shift+AD or arrow key to auto-orbit" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;
	return true;
}

void done()
{
}

void renderObjects(mat4 matrixView, float theta)
{
	mat4 m;

	glActiveTexture(GL_TEXTURE0);
	Program.SendUniform("reflectionPower", 0.0);

	//Render Textured Objects

	//Table & Chairs setup
	glBindTexture(GL_TEXTURE_2D, tableTex);
	Program.SendUniform("materialAmbient", 0.8f, 0.8f, 0.8f);
	Program.SendUniform("materialDiffuse", 0.6f, 0.6f, 0.6f);

	m = matrixView;
	m = translate(m, vec3(0.0f, 0.0f, 0.0f));
	m = rotate(m, radians(0.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.004f, 0.004f, 0.004f));
	table.render(1, m);


	//Chairs Setup
	glBindTexture(GL_TEXTURE_2D, chairsTex);
	Program.SendUniform("materialAmbient", 0.8f, 0.8f, 0.8f);
	Program.SendUniform("materialDiffuse", 0.6f, 0.6f, 0.6f);

	//render chair 1

	m = rotate(m, radians(0.f), vec3(0.0f, 1.0f, 0.0f));
	table.render(0, m);

	//render chair 2

	m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	table.render(0, m);

	//render chair 3

	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	table.render(0, m);

	//render chair 4

	m = rotate(m, radians(270.f), vec3(0.0f, 1.0f, 0.0f));
	table.render(0, m);


	//Tea Pot setup
	glBindTexture(GL_TEXTURE_2D, marbleTex);
	Program.SendUniform("materialAmbient", 0.6f, 0.6f, 0.6f);
	Program.SendUniform("materialDiffuse", 0.6f, 0.6f, 0.6f);

	m = matrixView;
	m = translate(m, vec3(1.8f, 3.03f, 0.0f));
	m = rotate(m, radians(270.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.5f, 0.5f, 0.5f));
	teaPot.render(m);


	//Render Non Textured Objects
	glBindTexture(GL_TEXTURE_2D, blankTex);

	//Table Lamps
	Program.SendUniform("materialAmbient", 0.255f, 0.020f, 0.147f);
	Program.SendUniform("materialDiffuse", 0.255f, 0.020f, 0.147f);

	m = matrixView;
	m = translate(m, vec3(2.0f, 3.043f, 1.2f));
	m = rotate(m, radians(300.0f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.02f, 0.02f, 0.02f));
	lamp1.render(m);

	m = matrixView;
	m = translate(m, vec3(-2.0f, 3.043f, -1.2f));
	m = rotate(m, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.02f, 0.02f, 0.02f));
	lamp2.render(m);


	//Bulbs
	Program.SendUniform("materialAmbient", 1.0, 1.0, 1.0);
	Program.SendUniform("materialDiffuse", 0.1f, 0.1f, 0.1f);
	Program.SendUniform("bulbAmbient1.color", 1.0f, 1.0f, 1.0f);

	m = matrixView;
	m = translate(m, vec3(1.65f, 4.3f, 0.6f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	Program.SendUniform("matrixModelView", m);
	glutSolidSphere(1, 32, 32);
	Program.SendUniform("bulbAmbient1.color", 0.1f, 0.1f, 0.1f);


	Program.SendUniform("bulbAmbient2.color", 1.0f, 1.0f, 1.0f);
	m = matrixView;
	m = translate(m, vec3(-1.3f, 4.3f, -1.2f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	Program.SendUniform("matrixModelView", m);
	glutSolidSphere(1, 32, 32);
	Program.SendUniform("bulbAmbient2.color", 0.1f, 0.1f, 0.1f);


	//Pyramid setup
	Program.SendUniform("materialAmbient", 0.8f, 0.0f, 0.1f);
	Program.SendUniform("materialDiffuse", 0.8f, 0.0f, 0.1f);

	m = matrixView;
	m = translate(m, vec3(-1.8f, 3.74f, 0.0f));
	m = rotate(m, radians(180.f), vec3(1.0f, 0.0f, 0.0f));
	m = rotate(m, radians(theta), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));

	// Get Attribute Locations
	Program.SendUniform("matrixModelView", m);
	GLuint attribVertex = Program.GetAttribLocation("aVertex");
	GLuint attribNormal = Program.GetAttribLocation("aNormal");

	// Enable vertex attribute arrays
	glEnableVertexAttribArray(attribVertex);
	glEnableVertexAttribArray(attribNormal);

	// Bind (activate) the vertex buffer and set the pointer to it
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Bind (activate) the normal buffer and set the pointer to it
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Draw triangles – using index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	// Disable arrays
	glDisableVertexAttribArray(attribVertex);
	glDisableVertexAttribArray(attribNormal);

}

void renderReflectiveVase(mat4 matrixView, float theta)
{
	mat4 m;

	//Vase reflective

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeTexVase);

	Program.SendUniform("reflectionPower", 0.8f);
	Program.SendUniform("materialAmbient", 0.2f, 0.2f, 0.8f);
	Program.SendUniform("materialDiffuse", 0.2f, 0.2f, 0.8f);

	m = matrixView;
	m = translate(m, vec3(0.0f, 3.043f, 0.0f));
	m = rotate(m, radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.08f, 0.08f, 0.08f));
	vase.render(m);


}

void renderReflectiveHelmet(mat4 matrixView, float theta)
{
	mat4 m;
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeTexHelmet);
	//Helmet setup
	Program.SendUniform("reflectionPower", 0.8);
	Program.SendUniform("materialAmbient", 0.0f, 0.1f, 0.1f);
	Program.SendUniform("materialDiffuse", 0.0f, 0.1f, 0.1f);


	m = matrixView;
	m = translate(m, vec3(-1.68f, 3.82f, 0.0f));
	m = rotate(m, radians(-theta), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.018f, 0.018f, 0.018f));
	helmet.render(m);


}

void prepareCubeMapVase(float x, float y, float z, float theta)
{
	// Store the current viewport in a safe place
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int w = viewport[2];
	int h = viewport[3];

	// setup the viewport to 256x256, 90 degrees FoV (Field of View)
	glViewport(0, 0, 256, 256);
	Program.SendUniform("matrixProjection", perspective(radians(90.f), 1.0f, 0.02f, 1000.0f));

	// render environment 6 times
	Program.SendUniform("reflectionPower", 0.0);
	for (int i = 0; i < 6; ++i)
	{
		// clear background
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// setup the camera
		const GLfloat ROTATION[6][6] =
		{	// at              up
			{ 1.0, 0.0, 0.0,   0.0, -1.0, 0.0 },  // pos x
			{ -1.0, 0.0, 0.0,  0.0, -1.0, 0.0 },  // neg x
			{ 0.0, 1.0, 0.0,   0.0, 0.0, 1.0 },   // pos y
			{ 0.0, -1.0, 0.0,  0.0, 0.0, -1.0 },  // neg y
			{ 0.0, 0.0, 1.0,   0.0, -1.0, 0.0 },  // poz z
			{ 0.0, 0.0, -1.0,  0.0, -1.0, 0.0 }   // neg z
		};
		mat4 matrixView2 = lookAt(
			vec3(x, y, z),
			vec3(x + ROTATION[i][0], y + ROTATION[i][1], z + ROTATION[i][2]),
			vec3(ROTATION[i][3], ROTATION[i][4], ROTATION[i][5]));

		// send the View Matrix
		Program.SendUniform("matrixView", matrixView);

		// render scene objects except the object that will use this map

		glActiveTexture(GL_TEXTURE0);
		renderObjects(matrixView2, theta);

		glActiveTexture(GL_TEXTURE1);
		renderReflectiveHelmet(matrixView2, theta);

		// send the image to the cube texture
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeTexVase);
		glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, 0, 0, 256, 256, 0);
	}

	// restore the matrixView, viewport and projection
	void reshape(int w, int h);
	reshape(w, h);
}

void prepareCubeMapHelmet(float x, float y, float z, float theta)
{
	// Store the current viewport in a safe place
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int w = viewport[2];
	int h = viewport[3];

	// setup the viewport to 256x256, 90 degrees FoV (Field of View)
	glViewport(0, 0, 256, 256);
	Program.SendUniform("matrixProjection", perspective(radians(90.f), 1.0f, 0.02f, 1000.0f));

	// render environment 6 times
	Program.SendUniform("reflectionPower", 0.0);
	for (int i = 0; i < 6; ++i)
	{
		// clear background
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// setup the camera
		const GLfloat ROTATION[6][6] =
		{	// at              up
			{ 1.0, 0.0, 0.0,   0.0, -1.0, 0.0 },  // pos x
			{ -1.0, 0.0, 0.0,  0.0, -1.0, 0.0 },  // neg x
			{ 0.0, 1.0, 0.0,   0.0, 0.0, 1.0 },   // pos y
			{ 0.0, -1.0, 0.0,  0.0, 0.0, -1.0 },  // neg y
			{ 0.0, 0.0, 1.0,   0.0, -1.0, 0.0 },  // poz z
			{ 0.0, 0.0, -1.0,  0.0, -1.0, 0.0 }   // neg z
		};
		mat4 matrixView2 = lookAt(
			vec3(x, y, z),
			vec3(x + ROTATION[i][0], y + ROTATION[i][1], z + ROTATION[i][2]),
			vec3(ROTATION[i][3], ROTATION[i][4], ROTATION[i][5]));

		// send the View Matrix
		Program.SendUniform("matrixView", matrixView);

		// render scene objects - all but the reflective one
		glActiveTexture(GL_TEXTURE0);
		renderObjects(matrixView2, theta);

		// render scene objects except the object that reflects
		glActiveTexture(GL_TEXTURE1);
		renderReflectiveVase(matrixView2, theta);


		// send the image to the cube texture
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeTexHelmet);
		glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, 0, 0, 256, 256, 0);
	}

	// restore the matrixView, viewport and projection
	void reshape(int w, int h);
	reshape(w, h);
}

void render()
{
	// this global variable controls the animation
	float theta = glutGet(GLUT_ELAPSED_TIME) * 0.01f;

	prepareCubeMapVase(0.0f, 4.2f, 0.0f, theta);

	prepareCubeMapHelmet(-1.68f, 4.0f, 0.0f, theta);


	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	mat4 m = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f)); // switch tilt off
	m = translate(m, cam);

	// animate camera motion (controlled by WASD keys)
	m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));			 // switch tilt on
	m = m * matrixView;

	m = rotate(m, radians(angleRot), vec3(0.f, 1.f, 0.f));			 // animate camera orbiting
	matrixView = m;
	Program.SendUniform("matrixView", matrixView);

	//Called to render non-reflective objects
	renderObjects(matrixView, theta);

	//Render reflective objects
	renderReflectiveVase(matrixView, theta);

	renderReflectiveHelmet(matrixView, theta);


	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();

}

// called before window opened or resized - to setup the Projection Matrix
void reshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 matrixProjection = perspective(radians(60.f), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix
	Program.SendUniform("matrixProjection", matrixProjection);

}




// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': cam.z = std::max(cam.z * 1.05f, 0.01f); break;
	case 's': cam.z = std::min(cam.z * 1.05f, -0.01f); break;
	case 'a': cam.x = std::max(cam.x * 1.05f, 0.01f); angleRot = 0.1f; break;
	case 'd': cam.x = std::min(cam.x * 1.05f, -0.01f); angleRot = -0.1f; break;
	case 'e': cam.y = std::max(cam.y * 1.05f, 0.01f); break;
	case 'q': cam.y = std::min(cam.y * 1.05f, -0.01f); break;
	case '1': ambient = (ambient == 0) ? 1 : 0; Program.SendUniform("lightAmbient.on", ambient);  break;
	case '2': directional = (directional == 0) ? 1 : 0; Program.SendUniform("lightDir.on", directional); break;
	case '3': blamp1 = (blamp1 == 0) ? 1 : 0; Program.SendUniform("lightPoint1.on", blamp1); Program.SendUniform("bulbAmbient1.on", blamp1);  break;
	case '4': blamp2 = (blamp2 == 0) ? 1 : 0; Program.SendUniform("lightPoint2.on", blamp2); Program.SendUniform("bulbAmbient2.on", blamp2);  break;
	case '5': specular = (specular == 0) ? 1 : 0; Program.SendUniform("lightPoint1.specular", specular, specular, specular); Program.SendUniform("lightPoint2.specular", specular, specular, specular); break;
	case '6': break;
	}

	// speed limit
	cam.x = std::max(-0.15f, std::min(0.15f, cam.x));
	cam.y = std::max(-0.15f, std::min(0.15f, cam.y));
	cam.z = std::max(-0.15f, std::min(0.15f, cam.z));
	// stop orbiting
	if ((glutGetModifiers() & GLUT_ACTIVE_SHIFT) == 0) angleRot = 0;
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': cam.z = 0; break;
	case 'a':
	case 'd': cam.x = 0; break;
	case 'q':
	case 'e': cam.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
bool bJustClicked = false;
void onMouse(int button, int state, int x, int y)
{
	bJustClicked = (state == GLUT_DOWN);
	glutSetCursor(bJustClicked ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
}

// handle mouse move
void onMotion(int x, int y)
{
	if (bJustClicked)
		bJustClicked = false;
	else
	{
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// find delta (change to) pan & tilt
		float deltaPan = 0.25f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
		float deltaTilt = 0.25f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// View = Tilt * DeltaPan * Tilt^-1 * DeltaTilt * View;
		angleTilt += deltaTilt;
		mat4 m = mat4(1.f);
		m = rotate(m, radians(angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaPan), vec3(0.f, 1.f, 0.f));
		m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaTilt), vec3(1.f, 0.f, 0.f));
		matrixView = m * matrixView;
	}
}

int main(int argc, char** argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("CI5520 3D Graphics Programming");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "GLEW Error: " << glewGetErrorString(err) << endl;
		return 0;
	}
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	// register callbacks
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);

	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Version: " << glGetString(GL_VERSION) << endl;

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		cerr << "Application failed to initialise" << endl;
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	done();

	return 1;
}
