#include "renderer3D.h"

GLuint v, f, p;

GLuint cubeVbo, cubeIbo;
GLsizei nbCubeElements;

GLuint texID;

float camAngle = 1.0;
float camHeight = 2.0;
float camZoom = 3.0;
float aspect = 1.0;

float mouse[2] = {0,0};
int rightClickState = 0;

void (*update)();

float cubeVerticies[] = {
	-1, -1, -1, 0.25, 0.334, //  0
	-1,  1, -1, 0.25, 0.6662, //  1
	-1,  1,  1, 0.5,  0.6662, //  2
	-1,  1,  1, 0.5,  0.6662, //  3
	-1, -1,  1, 0.5,  0.334, //  4
	-1, -1, -1, 0.25, 0.334, // 5 
	 1, -1, -1, 1,    0.334, //  6
	 1, -1,  1, 0.75, 0.334, // 7 
	 1,  1,  1, 0.75, 0.666, //  8
	 1,  1,  1, 0.75, 0.666, //  9
	 1,  1, -1, 1,    0.666, //  10
	 1, -1, -1, 1,    0.334, //  11
	-1, -1, -1, 0.25, 0.333, //12 
	-1, -1,  1, 0.5,  0.333, // 13
	 1, -1,  1, 0.5,  0, // 14
	 1, -1,  1, 0.5,  0, // 15
	 1, -1, -1, 0.25, 0, // 16
	-1, -1, -1, 0.25, 0.333, // 17 
	-1, -1,  1, 0.5,  0.334, //  18
	-1,  1,  1, 0.5,  0.666, //  19
	 1,  1,  1, 0.75, 0.666, //  20
	 1,  1,  1, 0.75, 0.666, //  21
	 1, -1,  1, 0.75, 0.334, // 22
	-1, -1,  1, 0.5,  0.334, // 23
	-1,  1,  1, 0.5,  0.667, //  24
	-1,  1, -1, 0.25, 0.667, //  25
	 1,  1, -1, 0.25, 1, //  26
	 1,  1, -1, 0.25, 1, //  27
	 1,  1,  1, 0.5,  1, //  28
	-1,  1,  1, 0.5,  0.667, //29  
	 1, -1, -1, 0,    0.333, //  30
	 1,  1, -1, 0,    0.666, //  31
	-1,  1, -1, 0.25, 0.666, //32 
	-1,  1, -1, 0.25, 0.666, // 33
	-1, -1, -1, 0.25, 0.333, // 34
	 1, -1, -1, 0,    0.333 // 35
};

unsigned int cubeIndicies[] = {
	0,  1,  2,
	3,  4,  5,
	6,  7,  8, 
	9,  10, 11, 
	12, 13, 14,
	15, 16, 17,
	18, 19, 20, 
	21, 22, 23, 
	24, 25, 26, 
	27, 28, 29, 
	30, 31, 32, 
	33, 34, 35, 36,
};

void changeSize(int w, int h) {
	if(h == 0)
		h = 1;

	aspect = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
}

void renderScene(void) {
	Mat4 projection, camera, model;
	Vec3 lookat = {0.0, 0.0, 0.0};
	Vec3 up = {0.0, 1.0, 0.0};
	Vec3 camLoc = {
		sin(camAngle)*camZoom,
		camHeight*camZoom,
		cos(camAngle)*camZoom
	};

	if (update != NULL) update();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// set buffers
	glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIbo);

	// set verticies attribute
	GLuint vertAttrib = glGetAttribLocation(p, "vert");
	glEnableVertexAttribArray(vertAttrib);
	glVertexAttribPointer(vertAttrib, 3, GL_FLOAT, 0, 5*4, (void*)0);

	// set texture coord attribute
	GLuint texCoordAttrib = glGetAttribLocation(p, "texCoord");
	glEnableVertexAttribArray(texCoordAttrib);
	glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, 0, 5*4, (void*)(3*4));

	Math_Perspective(projection, Math_DegToRad(CAMERA_FOV_Y), aspect, CAMERA_NEAR, CAMERA_FAR);
	GLint projectionLoc = glGetUniformLocation(p, "projection");
	glUniformMatrix4fv(projectionLoc, 1, 0, &projection[0]);

	Math_LookAtV(camera, camLoc, lookat, up);
	GLint cameraLoc = glGetUniformLocation(p, "camera");
	glUniformMatrix4fv(cameraLoc, 1, 0, &camera[0]);

	Math_Translate3D(model, 0.0, 0.0, 0.0);
	GLint modelLoc = glGetUniformLocation(p, "model");
	glUniformMatrix4fv(modelLoc, 1, 0, &model[0]);

	GLint cameraTranslationLoc = glGetUniformLocation(p, "cameraTranslation");
	glUniform3f(cameraTranslationLoc, camLoc[0], camLoc[1], camLoc[2]);

	glDrawElements(GL_TRIANGLES, nbCubeElements, GL_UNSIGNED_INT, (void*)0);

	glutSwapBuffers();
}

void processKeys(unsigned char key, int x, int y) {
	if (key == 113) {
		exit(0);
	}
}

void mouseClick(int button, int state, int x, int y) {
	mouse[0] = x;
	mouse[1] = y;
	rightClickState = button == 2;
}

void mouseMotion(int x, int y) {
	float delta[2] = {x-mouse[0], y-mouse[1]};
	if (rightClickState) {
		camZoom += CAMERA_SCROLL_SPEED*delta[1];
		if (camZoom < 0.001) camZoom = 0.001;
	} else {
		camAngle -= CAMERA_SPEED*delta[0];
		camHeight += CAMERA_SPEED*delta[1];
	}
	mouse[0] = x;
	mouse[1] = y;
}

void compileShader(GLuint shader) {
	glCompileShader(shader);
	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		int logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		char *logs = malloc(sizeof(char) * logLength);
		glGetShaderInfoLog(shader, logLength, NULL, logs);

		printf("failed to compile shader: %s", logs);
		free(logs);
	}
}

void setShaders() {
	char *vs = NULL,*fs = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("shaders/tracer.vert");
	fs = textFileRead("shaders/tracer.frag");

	const char * ff = fs;
	const char * vv = vs;

	glShaderSource(v, 1, &vv,NULL);
	glShaderSource(f, 1, &ff,NULL);

	free(vs);free(fs);

	compileShader(v);
	compileShader(f);

	p = glCreateProgram();
	glAttachShader(p,f);
	glAttachShader(p,v);

	glLinkProgram(p);
	glUseProgram(p);

	glBindFragDataLocation(p, 0, "outputColor");
}

GLuint loadTexture(Image img, GLuint textureUnit) {
	GLuint texId;
	glGenTextures(1, &texId);
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_3D, texId);
	glTexImage3D(
		GL_TEXTURE_3D,
		0,
		GL_RGBA,
		img.width,
		img.height,
		img.depth,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		img.data
	);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return texId;
}

GLuint loadVBO(Geometry geometry) {
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, geometry.nbVerticies*4, geometry.verticies, GL_STATIC_DRAW);
	return vbo;
}

GLuint loadIBO(Geometry geometry) {
	GLuint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry.nbVerticies*4, geometry.indicies, GL_STATIC_DRAW);
	return ibo;
}

void setImage(Image img) {
	glDeleteTextures(1, &texID);

	GLint textureLoc = glGetUniformLocation(p, "tex");
	glProgramUniform1i(p, textureLoc , 0);
	texID = loadTexture(img, GL_TEXTURE0);

	GLint marchLoc = glGetUniformLocation(p, "march");
	glUniform1f(marchLoc, 1.0/img.width);
	GLint detailLoc = glGetUniformLocation(p, "detail");
	glUniform1f(detailLoc, img.width/RAY_RESOLUTION);
}

void initCube() {
	Geometry cube;
	cube.indicies = &cubeIndicies[0];
	cube.verticies = &cubeVerticies[0];
	cube.nbIndicies = sizeof(cubeIndicies);
	cube.nbVerticies = sizeof(cubeVerticies);

	cubeVbo = loadVBO(cube);
	cubeIbo = loadIBO(cube);
	nbCubeElements = cube.nbIndicies;
}

void onUpdate(void (*fn)()) {
	update = fn;
}

void initRenderer(void) {
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processKeys);
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouseClick);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0, 0.0, 0.1, 1.0);
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	setShaders();
	initCube();
}