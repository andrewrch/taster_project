#include <stdlib.h>
#include <GL/glut.h>

// Location/Normals
#define X_POS 0
#define Y_POS 1
#define Z_POS 2

// Texture Coordinates
#define U_POS 0
#define V_POS 1

// Colours
#define R_POS 0
#define G_POS 1
#define B_POS 2
#define A_POS 3

// Vertex structure
typedef struct
{
	GLfloat location[3];
	GLfloat tex[2];
	GLfloat normal[3];
	GLfloat colour[4];
	GLubyte padding[16];
} Vertex;

// A helper macro to get a position
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// GLUT functions
void renderScene();
void changeSize(int w, int h);
void processNormalKeys(unsigned char key, int x, int y);

// VBO functions
void initVBO();

// Other functions
void buildCube();

// Globals - typically these would be in classes, 
// but in this simple example, I'm using globals
Vertex verts[24]; // We're making a cube, 6 faces * 4 verticies per face
GLubyte index[36]; // 2 Triangles per face (possible to use quads, but they're being phased out of OpenGL3, so we're using triangles instead)
GLuint vboID; // Vertex Buffer Object ID
GLuint indexVBOID; // Index Buffer Object ID
float angle = 0.0; // Just for display rotation

int main (int argc, char * argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("VBO Example");
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);
	glEnable(GL_DEPTH_TEST);
	buildCube();
	initVBO();
	glutMainLoop();
    return 0;
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotatef(angle, 1.0, 1.0, 1.0);

	// Bind our buffers much like we would for texturing
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
	
	// Set the state of what we are drawing (I don't think order matters here, but I like to do it in the same 
	// order I set the pointers
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	// Resetup our pointers.  This doesn't reinitialise any data, only how we walk through it
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
	glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
	glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
	
	// Actually do our drawing, parameters are Primative (Triangles, Quads, Triangle Fans etc), Elements to 
	// draw, Type of each element, Start Offset
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	
	// Disable our client state back to normal drawing
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glPopMatrix();
	glutSwapBuffers();
	angle += 0.01;
}

void changeSize(int w, int h)
{
	if (h == 0) // Prevent divide by zero
		h = 1;
	
	float ratio = 1.0 * w / h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glViewport(0, 0, w, h);
	
	gluPerspective(45, ratio, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0,
			  0.0, 0.0, -1.0,
			  0.0, 1.0, 0.0);
	
}

void processNormalKeys(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
}

void initVBO()
{
	glGenBuffers(1, &vboID); // Create the buffer ID, this is basically the same as generating texture ID's
	glBindBuffer(GL_ARRAY_BUFFER, vboID); // Bind the buffer (vertex array data)
	
	// Allocate space.  We could pass the mesh in here (where the NULL is), but it's actually faster to do it as a 
	// seperate step.  We also define it as GL_STATIC_DRAW which means we set the data once, and never 
	// update it.  This is not a strict rule code wise, but gives hints to the driver as to where to store the data	
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 24, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 24, verts);
	
	// Set the pointers to our data.  Except for the normal value (which always has a size of 3), we must pass 
	// the size of the individual component.  ie. A vertex has 3 points (x, y, z), texture coordinates have 2 (u, v) etc.
	// Basically the arguments are (ignore the first one for the normal pointer), Size (many components to 
	// read), Type (what data type is it), Stride (how far to move forward - in bytes - per vertex) and Offset 
	// (where in the buffer to start reading the data - in bytes)
	
	// Make sure you put glVertexPointer at the end as there is a lot of work that goes on behind the scenes
	// with it, and if it's set at the start, it has to do all that work for each gl*Pointer call, rather than once at
	// the end.	
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
	glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
	glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
	
	// When we get here, all the vertex data is effectively on the card
	
	glGenBuffers(1, &indexVBOID); // Generate buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID); // Bind the element array buffer
	// Upload the index array, this can be done the same way as above (with NULL as the data, then a 
	// glBufferSubData call, but doing it all at once for convenience)	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLubyte), index, GL_STATIC_DRAW);
}

void buildCube()
{
	// Ugh, ugly code this
	
	// Top
	verts[0].location[X_POS]	= 1;	verts[0].location[Y_POS]	= 1;	verts[0].location[Z_POS]	= -1;
	verts[0].normal[X_POS]		= 0;	verts[0].normal[Y_POS]		= 1;	verts[0].normal[Z_POS]		= 0;
	verts[0].tex[U_POS]			= 0;	verts[0].tex[V_POS]			= 0; 
	
	verts[1].location[X_POS]	= -1;	verts[1].location[Y_POS]	= 1;	verts[1].location[Z_POS]	= -1;
	verts[1].normal[X_POS]		= 0;	verts[1].normal[Y_POS]		= 1;	verts[1].normal[Z_POS]		= 0;
	verts[1].tex[U_POS]			= 0;	verts[1].tex[V_POS]			= 1; 
	
	verts[2].location[X_POS]	= -1;	verts[2].location[Y_POS]	= 1;	verts[2].location[Z_POS]	= 1;
	verts[2].normal[X_POS]		= 0;	verts[2].normal[Y_POS]		= 1;	verts[2].normal[Z_POS]		= 0;
	verts[2].tex[U_POS]			= 1;	verts[2].tex[V_POS]			= 1; 
	
	verts[3].location[X_POS]	= 1;	verts[3].location[Y_POS]	= 1;	verts[3].location[Z_POS]	= 1;
	verts[3].normal[X_POS]		= 0;	verts[3].normal[Y_POS]		= 1;	verts[3].normal[Z_POS]		= 0;
	verts[3].tex[U_POS]			= 1;	verts[3].tex[V_POS]			= 0;
	
	// Bottom
	verts[4].location[X_POS]	= 1;	verts[4].location[Y_POS]	= -1;	verts[4].location[Z_POS]	= 1;
	verts[4].normal[X_POS]		= 0;	verts[4].normal[Y_POS]		= -1;	verts[4].normal[Z_POS]		= 0;
	verts[4].tex[U_POS]			= 0;	verts[4].tex[V_POS]			= 0; 
	
	verts[5].location[X_POS]	= -1;	verts[5].location[Y_POS]	= -1;	verts[5].location[Z_POS]	= 1;
	verts[5].normal[X_POS]		= 0;	verts[5].normal[Y_POS]		= -1;	verts[5].normal[Z_POS]		= 0;
	verts[5].tex[U_POS]			= 0;	verts[5].tex[V_POS]			= 1; 
	
	verts[6].location[X_POS]	= -1;	verts[6].location[Y_POS]	= -1;	verts[6].location[Z_POS]	= -1;
	verts[6].normal[X_POS]		= 0;	verts[6].normal[Y_POS]		= -1;	verts[6].normal[Z_POS]		= 0;
	verts[6].tex[U_POS]			= 1;	verts[6].tex[V_POS]			= 1; 
	
	verts[7].location[X_POS]	= 1;	verts[7].location[Y_POS]	= -1;	verts[7].location[Z_POS]	= -1;
	verts[7].normal[X_POS]		= 0;	verts[7].normal[Y_POS]		= -1;	verts[7].normal[Z_POS]		= 0;
	verts[7].tex[U_POS]			= 1;	verts[7].tex[V_POS]			= 0;
	
	// Front
	verts[8].location[X_POS]	= 1;	verts[8].location[Y_POS]	= 1;	verts[8].location[Z_POS]	= 1;
	verts[8].normal[X_POS]		= 0;	verts[8].normal[Y_POS]		= 0;	verts[8].normal[Z_POS]		= 1;
	verts[8].tex[U_POS]			= 0;	verts[8].tex[V_POS]			= 0; 
	
	verts[9].location[X_POS]	= -1;	verts[9].location[Y_POS]	= 1;	verts[9].location[Z_POS]	= 1;
	verts[9].normal[X_POS]		= 0;	verts[9].normal[Y_POS]		= 0;	verts[9].normal[Z_POS]		= 1;
	verts[9].tex[U_POS]			= 0;	verts[9].tex[V_POS]			= 1; 
	
	verts[10].location[X_POS]	= -1;	verts[10].location[Y_POS]	= -1;	verts[10].location[Z_POS]	= 1;
	verts[10].normal[X_POS]		= 0;	verts[10].normal[Y_POS]		= 0;	verts[10].normal[Z_POS]		= 1;
	verts[10].tex[U_POS]		= 1;	verts[10].tex[V_POS]		= 1; 
	
	verts[11].location[X_POS]	= 1;	verts[11].location[Y_POS]	= -1;	verts[11].location[Z_POS]	= 1;
	verts[11].normal[X_POS]		= 0;	verts[11].normal[Y_POS]		= 0;	verts[11].normal[Z_POS]		= 1;
	verts[11].tex[U_POS]		= 1;	verts[11].tex[V_POS]		= 0;

	// Back
	verts[12].location[X_POS]	= 1;	verts[12].location[Y_POS]	= -1;	verts[12].location[Z_POS]	= -1;
	verts[12].normal[X_POS]		= 0;	verts[12].normal[Y_POS]		= 0;	verts[12].normal[Z_POS]		= -1;
	verts[12].tex[U_POS]		= 0;	verts[12].tex[V_POS]		= 0; 
	
	verts[13].location[X_POS]	= -1;	verts[13].location[Y_POS]	= -1;	verts[13].location[Z_POS]	= -1;
	verts[13].normal[X_POS]		= 0;	verts[13].normal[Y_POS]		= 0;	verts[13].normal[Z_POS]		= -1;
	verts[13].tex[U_POS]		= 0;	verts[13].tex[V_POS]		= 1; 
	
	verts[14].location[X_POS]	= -1;	verts[14].location[Y_POS]	= 1;	verts[14].location[Z_POS]	= -1;
	verts[14].normal[X_POS]		= 0;	verts[14].normal[Y_POS]		= 0;	verts[14].normal[Z_POS]		= -1;
	verts[14].tex[U_POS]		= 1;	verts[14].tex[V_POS]		= 1; 
	
	verts[15].location[X_POS]	= 1;	verts[15].location[Y_POS]	= 1;	verts[15].location[Z_POS]	= -1;
	verts[15].normal[X_POS]		= 0;	verts[15].normal[Y_POS]		= 0;	verts[15].normal[Z_POS]		= -1;
	verts[15].tex[U_POS]		= 1;	verts[15].tex[V_POS]		= 0;

	// Left
	verts[16].location[X_POS]	= -1;	verts[16].location[Y_POS]	= 1;	verts[16].location[Z_POS]	= 1;
	verts[16].normal[X_POS]		= -1;	verts[16].normal[Y_POS]		= 0;	verts[16].normal[Z_POS]		= 0;
	verts[16].tex[U_POS]		= 0;	verts[16].tex[V_POS]		= 0; 
	
	verts[17].location[X_POS]	= -1;	verts[17].location[Y_POS]	= 1;	verts[17].location[Z_POS]	= -1;
	verts[17].normal[X_POS]		= -1;	verts[17].normal[Y_POS]		= 0;	verts[17].normal[Z_POS]		= 0;
	verts[17].tex[U_POS]		= 0;	verts[17].tex[V_POS]		= 1; 
	
	verts[18].location[X_POS]	= -1;	verts[18].location[Y_POS]	= -1;	verts[18].location[Z_POS]	= -1;
	verts[18].normal[X_POS]		= -1;	verts[18].normal[Y_POS]		= 0;	verts[18].normal[Z_POS]		= 0;
	verts[18].tex[U_POS]		= 1;	verts[18].tex[V_POS]		= 1; 
	
	verts[19].location[X_POS]	= -1;	verts[19].location[Y_POS]	= -1;	verts[19].location[Z_POS]	= 1;
	verts[19].normal[X_POS]		= -1;	verts[19].normal[Y_POS]		= 0;	verts[19].normal[Z_POS]		= 0;
	verts[19].tex[U_POS]		= 1;	verts[19].tex[V_POS]		= 0;

	// Right
	verts[20].location[X_POS]	= 1;	verts[20].location[Y_POS]	= 1;	verts[20].location[Z_POS]	= -1;
	verts[20].normal[X_POS]		= 1;	verts[20].normal[Y_POS]		= 0;	verts[20].normal[Z_POS]		= 0;
	verts[20].tex[U_POS]		= 0;	verts[20].tex[V_POS]		= 0; 
	
	verts[21].location[X_POS]	= 1;	verts[21].location[Y_POS]	= 1;	verts[21].location[Z_POS]	= 1;
	verts[21].normal[X_POS]		= 1;	verts[21].normal[Y_POS]		= 0;	verts[21].normal[Z_POS]		= 0;
	verts[21].tex[U_POS]		= 0;	verts[21].tex[V_POS]		= 1; 
	
	verts[22].location[X_POS]	= 1;	verts[22].location[Y_POS]	= -1;	verts[22].location[Z_POS]	= 1;
	verts[22].normal[X_POS]		= 1;	verts[22].normal[Y_POS]		= 0;	verts[22].normal[Z_POS]		= 0;
	verts[22].tex[U_POS]		= 1;	verts[22].tex[V_POS]		= 1; 
	
	verts[23].location[X_POS]	= 1;	verts[23].location[Y_POS]	= -1;	verts[23].location[Z_POS]	= -1;
	verts[23].normal[X_POS]		= 1;	verts[23].normal[Y_POS]		= 0;	verts[23].normal[Z_POS]		= 0;
	verts[23].tex[U_POS]		= 1;	verts[23].tex[V_POS]		= 0;
	
	// Colours
	for (int i = 0; i < 24; i++)
	{
		verts[i].colour[R_POS] = 1.0;
		verts[i].colour[G_POS] = 1.0;
		verts[i].colour[B_POS] = 1.0;
		verts[i].colour[A_POS] = 1.0;
	}
	
	// Index Array (define our triangles)
	// A Face looks like (numbers are the array index number of the vertex)
	// 1      2
	// +------+
	// |      |
	// |      |
	// +------+
	// 0      3
	index[0] = 0;	index[1] = 1;	index[2] = 2;
	index[3] = 2;	index[4] = 3;	index[5] = 0;
	
	index[6] = 4;	index[7] = 5;	index[8] = 6;
	index[9] = 6;	index[10] = 7;	index[11] = 4;
	
	index[12] = 8;	index[13] = 9;	index[14] = 10;
	index[15] = 10;	index[16] = 11;	index[17] = 8;
	
	index[18] = 12;	index[19] = 13;	index[20] = 14;
	index[21] = 14;	index[22] = 15;	index[23] = 12;
	
	index[24] = 16;	index[25] = 17;	index[26] = 18;
	index[27] = 18;	index[28] = 19;	index[29] = 16;
	
	index[30] = 20;	index[31] = 21;	index[32] = 22;
	index[33] = 22;	index[34] = 23;	index[35] = 20;
	
}
