#include <Windows.h>	// using Microsoft Windows library
#include <gl/GL.h>		// OpenGL Library (To interact directly with to GPU, faster) [Code <-> OpenGL (Abstraction on top of many different GPUs) <-> GPU]
#include <math.h>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <gl/GLU.h>
#include <cmath>

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")

#define CLASS_TITLE "OpenGL Window Class"
#define WINDOW_TITLE "NeZha Prototype"

struct Color
{
	float r;
	float g;
	float b;
};

struct ColorList
{
	std::vector<Color> colors;
};

enum CameraMode {
	ORTHO,
	PERSPECTIVE,
	BRIDGE_VIEW,    // fixed point around the bridge
	THIRD_PERSON    // follows the car
};

CameraMode currentCameraMode = PERSPECTIVE;

// global system values
int questionNum = 1;

// Question 1
// Camera
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 0.0f;
float movementFactor = 0.1f;

float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;
float cameraAngleZ = 0.0f;
float rotationFactor = 2.0f;

// Light
float lightX = 0.0f;
float lightY = 0.0f;
float lightZ = 0.0f;

// Textures [Q2]
std::vector<GLuint> textures;
int currentTextureIndex = 0;
GLuint currentTexture;

// -------------------
// Lighting Setup
// -------------------
GLfloat lightAmbient[] = { 0.7f, 0.7f, 0.7f, 1.0f };	// ambient = minimum brightness of scene
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// diffuse = real lighting that reveals geometry
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// specular = how glossy the surface looks

// -------------------
// Material Setup
// -------------------
GLfloat defaultAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat defaultDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat defaultSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat defaultShininess = 0.0f;

// Red beans
GLfloat redBean_matAmbient[] = { 0.13f, 0.01f, 0.02f, 1.0f };
GLfloat redBean_matDiffuse[] = { 0.25f, 0.15f, 0.1f, 1.0f };
GLfloat redBean_matSpecular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat redBean_shininess = 60.0f;

// Chocolate chips
GLfloat chocoChip_matAmbient[] = { 0.1f, 0.02f, 0.0f, 1.0f };
GLfloat chocoChip_matDiffuse[] = { 0.15f, 0.12f, 0.15f, 1.0f };
GLfloat chocoChip_matSpecular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat chocoChip_shininess = 60.0f;

// Cherry Stem
GLfloat cherryStem_matAmbient[] = { 0.2f, 0.1f, 0.05f, 1.0f };
GLfloat cherryStem_matDiffuse[] = { 0.39f, 0.26f, 0.13f, 1.0f };
GLfloat cherryStem_matSpecular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat cherryStem_shininess = 5.0f;

// -------------------
// GLU Quadric Objects
// -------------------
GLUquadricObj* quadric = gluNewQuadric();
GLUquadric* skyQuadric = gluNewQuadric();

// -------------------
// Textures Setup
// -------------------
// COMMON TEXTURES
GLuint brickTexture;
GLuint steelTexture;
GLuint woodTexture;
GLuint boxTexture;
GLuint goldTexture;
GLuint silverTexture;

// ------------------
// ICE CREAM TEXTURES
// ------------------
// Cream Textures
GLuint matchaIceCreamTexture;
GLuint chocolateIceCreamTexture;
GLuint vanillaIceCreamTexture;
GLuint cookieCreamTexture;
// Cookies
GLuint coneTexture;
GLuint oreoCookieTexture;
GLuint waffleTexture;
// Decoration
GLuint cherryTexture;

// Environment Textures
GLuint skyTexture;
GLuint seaTexture;

// ---------
// Constants
// ---------
const int SLICES = 50;
const int STACKS = 50;
const int LOOPS = 50;

// ---------
const float GLU_ANGLEY_OFFSET = -90.0f;

const float MIN_ANGLE = 0.0f;
const float MAX_ANGLE = 360.0f;

const int MIN_SLICES = 10;
const int MAX_SLICES = 50;
const int MIN_STACKS = 10;
const int MAX_STACKS = 50;

const float WORLD_TOP = 1.0f;
const float WORLD_LEFT = -1.0f;
const float WORLD_BOTTOM = -1.0f;
const float WORLD_RIGHT = 1.0f;

const float GRAVITY = 0.01f;

void ResetCameraPosition()
{
	cameraX = 0.0f;
	cameraY = 0.0f;
	cameraZ = 0.0f;
}

void ResetCameraAngle()
{
	cameraAngleX = 0.0f;
	cameraAngleY = 0.0f;
	cameraAngleZ = 0.0f;
}

void ResetLightPosition()
{
	lightX = 0.0f;
	lightY = 0.0f;
	lightZ = 0.0f;
}

void SetTexture(GLuint tex)
{
	currentTexture = tex;
}

void UpdateCurrentTexture()
{
	if (!textures.empty())
	{
		currentTexture = textures[currentTextureIndex];
	}
}

// UINT = Unsigned integer e.g. Mouse Moved (Like email title)
// WPARAM, LPARAM = Parameters
LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CREATE:
		SetTimer(hWnd, 1, 30, NULL); // 30ms (~33 FPS animation)
		break;

	case WM_TIMER:
		InvalidateRect(hWnd, NULL, FALSE); // redraw
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;

		case 0x30: // Press 0 - DEBUG
			cameraY = -90.0f;
			break;

		case 0x31: // Press 1 - Draw Pyramid
			break;

			// --------------
			// CAMERA CONTROL
			// --------------
		case VK_LEFT:
			cameraAngleY -= rotationFactor;
			break;
		case VK_RIGHT:
			cameraAngleY += rotationFactor;
			break;
		case VK_UP:
			cameraAngleX -= rotationFactor;
			break;
		case VK_DOWN:
			cameraAngleX += rotationFactor;
			break;

		case 0x57: // W
			cameraZ -= movementFactor;
			break;
		case 0x53: // S
			cameraZ += movementFactor;
			break;
		case 0x41: // A
			cameraX -= movementFactor;
			break;
		case 0x44: // D
			cameraX += movementFactor;
			break;
		case 0x51: // Q
			cameraY -= movementFactor;
			break;
		case 0x45: // E
			cameraY += movementFactor;
			break;

			// --------------
			// LIGHT CONTROL
			// --------------
		case 0x55: // To move light forward [U]
			lightZ -= movementFactor;
			break;
		case 0x4A: // To move light backward [J]
			lightZ += movementFactor;
			break;
		case 0x48: // To move light left [H]
			lightX -= movementFactor;
			break;
		case 0x4B: // To move light right [K]
			lightX += movementFactor;
			break;
		case 0x59: // To move light down [Y]
			lightY -= movementFactor;
			break;
		case 0x49: // To move light up [I]
			lightY += movementFactor;
			break;


		case VK_SPACE: // To reset camera and light position and angle
			ResetCameraPosition();
			ResetCameraAngle();
			ResetLightPosition();
			break;
		}

		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

// HDC = Handler to pull a memory (Pointer) [Handle to Display context]
// Display Context = piece of memory that stores what u want to show on screen
bool InitPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}

GLuint LoadTexture(const char* filePath)
{
	BITMAP bmp;
	HBITMAP hBMP;

	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), filePath, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	GetObject(hBMP, sizeof(bmp), &bmp);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);

	DeleteObject(hBMP); // Clean up the bitmap handle

	return textureID;
}

void LoadCommonTextures()
{
	// ===============
	// COMMON TEXTURES
	// ===============
	brickTexture = LoadTexture("Assets/Common/Brick.bmp");
	steelTexture = LoadTexture("Assets/Common/Steel.bmp");
	woodTexture = LoadTexture("Assets/Common/Wood.bmp");
	boxTexture = LoadTexture("Assets/Common/Box.bmp");
	goldTexture = LoadTexture("Assets/Common/Gold.bmp");
	silverTexture = LoadTexture("Assets/Common/Silver.bmp");
}

void LoadIceCreamTextures()
{
	// ==================
	// ICE CREAM TEXTURES
	// ==================
	// Cream Textures
	matchaIceCreamTexture = LoadTexture("Assets/IceCream/MatchaIceCream.bmp");
	chocolateIceCreamTexture = LoadTexture("Assets/IceCream/ChocolateIceCream.bmp");
	vanillaIceCreamTexture = LoadTexture("Assets/IceCream/VanillaIceCream.bmp");
	cookieCreamTexture = LoadTexture("Assets/IceCream/CookieCream.bmp");

	// Cookies
	coneTexture = LoadTexture("Assets/IceCream/Cone.bmp");
	oreoCookieTexture = LoadTexture("Assets/IceCream/OreoCookie.bmp");
	waffleTexture = LoadTexture("Assets/IceCream/Waffle.bmp");

	// Decoration
	cherryTexture = LoadTexture("Assets/IceCream/Cherry.bmp");
}

void LoadEnvironmentTextures()
{
	skyTexture = LoadTexture("Assets/Environment/Sky.bmp");
	seaTexture = LoadTexture("Assets/Environment/Sea.bmp");
}

void InitTextures()
{
	glEnable(GL_TEXTURE_2D);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // 4 byte alignment = r g b a (32 bits = 4 bytes)

	LoadCommonTextures();

	LoadIceCreamTextures();

	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricNormals(quadric, GLU_SMOOTH);
}

// ************
//	2D SHAPES
// ************

float DegreeToRadian(float degree)
{
	return (3.142f / 180.0f) * degree;
}

float RadianToDegree(float radian)
{
	return (180.0f / 3.142f) * radian;
}

void SetFaceNormal(
	float ax, float ay, float az,
	float bx, float by, float bz,
	float cx, float cy, float cz
)
{
	float ux = bx - ax;
	float uy = by - ay;
	float uz = bz - az;

	float vx = cx - ax;
	float vy = cy - ay;
	float vz = cz - az;

	// Cross Product of U x V = a vector perpendicular to both U and V
	float nx = uy * vz - uz * vy;
	float ny = uz * vx - ux * vz;
	float nz = ux * vy - uy * vx;

	// normalize (Pythagoras)
	float length = sqrt(nx * nx + ny * ny + nz * nz);
	if (length != 0.0f)
	{
		nx /= length;
		ny /= length;
		nz /= length;
	}

	glNormal3f(nx, ny, nz);
}

void DrawRectangle(float width, float height)
{
	// To center the rectangle at the center origin
	float w = width / 2.0f;
	float h = height / 2.0f;
	float z = 0.0f;

	glVertex3f(-w, h, z);
	glVertex3f(-w, -h, z);
	glVertex3f(w, -h, z);
	glVertex3f(w, h, z);
}

void DrawEquilateralTriangle(float width, float height)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float z = 0.0f;

	glTexCoord2f(0.0f, 0.5f); glVertex3f(0.0f, h, z);		// top edge
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, z);		// left edge
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, z);		// right edge
}

void DrawRightTriangleWithColor(float width, float height, Color c1, Color c2, Color c3)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float z = 0.0f;

	glColor3f(c1.r, c1.g, c1.b);
	glVertex3f(-w, h, z);	// top edge
	glColor3f(c2.r, c2.g, c2.b);
	glVertex3f(-w, -h, z);	// left edge
	glColor3f(c3.r, c3.g, c3.b);
	glVertex3f(w, -h, z);	// right edge
}

void DrawRightTriangle(float width, float height)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float z = 0.0f;

	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, z);	// top edge
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, z);	// left edge
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, z);	// right edge
}

void DrawIsocelesTriangleWithColor_RightTriangle(float width, float height, Color c1, Color c2, Color c3)
{
	float w = width / 2.0f;

	glPushMatrix();
	glTranslatef(w, 0.0f, 0.0f);
	glBegin(GL_POLYGON);
	DrawRightTriangleWithColor(width, height, c1, c2, c3);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-w, 0.0f, 0.0f);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	DrawRightTriangleWithColor(width, height, c1, c2, c3);
	glEnd();
	glPopMatrix();
}

void DrawDiamondWithColor_IsocelesTriangle(float width, float height, Color c1, Color c2, Color c3)
{
	float h = height / 2.0f;

	glPushMatrix();
	glTranslatef(0.0f, h, 0.0f);
	DrawIsocelesTriangleWithColor_RightTriangle(width, height, c1, c2, c3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, -h, 0.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	DrawIsocelesTriangleWithColor_RightTriangle(width, height, c1, c2, c3);
	glPopMatrix();
}

void DrawIsocelesTriangle(float width, float height)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float z = 0.0f;

	glVertex3f(0.0f, h, z);		// top edge
	glVertex3f(-w, -h, z);		// left edge
	glVertex3f(w, -h, z);		// right edge
}

void DrawCircle(float radius, int angle)
{
	float z = 0.0f;

	for (int i = 0; i < angle; i += 10)
	{
		float tempX = cos(DegreeToRadian(i)) * radius;
		float tempY = sin(DegreeToRadian(i)) * radius;
		glVertex3f(tempX, tempY, z);
	}
}

void DrawSemiCircle(float radius)
{
	float z = 0.0f;

	//glVertex3f(centerX, centerY, z);

	for (int i = 0; i <= 180; i += 10)
	{
		float tempX = cos(DegreeToRadian(i)) * radius;
		float tempY = sin(DegreeToRadian(i)) * radius;
		glVertex3f(tempX, tempY, z);
	}
}

void DrawArc(float radius, float angle)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	glVertex3f(x, y, z);

	for (int i = 0; i <= angle; i += 10)
	{
		float tempX = cos(DegreeToRadian(i)) * radius;
		float tempY = sin(DegreeToRadian(i)) * radius;
		glVertex3f(tempX, tempY, z);
	}
}

void DrawArcWithCurveOnly(float radius, float angle)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	glBegin(GL_LINE_STRIP);

	for (int i = 0; i <= angle; i += 1)
	{
		float tempX = cos(DegreeToRadian(i)) * radius;
		float tempY = sin(DegreeToRadian(i)) * radius;
		glVertex3f(tempX, tempY, z);
	}

	glEnd();
}

void DrawPentagon(float radius)
{
	float z = 0.0f;

	if (radius > 1) return;
	for (int i = 0; i < 5; i++)
	{
		float angle = 72.0f * i + 90.0f; // 360 / 5 = 72 (each angle is 72deg)
		float tempX = cos(DegreeToRadian(angle)) * radius;
		float tempY = sin(DegreeToRadian(angle)) * radius;

		glVertex3f(tempX, tempY, z);
	}
}

void DrawLineWithColor(float length, float width, Color c)
{
	float z = 0.0f;

	glColor3f(c.r, c.g, c.b);
	glLineWidth(width);
	glBegin(GL_LINES);
	glVertex3f(-length / 2.0f, 0.0f, z);
	glVertex3f(length / 2.0f, 0.0f, z);
	glEnd();
}

// ***********
//	3D Shapes
// ***********

void DrawCuboidPolygon(float width, float height, float depth)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float d = depth / 2.0f;

	// FRONT
	glNormal3f(0, 0, 1);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, d);
	glEnd();

	// RIGHT
	glNormal3f(1, 0, 0);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(w, h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, -d);
	glEnd();

	// BACK
	glNormal3f(0, 0, -1);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(w, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, -d);
	glEnd();

	// LEFT
	glNormal3f(-1, 0, 0);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, d);
	glEnd();

	// TOP
	glNormal3f(0, 1, 0);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, -d);
	glEnd();

	// BOTTOM
	glNormal3f(0, -1, 0);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, -h, d);
	glEnd();
}

void DrawCuboidLines(float width, float height, float depth)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float d = depth / 2.0f;

	// Front face
	glBegin(GL_LINE_LOOP);
	glVertex3f(-w, h, d);
	glVertex3f(-w, -h, d);
	glVertex3f(w, -h, d);
	glVertex3f(w, h, d);
	glEnd();

	// Right face
	glBegin(GL_LINE_LOOP);
	glVertex3f(w, h, d);
	glVertex3f(w, -h, d);
	glVertex3f(w, -h, -d);
	glVertex3f(w, h, -d);
	glEnd();

	// Back face
	glBegin(GL_LINE_LOOP);
	glVertex3f(w, h, -d);
	glVertex3f(w, -h, -d);
	glVertex3f(-w, -h, -d);
	glVertex3f(-w, h, -d);
	glEnd();

	// Left face
	glBegin(GL_LINE_LOOP);
	glVertex3f(-w, h, -d);
	glVertex3f(-w, -h, -d);
	glVertex3f(-w, -h, d);
	glVertex3f(-w, h, d);
	glEnd();

	// Top face
	glBegin(GL_LINE_LOOP);
	glVertex3f(-w, h, -d);
	glVertex3f(-w, h, d);
	glVertex3f(w, h, d);
	glVertex3f(w, h, -d);
	glEnd();

	// Bottom face
	glBegin(GL_LINE_LOOP);
	glVertex3f(-w, -h, -d);
	glVertex3f(-w, -h, d);
	glVertex3f(w, -h, d);
	glVertex3f(w, -h, -d);
	glEnd();
}

void DrawPrism(float width, float height, float depth)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float d = depth / 2.0f;

	// Front face
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, d);
	glBegin(GL_TRIANGLES);
	SetFaceNormal(
		0.0f, h, d,
		-w, -h, d,
		w, -h, d
	);
	DrawEquilateralTriangle(width, height);
	glEnd();
	glPopMatrix();

	// Right face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		w, h, 0.0f,   // top front right
		w, -h, d,   // bottom front right
		w, -h, -d    // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, h, -d);
	glEnd();
	glPopMatrix();

	// Back face
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -d);
	glBegin(GL_TRIANGLES);
	SetFaceNormal(
		0.0f, h, -d,
		-w, -h, -d,
		w, -h, -d
	);
	DrawEquilateralTriangle(width, height);
	glEnd();
	glPopMatrix();

	// Left face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		-w, h, 0.0f,   // top front right
		-w, -h, -d,   // bottom front right
		-w, -h, d    // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, h, d);
	glEnd();
	glPopMatrix();

	// Bottom face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();
}

void DrawPrismWithOffset(float width, float height, float depth, float topOffset)
{
	if (topOffset > width) return;

	float w = width / 2.0f;
	float h = height / 2.0f;
	float d = depth / 2.0f;

	float offsetZ = (depth - topOffset) / 2.0f;

	// Front face
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	SetFaceNormal(
		0.0f, h, d - offsetZ,
		-w, -h, d,
		w, -h, d
	);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, h, d - offsetZ);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();

	// Right face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		w, h, 0.0f,   // top front right
		w, -h, d,   // bottom front right
		w, -h, -d    // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, h, d - offsetZ);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, h, -(d - offsetZ));
	glEnd();
	glPopMatrix();

	// Back face
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	SetFaceNormal(
		0.0f, h, -(d - offsetZ),
		w, -h, -d,
		-w, -h, -d
	);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, h, -(d - offsetZ));
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, -d);
	glEnd();
	glPopMatrix();

	// Left face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		-w, h, 0.0f,   // top front right
		-w, -h, -d,   // bottom front right
		-w, -h, d    // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, h, -(d - offsetZ));
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, h, d - offsetZ);
	glEnd();
	glPopMatrix();

	// Bottom face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();
}

void DrawRightTriangularPrism(float width, float height, float depth)
{
	float w = width / 2.0f;
	float h = height / 2.0f;
	float d = depth / 2.0f;

	// Front face
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	glTranslatef(0.0f, 0.0f, d);
	SetFaceNormal(
		0.0f, h, d,
		-w, -h, d,
		w, -h, d
	);
	DrawRightTriangle(width, height);
	glEnd();
	glPopMatrix();

	// Right / Top face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		w, h, 0.0f,   // top front right
		w, -h, d,   // bottom front right
		w, -h, -d    // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, -d);
	glEnd();
	glPopMatrix();

	// Back face
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	glTranslatef(0.0f, 0.0f, -d);
	SetFaceNormal(
		0.0f, h, -d,
		w, -h, -d,
		-w, -h, -d
	);
	DrawRightTriangle(width, height);
	glEnd();
	glPopMatrix();

	// Left face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, d);
	glEnd();
	glPopMatrix();

	// Bottom face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();
}

void DrawTrapezoidalPrism(float width, float height, float depth, float topOffset)
{
	float w = width / 2;
	float h = height / 2;
	float d = depth / 2;
	float t = topOffset / 2;

	// Front face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-t, h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(t, h, d);
	glEnd();
	glPopMatrix();

	// Right face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		t, h, 0.0f,   // top front right
		w, -h, d,   // bottom front right
		w, -h, -d    // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(t, h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(t, h, -d);
	glEnd();
	glPopMatrix();

	// Back face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(t, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-t, h, -d);
	glEnd();
	glPopMatrix();

	// Left face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		-t, h, 0.0f,   // top front right
		-w, -h, -d,   // bottom front right
		-w, -h, d    // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-t, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-t, h, d);
	glEnd();
	glPopMatrix();

	// Top face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-t, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-t, h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(t, h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(t, h, -d);
	glEnd();
	glPopMatrix();

	// Bottom face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();
}

void DrawRightTrapezoidalPrism(float width, float height, float depth, float topOffset)
{
	float w = width / 2;
	float h = height / 2;
	float d = depth / 2;
	float t = topOffset / 2;

	// Front face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(t, h, d);
	glEnd();
	glPopMatrix();

	// Right face
	glPushMatrix();
	glBegin(GL_QUADS);
	SetFaceNormal(
		t, h, 0.0f,   // top front right
		w, -h, d,   // bottom front right
		w, -h, -d    // bottom back right
	);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(t, h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(t, h, -d);
	glEnd();
	glPopMatrix();

	// Back face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(t, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, -d);
	glEnd();
	glPopMatrix();

	// Left face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, d);
	glEnd();
	glPopMatrix();

	// Top face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, h, d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(t, h, d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(t, h, -d);
	glEnd();
	glPopMatrix();

	// Bottom face
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, -h, d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -d);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -d);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, -h, d);
	glEnd();
	glPopMatrix();
}


void DrawPyramid(float width, float height)
{
	float w = width / 2.0f;
	float h = height / 2.0f;

	float apexX = 0.0f, apexY = h, apexZ = 0.0f;

	// Front face
	SetFaceNormal(apexX, apexY, apexZ, -w, -h, w, w, -h, w);
	glBegin(GL_TRIANGLES);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, h, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, w);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, w);
	glEnd();

	// Right face
	SetFaceNormal(apexX, apexY, apexZ, w, -h, w, w, -h, -w);
	glBegin(GL_TRIANGLES);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, h, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, w);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, -w);
	glEnd();

	// Back face
	SetFaceNormal(apexX, apexY, apexZ, w, -h, -w, -w, -h, -w);
	glBegin(GL_TRIANGLES);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, h, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(w, -h, -w);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, -w);
	glEnd();

	// Left face
	SetFaceNormal(apexX, apexY, apexZ, -w, -h, -w, -w, -h, w);
	glBegin(GL_TRIANGLES);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, h, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, -w);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, -h, w);
	glEnd();

	glEnd();

	// Bottom face
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, -h, -w);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, -h, w);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, -h, w);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, -h, -w);
	glEnd();
}

void DrawDisk(GLUquadricObj* disk, float innerRadius, float outerRadius, int slices, int loops)
{
	glPushMatrix();
	glRotatef(-GLU_ANGLEY_OFFSET, 1.0f, 0.0f, 0.0f);
	gluDisk(disk, innerRadius, outerRadius, slices, loops);
	glPopMatrix();
}

void DrawFlatCircle(GLUquadricObj* disk, float radius, int slices, int loops)
{
	DrawDisk(disk, 0.0f, radius, slices, loops);
}

void DrawCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	float offsetY = height / 2.0f;

	glPushMatrix();
	glTranslatef(0.0f, -offsetY, 0.0f);
	glRotatef(GLU_ANGLEY_OFFSET, 1.0f, 0.0f, 0.0f);
	gluCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);
	glPopMatrix();
}

void DrawSemiCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = { 1.0, 0.0, 0.0, 0.0 };
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	glDisable(GL_CLIP_PLANE0);

}

void DrawEnclosedCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	glPushMatrix();
	DrawCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	float offsetY = height / 2.0f;

	// Bottom Surface
	glPushMatrix();
	glTranslatef(0.0f, -offsetY, 0.0f);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	DrawFlatCircle(quadric, baseRadius, slices, stacks);
	glPopMatrix();
	// END Bottom Surface

	// Top Surface
	glPushMatrix();
	glTranslatef(0.0f, offsetY, 0.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	DrawFlatCircle(quadric, topRadius, slices, stacks);
	glPopMatrix();
	// END Top Surface

	glPopMatrix();
}

void DrawPartialEnclosedCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks, boolean isTopClosed, boolean isBottomClosed)
{
	glPushMatrix();
	DrawCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	float offsetY = height / 2.0f;

	if (isBottomClosed)
	{
		// Bottom Surface
		glPushMatrix();
		glTranslatef(0.0f, -offsetY, 0.0f);
		gluQuadricDrawStyle(quadric, GLU_FILL);
		DrawFlatCircle(quadric, baseRadius, slices, stacks);
		glPopMatrix();
		// END Bottom Surface
	}

	if (isTopClosed)
	{
		// Top Surface
		glPushMatrix();
		glTranslatef(0.0f, offsetY, 0.0f);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		gluQuadricDrawStyle(quadric, GLU_FILL);
		DrawFlatCircle(quadric, topRadius, slices, stacks);
		glPopMatrix();
		// END Top Surface
	}

	glPopMatrix();
}

void DrawEnclosedSemiCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = { 1.0, 0.0, 0.0, 0.0 };
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawEnclosedCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	glDisable(GL_CLIP_PLANE0);
}

void DrawPartialEnclosedSemiCylinder(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float slices, float stacks, boolean isTopClosed, boolean isBottomClosed)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = { 1.0, 0.0, 0.0, 0.0 };
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawPartialEnclosedCylinder(cylinder, baseRadius, topRadius, height, slices, stacks, isBottomClosed, isTopClosed);

	glDisable(GL_CLIP_PLANE0);
}

void DrawEnclosedCylinderWithThickness(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float thickness, float slices, float stacks, float loops)
{
	glPushMatrix();
	DrawCylinder(cylinder, baseRadius, topRadius, height, slices, stacks);

	float offsetY = height / 2.0f;

	float innerBottomRadius = baseRadius * thickness;
	float innerTopRadius = topRadius * thickness;

	// Bottom Surface
	glPushMatrix();
	glTranslatef(0.0f, -offsetY, 0.0f);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	DrawDisk(quadric, innerBottomRadius, baseRadius, slices, loops);
	glPopMatrix();
	// END Bottom Surface

	// Top Surface
	glPushMatrix();
	glTranslatef(0.0f, offsetY, 0.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	gluQuadricDrawStyle(quadric, GLU_FILL);
	DrawDisk(quadric, innerTopRadius, topRadius, slices, loops);
	glPopMatrix();
	// END Top Surface

	glPopMatrix();
}

void DrawEnclosedSemiCylinderWithThickness(GLUquadricObj* cylinder, float baseRadius, float topRadius, float height, float thickness, float slices, float stacks, float loops)
{
	float offsetY = height / 2.0f;

	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = { 1.0, 0.0, 0.0, 0.0 };
	// Equation: x >= 0 side is kept

	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	// Draw full cylinder (but clipped)
	DrawEnclosedCylinderWithThickness(cylinder, baseRadius, topRadius, height, thickness, slices, stacks, loops);

	glDisable(GL_CLIP_PLANE0);

}


void DrawSphere(GLUquadricObj* quadric, float radius, int slices, int stacks)
{
	glPushMatrix();
	gluSphere(quadric, radius, slices, stacks);
	glPopMatrix();
}

void DrawSemiSphere(GLUquadricObj* quadric, float radius, int slices, int stacks)
{
	// Define clipping plane (cuts along X axis to keeps one half)
	GLdouble plane[] = { 1.0, 0.0, 0.0, 0.0 };
	// Equation: x >= 0 side is kept
	// Enable clipping
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);
	// Draw full sphere (but clipped)
	DrawSphere(quadric, radius, slices, stacks);
	glDisable(GL_CLIP_PLANE0);
}

// -----------------
// PROJECTION SETUP
// -----------------

void SetPerspectiveProjection(float fovY, float aspectRatio, float nearPlane, float farPlane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovY, aspectRatio, nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
}

void SetOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
}

// -----------------
// CAMERA SETUP
// -----------------

void SetupCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	SetPerspectiveProjection(60.0f, 1.0f, 0.1f, 100.0f);

	glTranslatef(-cameraX, -cameraY, -cameraZ);
	glRotatef(-cameraAngleX, 1, 0, 0);
	glRotatef(-cameraAngleY, 0, 1, 0);
	glRotatef(-cameraAngleZ, 0, 0, 1);

	glMatrixMode(GL_MODELVIEW);
}

// -----------------
// LIGHTING SETUP
// -----------------
void SetupLighting()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

	if (questionNum == 4)
	{
		lightZ = cameraZ;
	}

	GLfloat lightPosition[] = { lightX, lightY, lightZ, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void ResetMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, defaultAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, defaultSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, defaultShininess);
}

void DrawLightIndicator()
{
	// DEBUG [LIGHT VISUALIZER]
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	Color lightVisualizerColor = { 1.0f, 1.0f, 0.0f };

	glPushMatrix();
	glTranslatef(lightX, lightY, lightZ);
	glColor3f(lightVisualizerColor.r, lightVisualizerColor.g, lightVisualizerColor.b);
	DrawSphere(quadric, 0.03f, SLICES, STACKS);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	// END DEBUG [LIGHT VISUALIZER]
}

// -----------------------------------------------------------------
// *********************** UTILITY FUNCTIONS ***********************
// -----------------------------------------------------------------

// ***********************
// ICE CREAM FUNCTIONS
// ***********************


// ***********************
// CHARACTER FUNCTIONS
// ***********************

// ---------------
// HEAD COMPONENTS
// ---------------

void DrawHeadBase(float baseRadius, float baseHeight)
{
	glPushMatrix();
	glScalef(1.1f, 1.0f, 1.2f);
	DrawEnclosedCylinder(quadric, baseRadius, baseRadius, baseHeight, SLICES, STACKS);
	glPopMatrix();
}
void DrawUpperHead()
{
	float baseRadius = 0.1f;
	float baseHeight = 0.1f;

	glPushMatrix();
	glScalef(1.1f, 1.0f, 1.2f);
	glTranslatef(0.0f, baseHeight * 0.1f, 0.0f);
	DrawSphere(quadric, baseRadius, SLICES, STACKS);
	glPopMatrix();
}
void DrawLowerHead()
{
	float baseRadius = 0.1f;
	float baseHeight = 0.1f;

	glPushMatrix();
	glScalef(1.1f, 0.6f, 1.2f);
	glTranslatef(0.0f, -baseHeight * 0.4f, 0.0f);
	DrawSphere(quadric, baseRadius, SLICES, STACKS);
	glPopMatrix();
}

void DrawEyeSclera(float side)
{
	float scleraRadius = 0.03f;
	glPushMatrix();
	glTranslatef(side * scleraRadius / 2, 0.0f, scleraRadius * 0.2f);
	glRotatef(side * 50.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 0.8f, 0.5f);
	DrawSphere(quadric, scleraRadius, SLICES, STACKS);
	glPopMatrix();
}
void DrawEyePupil(float side)
{
	float scleraRadius = 0.03f;
	float pupilRadius = 0.007f;
	glPushMatrix();
	glTranslatef(side * scleraRadius / 2, 0.0f, (scleraRadius * 0.6f + pupilRadius));
	glRotatef(side * 35.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 1.0f, 0.2f);
	DrawSphere(quadric, pupilRadius, SLICES, STACKS);
	glPopMatrix();
}
void DrawEye(float side)
{
	float baseRadius = 0.1f;

	glPushMatrix();
	glTranslatef(side * 0.035f, baseRadius * 0.15f, baseRadius * 0.88f);
	DrawEyeSclera(side);
	DrawEyePupil(side);
	glPopMatrix();
}
void DrawLeftEye() { DrawEye(-1.0f); }
void DrawRightEye() { DrawEye(1.0f); }

void DrawEyelash()
{
	float baseRadius = 0.1f;
	float eyelashLength = 0.02f;
	float eyelashThickness = 0.002f;

	// Middle Eyelash
	glPushMatrix();
	glTranslatef(-0.048f, baseRadius * 0.4f, baseRadius * 1.1f);
	glRotatef(-30.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quadric, eyelashThickness, eyelashThickness, eyelashLength, SLICES, STACKS);

	// Left Eyelash
	glPushMatrix();
	glTranslatef(0.0f, -0.015f, 0.0015f);
	glRotatef(-10.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quadric, eyelashThickness, eyelashThickness, eyelashLength, SLICES, STACKS);
	glPopMatrix();
	// END Left Eyelash

	// Right Eyelash
	glPushMatrix();
	glTranslatef(0.0f, 0.015f, 0.0015f);
	glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quadric, eyelashThickness, eyelashThickness, eyelashLength, SLICES, STACKS);
	glPopMatrix();
	// END Right Eyelash

	glPopMatrix();
	// END Middle Eyelash
}

void DrawLeftEyelash()
{
	glPushMatrix();
	DrawEyelash();
	glPopMatrix();
}
void DrawRightEyelash()
{
	glPushMatrix();
	glScalef(-1.0f, 1.0f, 1.0f);
	DrawEyelash();
	glPopMatrix();
}

void DrawLip(float length)
{
	float lipLength = length / 3;
	float lipRadius = 0.003f;

	// Middle Lip
	glPushMatrix();
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, lipRadius, lipRadius, lipLength, SLICES, STACKS);

	// Left Lip
	glPushMatrix();
	glTranslatef(-lipRadius * 0.85f, lipLength * 0.95f, 0.0f);
	glRotatef(12.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(80.0f, 0.0f, 1.0f, 0.0f);
	DrawEnclosedCylinder(quadric, lipRadius, lipRadius * 0.1f, lipLength, SLICES, STACKS);
	glPopMatrix();
	// END Left Lip

	// Right Lip
	glPushMatrix();
	glTranslatef(-lipRadius * 0.85f, -lipLength * 0.95f, 0.0f);
	glRotatef(-12.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-80.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quadric, lipRadius, lipRadius * 0.1f, lipLength, SLICES, STACKS);
	glPopMatrix();
	// END Right Lip

	glPopMatrix();
	// END Middle Lip
}
void DrawMouth()
{
	float baseRadius = 0.1f;
	float lipLength = baseRadius * 0.75f;

	//// Upper Lip
	//glPushMatrix();
	//glTranslatef(0.0f, -baseRadius * 0.3f, baseRadius * 1.2f);
	//DrawLip(lipLength);
	//glPopMatrix();
	//// END Upper Lip

	// Lower Lip
	glPushMatrix();
	glTranslatef(0.0f, -baseRadius * 0.4f, baseRadius * 1.2f);
	glScalef(1.0f, -1.0f, 1.0f);
	DrawLip(lipLength);
	glPopMatrix();
	// END Lower Lip
}

void DrawEar(float side)
{
	float baseRadius = 0.1f;
	float earSphereRadius = baseRadius * 0.3f;

	glPushMatrix();

	glTranslatef(baseRadius * 1.2f * side, -earSphereRadius * 0.5f, 0.0f);
	glRotatef(-10.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-20.0f * side, 0.0f, 0.0f, 1.0f);

	glScalef(0.8f, 1.2f, 0.5f);
	DrawSphere(quadric, earSphereRadius, SLICES, STACKS);

	glPopMatrix();
}
void DrawLeftEar() { DrawEar(-1.0f); }
void DrawRightEar() { DrawEar(1.0f); }

void DrawNoseSide(float side)
{
	float baseRadius = 0.1f;
	float baseHeight = 0.1f;

	float noseBaseRadius = baseRadius * 0.05f;
	float noseBaseHeight = baseHeight * 0.15f;
	float noseThickness = 0.7f;

	glPushMatrix();

	glTranslatef(noseBaseRadius * 0.5f * side, 0.0f, baseRadius * 1.2f);
	glRotatef(15.0f * side, 0.0f, 0.0f, 1.0f);
	glRotatef(-10.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

	DrawEnclosedSemiCylinderWithThickness(
		quadric,
		noseBaseRadius,
		noseBaseRadius * 0.5f,
		noseBaseHeight,
		noseThickness,
		SLICES, STACKS, LOOPS
	);

	glPopMatrix();
}
void DrawNoseMiddle()
{
	float baseRadius = 0.1f;
	float baseHeight = 0.1f;

	float noseBaseRadius = baseRadius * 0.05f;
	float noseBaseHeight = baseHeight * 0.1f;
	float noseThickness = 0.7f;

	glPushMatrix();

	glTranslatef(0.0f, noseBaseHeight * 0.3f, baseRadius * 1.2f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

	DrawEnclosedSemiCylinderWithThickness(
		quadric,
		noseBaseRadius * 2.0f,
		noseBaseRadius * 0.22f,
		noseBaseHeight * 2.0f,
		noseThickness,
		SLICES, STACKS, LOOPS
	);

	glPopMatrix();
}
void DrawNose()
{
	DrawNoseSide(-1.0f);
	DrawNoseSide(1.0f);
	DrawNoseMiddle();
}

// ----------------
// TORSO COMPONENTS
// ----------------

void DrawTorsoPart(float baseRadius, float baseHeight)
{
	// Center Base
	glPushMatrix();
	glScalef(1.5f, 1.0f, 0.5f);
	DrawEnclosedCylinder(quadric, baseRadius, baseRadius, baseHeight, SLICES, STACKS);

	// Sholder / Pelvis
	glPushMatrix();
	glTranslatef(0.0f, baseHeight * 0.5f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawSemiSphere(quadric, baseRadius, SLICES, STACKS);
	glPopMatrix();
	// END Sholder / Pelvis

	glPopMatrix();
	// END Center Base
}

// --------------
// ARM COMPONENTS
// --------------

void DrawUpperArm(float armLength)
{
	float baseRadius = 0.03f;

	// Upper Arm Joint
	glPushMatrix();
	DrawSphere(quadric, baseRadius, SLICES, STACKS);

	// Upper Arm
	glPushMatrix();
	glTranslatef(-armLength / 2, 0.0f, 0.0f);
	glRotatef(88.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, baseRadius * 0.8f, baseRadius * 0.45f, armLength, SLICES, STACKS);
	glPopMatrix();
	// END Upper Arm

	glPopMatrix();
	// END Upper Arm Joint
}

void DrawLowerArm(float armLength)
{
	float upperBaseRadius = 0.03f;
	float baseRadius = 0.02f;

	// Lower Arm Joint
	glPushMatrix();
	DrawSphere(quadric, upperBaseRadius * 0.6f, SLICES, STACKS);

	// Lower Arm
	glPushMatrix();
	glTranslatef(-armLength / 2, 0.0f, 0.0f);
	glRotatef(88.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, baseRadius * 0.7f, baseRadius * 0.3f, armLength, SLICES, STACKS);
	glPopMatrix();
	// END Lower Arm

	glPopMatrix();
	// END Lower Arm Joint
}

void DrawFinger(float length)
{
	// Finger Joint 1
	float joint1Radius = 0.005f;
	glPushMatrix();
	DrawSphere(quadric, joint1Radius, SLICES, STACKS);

	// Finger 1
	float finger1Length = length * 0.5f;
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, finger1Length / 2);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	DrawEnclosedCylinder(quadric, joint1Radius * 0.8f, joint1Radius * 0.5f, finger1Length, SLICES, STACKS);

	// Finger Joint 2
	float joint2Radius = joint1Radius * 0.8f;
	glPushMatrix();
	glTranslatef(0.0f, finger1Length / 2, 0.0f);
	DrawSphere(quadric, joint2Radius, SLICES, STACKS);

	// Finger 2
	float finger2Length = length / 2;
	glPushMatrix();
	glTranslatef(0.0f, finger1Length / 2 + finger2Length / 2, 0.0f);
	DrawEnclosedCylinder(quadric, joint2Radius * 0.8f, joint2Radius * 0.5f, finger2Length, SLICES, STACKS);

	// Finger Joint 3
	float joint3Radius = joint2Radius * 0.8f;
	glPushMatrix();
	glTranslatef(0.0f, finger2Length / 2, 0.0f);
	DrawSphere(quadric, joint3Radius, SLICES, STACKS);

	// Finger 3
	float finger3Length = length / 2;
	glPushMatrix();
	glTranslatef(0.0f, finger3Length / 2, 0.0f);
	DrawEnclosedCylinder(quadric, joint3Radius * 0.8f, joint3Radius * 0.5f, finger3Length, SLICES, STACKS);

	// Finger Tip
	glPushMatrix();
	glTranslatef(0.0f, finger3Length / 2, 0.0f);
	DrawSphere(quadric, joint3Radius * 0.5f, SLICES, STACKS);
	glPopMatrix();
	// END Finger Tip

	glPopMatrix();
	// END Finger 3

	glPopMatrix();
	// END Finger Joint 3

	glPopMatrix();
	// END Finger 2

	glPopMatrix();
	// END Finger Joint 2

	glPopMatrix();
	// END Finger 1

	glPopMatrix();
	// END Finger Joint 1
}

void DrawHand()
{
	float baseRadius = 0.018f;
	float palmSize = 0.018f;

	float thumbLength = 0.015f;
	float indexLength = 0.017f;
	float middleLength = 0.019f;
	float ringLength = indexLength;
	float littleLength = 0.014f;

	float fingerSpacing = 0.0065f;

	// Palm
	glPushMatrix();
	glScalef(0.9f, 0.5f, 1.0f);
	DrawSphere(quadric, baseRadius, SLICES, STACKS);

	// Reset scale
	glScalef(1.1f, 1.5f, 1.0f);

	// Thumb Finger
	glPushMatrix();
	glTranslatef(baseRadius * 0.3f, 0.0f, baseRadius * 0.8f);
	glRotatef(-70.0f, 0.0f, 1.0f, 0.0f);
	DrawFinger(thumbLength);
	glPopMatrix();
	// END Thumb Finger

	// Index Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2 + indexLength / 3), 0.0f, fingerSpacing);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	DrawFinger(indexLength);
	glPopMatrix();
	// END Index Finger

	// Middle Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2 + middleLength / 2), 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	DrawFinger(middleLength);
	glPopMatrix();
	// END Middle Finger

	// Ring Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2 + ringLength / 3), 0.0f, -fingerSpacing);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	DrawFinger(ringLength);
	glPopMatrix();
	// END Ring Finger

	// Little Finger
	glPushMatrix();
	glTranslatef(-(baseRadius / 2), 0.0f, -fingerSpacing * 2);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	DrawFinger(littleLength);
	glPopMatrix();
	// END Little Finger

	glPopMatrix();
	// END Palm
}

void DrawArm(float side)
{
	float upperArmLength = 0.06f;
	float lowerArmLength = 0.08f;

	// Upper Arm
	glPushMatrix();
	glRotatef((side + 1.0f) * 90.0f, 0.0f, 0.0f, 1.0f);
	DrawUpperArm(upperArmLength);

	// Lower Arm
	glPushMatrix();
	glTranslatef(-(upperArmLength / 2 + lowerArmLength / 2), 0.0f, 0.0f);
	DrawLowerArm(lowerArmLength);

	// Hand
	glPushMatrix();
	glTranslatef(-lowerArmLength, 0.0f, 0.0f);
	DrawHand();
	glPopMatrix();
	// END Hand

	glPopMatrix();
	// END Lower Arm

	glPopMatrix();
	// END Upper Arm
}

// --------------
// LEG COMPONENTS
// --------------

void DrawUpperLeg(float legLength)
{
	float baseRadius = 0.03f;

	// Upper Arm Joint
	glPushMatrix();
	DrawSphere(quadric, baseRadius, SLICES, STACKS);

	// Upper Arm
	glPushMatrix();
	glTranslatef(0.0f, -legLength / 2, 0.0f);
	//glRotatef(88.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, baseRadius * 0.45f, baseRadius * 0.8f, legLength, SLICES, STACKS);
	glPopMatrix();
	// END Upper Arm

	glPopMatrix();
	// END Upper Arm Joint

}

void DrawLowerLeg(float legLength)
{
	float upperBaseRadius = 0.03f;
	float baseRadius = 0.02f;

	// Lower Arm Joint
	glPushMatrix();
	DrawSphere(quadric, upperBaseRadius * 0.6f, SLICES, STACKS);

	// Lower Arm
	glPushMatrix();
	glTranslatef(0.0f, -legLength / 2, 0.0f);
	DrawEnclosedCylinder(quadric, baseRadius * 0.3f, baseRadius * 0.7f, legLength, SLICES, STACKS);
	glPopMatrix();
	// END Lower Arm

	glPopMatrix();
	// END Lower Arm Joint
}

void DrawToe(float length, float joint1Radius, boolean isBigToe)
{
	glScalef(1.0f, 1.0f, 0.8f);

	// Toe Joint 1
	//float joint1Radius = 0.0045f;
	glPushMatrix();
	DrawSphere(quadric, joint1Radius, SLICES, STACKS);

	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	// Toe 1
	float toe1Length = length * 0.5f;
	glPushMatrix();
	glTranslatef(0.0f, toe1Length / 2, 0.0f);
	DrawEnclosedCylinder(quadric, joint1Radius * 0.8f, joint1Radius * 0.5f, toe1Length, SLICES, STACKS);

	// Toe Joint 2
	float joint2Radius = joint1Radius * 0.5f;
	glPushMatrix();
	glTranslatef(0.0f, toe1Length / 2, 0.0f);
	DrawSphere(quadric, joint2Radius, SLICES, STACKS);

	// Toe 2
	float toe2Length = length * 0.3f;
	glPushMatrix();
	glTranslatef(0.0f, toe2Length / 2, 0.0f);
	DrawEnclosedCylinder(quadric, joint2Radius * 0.8f, joint2Radius * 0.5f, toe2Length, SLICES, STACKS);

	if (!isBigToe)
	{
		// Toe Joint 3
		float joint3Radius = joint2Radius * 0.8f;
		glPushMatrix();
		glTranslatef(0.0f, toe2Length / 2, 0.0f);
		DrawSphere(quadric, joint3Radius, SLICES, STACKS);

		// Toe 3
		float toe3Length = length / 2;
		glPushMatrix();
		glTranslatef(0.0f, joint3Radius + toe3Length / 2, 0.0f);
		DrawEnclosedCylinder(quadric, joint3Radius * 0.8f, joint3Radius * 0.5f, toe3Length, SLICES, STACKS);

		// Toe Tip
		glPushMatrix();
		glTranslatef(0.0f, toe3Length / 2, 0.0f);
		DrawSphere(quadric, joint3Radius * 0.5f, SLICES, STACKS);
		glPopMatrix();
		// END Toe Tip

		glPopMatrix();
		// END Toe 3

		glPopMatrix();
		// END Toe Joint 3
	}
	else
	{
		// Toe Tip
		glPushMatrix();
		glTranslatef(0.0f, toe2Length / 2, 0.0f);
		DrawSphere(quadric, joint2Radius * 0.5f, SLICES, STACKS);
		glPopMatrix();
		// END Toe Tip
	}


	glPopMatrix();
	// END Toe 2

	glPopMatrix();
	// END Toe Joint 2

	glPopMatrix();
	// END Toe 1

	glPopMatrix();
	// END Toe Joint 1
}

void DrawFoot()
{
	float ankleRadius = 0.01f;

	float bigToeLength = 0.01f;
	float indexToeLength = 0.015f;
	float middleToeLength = 0.01f;
	float ringToeLength = 0.008f;
	float littleToeLength = 0.007f;

	float bigToeRadius = 0.004f;
	float indexToeRadius = 0.003f;
	float middleToeRadius = 0.003f;
	float ringToeRadius = 0.003f;
	float littleToeRadius = 0.003f;

	float toeSpacing = 0.01f;

	// Ankle
	glPushMatrix();
	glScalef(1.0f, 0.7f, 1.2f);
	DrawSphere(quadric, ankleRadius, SLICES, STACKS);

	glScalef(1.0f, 1.3f, 0.5f);

	// Midfoot
	float midfootWidth = 0.1f;
	float midfootHeight = 0.015f;
	float midfootDepth = 0.03f;
	float midfootTopOffset = midfootWidth * 0.1f;
	glPushMatrix();
	glTranslatef(0.0f, -ankleRadius, ankleRadius + midfootTopOffset * 5);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	DrawRightTrapezoidalPrism(midfootWidth, midfootHeight, midfootDepth, midfootTopOffset);

	// Midfoot [BACK]
	float midfootBottomRadius = midfootDepth / 2;
	float midfootTopRadius = midfootTopOffset;
	glPushMatrix();
	glTranslatef(-midfootWidth / 2, 0.0f, 0.0f);
	glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);
	glScalef(5.5f, 1.0f, 1.5f);
	DrawEnclosedCylinder(quadric, midfootBottomRadius, midfootTopRadius, midfootHeight, SLICES, STACKS);
	glPopMatrix();
	// END Midfoot [BACK]

	float offsetX = (midfootWidth - midfootTopOffset) / 2;
	glTranslatef(offsetX, -midfootHeight / 4, -toeSpacing * 0.2f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 1.0f, 2.0f);

	// Big Toe
	glPushMatrix();
	glTranslatef(-toeSpacing * 1.2f, 0.0f, 0.0f);
	DrawToe(bigToeLength, bigToeRadius, true);
	glPopMatrix();
	// END Big Toe

	// Index Toe
	glPushMatrix();
	glTranslatef(-toeSpacing * 0.5f, 0.0f, 0.0f);
	DrawToe(indexToeLength, indexToeRadius, true);
	glPopMatrix();
	// END Index Toe

	// Middle Toe
	glPushMatrix();
	DrawToe(middleToeLength, middleToeRadius, true);
	glPopMatrix();
	// END Middle Toe

	// Ring Toe
	glPushMatrix();
	glTranslatef(toeSpacing * 0.5f, 0.0f, 0.0f);
	DrawToe(ringToeLength, ringToeRadius, true);
	glPopMatrix();
	// END Ring Toe

	// Little Toe
	glPushMatrix();
	glTranslatef(toeSpacing, 0.0f, 0.0f);
	DrawToe(littleToeLength, littleToeRadius, true);
	glPopMatrix();
	// END Little Toe

	glPopMatrix();
	// END Midfoot

		//// Midfoot [FRONT]
		//glPushMatrix();
		//glTranslatef(0.0f, 0.0f, 0.0f);
		//glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);
		//DrawEnclosedSemiCylinder(quadric, midfootRadius, midfootRadius * 0.85f, midfootHeight, SLICES, STACKS);


		//float offsetX = (midfootRadius + midfootRadius * 0.85f) / 2;
		//glTranslatef(offsetX, 0.0f, 0.0f);
		//glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		//glScalef(0.5f, 1.3f, 0.5f);

		//// Big Toe
		//glPushMatrix();
		//glTranslatef(-toeSpacing, 0.0f, -toeSpacing * 0.2f);
		////glTranslatef(toeSpacing * 2, -midfootHeight * 0.1f, midfootRadius * 1.8);
		//DrawToe(bigToeLength, true);
		//glPopMatrix();
		//// END Big Toe

		//glPopMatrix();
		//// END Midfoot [FRONT]

		//glPopMatrix();
		//// END Midfoot [BACK]

	glPopMatrix();
	// END Ankle
}

void DrawLeg(float side)
{
	float upperLegLength = 0.06f;
	float lowerLegLength = 0.07f;

	glScalef(side, 1.0f, 1.0f);

	// Upper Leg
	glPushMatrix();
	glRotatef(0.0f, 0.0f, (side + 1.0f) * 90.0f, 1.0f);
	DrawUpperLeg(upperLegLength);

	// Lower Leg
	glPushMatrix();
	glTranslatef(0.0f, -(upperLegLength / 2 + lowerLegLength / 2), 0.0f);
	DrawLowerLeg(lowerLegLength);

	// Foot
	glPushMatrix();
	glTranslatef(0.0f, -lowerLegLength, 0.0f);
	DrawFoot();
	glPopMatrix();
	// END Foot

	glPopMatrix();
	// END Lower Leg

	glPopMatrix();
	// END Upper Leg
}

// ========================
// CHARACTER PARTS ASSEMBLY
// ========================

void DrawHead(float headBaseRadius, float headBaseHeight)
{
	// Head
	glPushMatrix();
	DrawHeadBase(headBaseRadius, headBaseHeight);
	DrawUpperHead();
	DrawLowerHead();

	// Eyes
	glPushMatrix();
	DrawLeftEye();
	DrawRightEye();
	glPopMatrix();
	// END Eyes

	// Eyelashes
	glPushMatrix();
	DrawLeftEyelash();
	DrawRightEyelash();
	glPopMatrix();
	// END Eyelashes

	// Mouth
	glPushMatrix();
	DrawMouth();
	glPopMatrix();
	// END Mouth

	// Ears
	glPushMatrix();
	DrawLeftEar();
	DrawRightEar();
	glPopMatrix();
	// END Ears

	// Nose
	glPushMatrix();
	DrawNose();
	glPopMatrix();
	// END Nose

	glPopMatrix();
	// END Head
}

void DrawNeck(float neckHeight)
{
	float neckRadius = 0.02f;

	glPushMatrix();
	DrawEnclosedCylinder(quadric, neckRadius, neckRadius, neckHeight, SLICES, STACKS);
	glPopMatrix();
}

void DrawTorso(float torsoRadius, float torsoHeight)
{
	// Upper Torso
	glPushMatrix();
	//glRotatef(-10.0f, 1.0f, 0.0f, 0.0f);
	DrawTorsoPart(torsoRadius, torsoHeight);
	glPopMatrix();
	// END Upper Torso

	// Lower Torso
	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight, 0.0f);
	glScalef(1.0f, -1.0f, 1.0f);
	DrawTorsoPart(torsoRadius, torsoHeight);
	glPopMatrix();
	// END Lower Torso

}

void DrawArms()
{
	float torsoOffsetX = 0.085f;

	// Left Arm
	glPushMatrix();
	glTranslatef(-torsoOffsetX, 0.0f, 0.0f);
	glRotatef(-30.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(80.0f, 0.0f, 0.0f, 1.0f);
	DrawArm(-1.0f);
	glPopMatrix();
	// END Left Arm

	// Right Arm
	glPushMatrix();
	glTranslatef(torsoOffsetX, 0.0f, 0.0f);
	glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-80.0f, 0.0f, 0.0f, 1.0f);
	DrawArm(1.0f);
	glPopMatrix();
	// END Right Arm
}

void DrawLegs()
{
	float torsoOffsetX = 0.04f;

	// Left Leg
	glPushMatrix();
	glTranslatef(-torsoOffsetX, 0.0f, 0.0f);
	//glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
	DrawLeg(-1.0f);
	glPopMatrix();
	// END Left Leg

	// Right Leg
	glPushMatrix();
	glTranslatef(torsoOffsetX, 0.0f, 0.0f);
	//glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
	DrawLeg(1.0f);
	glPopMatrix();
	// END Right Leg
}

// ***********************
// HAIR FUNCTIONS
// ***********************

void DrawHair(float headBaseRadius)
{
	float hairRadius = headBaseRadius * 1.2f;

	// Hair Scalp
	glPushMatrix();
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.8f, 1.0f, 1.025f);
	DrawSemiSphere(quadric, hairRadius, SLICES, STACKS);

	float sideHairTopRadius = hairRadius * 0.6f;

	// Side Hair [LEFT]
	glPushMatrix();
	glTranslatef(0.0f, headBaseRadius * 0.47f, 0.0f);
	glScalef(1.3f, 1.0f, 1.1f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawSemiSphere(quadric, sideHairTopRadius, SLICES, STACKS);
	glPopMatrix();
	// END Side Hair [LEFT]

	// Side Hair [RIGHT]
	glPushMatrix();
	glTranslatef(0.0f, -headBaseRadius * 0.47f, 0.0f);
	glScalef(1.3f, 1.0f, 1.1f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	DrawSemiSphere(quadric, sideHairTopRadius, SLICES, STACKS);
	glPopMatrix();
	// END Side Hair [RIGHT]

	// Back Hair [TOP]
	float topBackHairRadius = hairRadius * 0.9f;
	glPushMatrix();
	glTranslatef(-topBackHairRadius * 0.5f, 0.0f, -hairRadius * 0.2f);
	glRotatef(80.0f, 0.0f, 1.0f, 0.0f);
	DrawSemiSphere(quadric, topBackHairRadius, SLICES, STACKS);
	glPopMatrix();
	// END Back Hair [TOP]

	// Back Hair [BOTTOM]
	float bottomBackHairRadius = hairRadius * 0.9f;
	glPushMatrix();
	glTranslatef(-bottomBackHairRadius * 1.2f, 0.0f, -hairRadius * 0.22f);
	glRotatef(93.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 1.0f, 1.5f);
	DrawSemiSphere(quadric, bottomBackHairRadius, SLICES, STACKS);
	glPopMatrix();
	// END Back Hair [BOTTOM]

	float bangRadius = hairRadius * 0.28f;

	//glScalef(1.5f, 1.0f, 1.2f);
	glScalef(1.3f, 1.0f, 1.2f);

	// Left Bun
	glPushMatrix();
	glTranslatef(hairRadius * 0.6f, hairRadius * 0.8f, 0.0f);
	DrawSphere(quadric, bangRadius, SLICES, STACKS);
	glPopMatrix();
	// END Left Bun

	// Right Bun
	glPushMatrix();
	glTranslatef(hairRadius * 0.6f, -hairRadius * 0.8f, 0.0f);
	DrawSphere(quadric, bangRadius, SLICES, STACKS);
	glPopMatrix();
	// END Right Bun

	glPopMatrix();
	// END Hair Scalp
}

// ***********************
// COSTUME FUNCTIONS
// ***********************

void DrawRedVest(float torsoRadius, float torsoHeight)
{
	float vestRadius = torsoRadius * 1.2f;
	float vestHeight = torsoRadius * 1.5f;

	// Left Side Vest
	glPushMatrix();
	glTranslatef(-torsoRadius * 0.35f, 0.0f, 0.0f);
	glRotatef(-3.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 1.62f, 0.8f);
	DrawPartialEnclosedSemiCylinder(quadric, vestRadius, vestRadius, vestHeight, SLICES, STACKS, false, true);

	// Left Side Top Semi Cylinder
	float topVestHeight = vestHeight * 0.12f;
	glPushMatrix();
	glTranslatef(0.0f, vestHeight / 2 + topVestHeight / 2, 0.0f);
	DrawEnclosedSemiCylinder(quadric, vestRadius, 0.0f, topVestHeight, SLICES, STACKS);
	glPopMatrix();
	// END Left Side Top Semi Cylinder

	glPopMatrix();
	// END Left Side Vest

	// Right Side Vest
	glPushMatrix();
	glTranslatef(torsoRadius * 0.35f, 0.0f, 0.0f);
	glRotatef(3.0f, 0.0f, 0.0f, 1.0f);
	glScalef(1.0f, 1.62f, 0.8f);
	DrawPartialEnclosedSemiCylinder(quadric, vestRadius, vestRadius, vestHeight, SLICES, STACKS, false, true);

	// Right Side Top Semi Cylinder
	glPushMatrix();
	glTranslatef(0.0f, vestHeight / 2 + topVestHeight / 2, 0.0f);
	DrawEnclosedSemiCylinder(quadric, vestRadius, 0.0f, topVestHeight, SLICES, STACKS);
	glPopMatrix();
	// END Right Side Top Semi Cylinder

	glPopMatrix();
	// END Right Side Vest

	// Back Side Vest
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -torsoRadius / 2);
	glScalef(1.0f, 1.62f, 0.9f);
	DrawCuboidPolygon(torsoRadius, vestHeight, torsoRadius);
	glPopMatrix();
	// END Back Side Vest
}

void DrawPantLeg(float side, float torsoRadius)
{
	float legRadius = 0.03f;
	float legHeight = 0.1f;

	// Pant Leg
	glPushMatrix();
	glTranslatef(-(torsoRadius / 2 + legHeight * 0.3f), -side * (torsoRadius * 0.5f + legRadius * 0.35f), 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	DrawCylinder(quadric, legRadius * 0.4f, legRadius, legHeight, SLICES, STACKS);

	// Hem
	float hemRadius = legHeight * 0.118f;
	glPushMatrix();
	glTranslatef(0.0f, -legHeight / 2, 0.0f);
	DrawSphere(quadric, hemRadius, SLICES, STACKS);

	// Opening
	float openingRadius = hemRadius * 0.68f;
	float openingHeight = legHeight * 0.05f;
	glPushMatrix();
	glTranslatef(0.0f, -hemRadius, 0.0f);
	DrawEnclosedCylinder(quadric, openingRadius, openingRadius, openingHeight, SLICES, STACKS);
	glPopMatrix();
	// END Opening

	glPopMatrix();
	// END Hem

	glPopMatrix();
	// END Pant Leg
}

void DrawPants(float torsoRadius)
{
	float upperPantRadius = 0.03f;
	float upperPantHeight = torsoRadius * 2.7f;

	// Waistband
	glPushMatrix();
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	DrawEnclosedCylinder(quadric, upperPantRadius, upperPantRadius, upperPantHeight, SLICES, STACKS);

	// Pant Leg [LEFT]
	glPushMatrix();
	DrawPantLeg(-1.0f, torsoRadius);
	glPopMatrix();
	// END Pant Leg [LEFT]

	// Pant Leg [RIGHT]
	glPushMatrix();
	DrawPantLeg(1.0f, torsoRadius);
	glPopMatrix();
	// END Pant Leg [RIGHT]

	glPopMatrix();
	// END Waistband
}

// ***********************
// PROPS FUNCTIONS
// ***********************

void DrawHairRibbon()
{

}

void DrawBelt()
{

}

// ***********************
// PROPS FUNCTIONS
// ***********************



// ----------------------------------------------------

// ==============
// MODEL ASSEMBLY
// ==============

void DrawCharacter()
{
	// Torso
	float torsoRadius = 0.06f;
	float torsoHeight = 0.03f;
	glPushMatrix();
	DrawTorso(torsoRadius, torsoHeight);

	// Neck
	float neckHeight = 0.06f;
	glPushMatrix();
	glTranslatef(0.0f, (torsoHeight / 2 + neckHeight * 1.1f), 0.0f);
	DrawNeck(neckHeight);

	// Head
	float headBaseRadius = 0.1f;
	float headBaseHeight = 0.05f;
	glPushMatrix();
	glTranslatef(0.0f, neckHeight + headBaseHeight * 0.8f, 0.0f);
	DrawHead(headBaseRadius, headBaseHeight);

	// Hair
	glPushMatrix();
	glTranslatef(0.0f, headBaseRadius * 0.5f, 0.0f);
	DrawHair(headBaseRadius);
	glPopMatrix();
	// END Hair

	glPopMatrix();
	// END Head

	glPopMatrix();
	// END Neck


	// Arms
	glPushMatrix();
	glTranslatef(0.0f, torsoHeight, 0.0f);
	DrawArms();
	glPopMatrix();
	// END Arms

	// Legs
	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight * 3, 0.0f);
	DrawLegs();
	glPopMatrix();
	// END Legs

	// Pants
	glPushMatrix();
	glTranslatef(0.0f, -torsoHeight * 3, 0.0f);
	DrawPants(torsoRadius);
	glPopMatrix();
	// END Pants

	// Red Vest [COSTUME]
	glPushMatrix();
	DrawRedVest(torsoRadius, torsoHeight);
	glPopMatrix();
	// END Red Vest [COSTUME]

	glPopMatrix();
	// END Torso
}

// ------------------
// ENVIRONMENT SETUP
// ------------------

void DrawSky(GLUquadricObj* quadric, float radius, int slices, int stacks)
{
	glPushMatrix();

	// Optional: keep the sky fixed relative to camera
	glTranslatef(cameraX, cameraY, cameraZ);

	// Invert normals for inside view
	gluQuadricOrientation(quadric, GLU_INSIDE);
	glColor3f(0.53f, 0.81f, 0.92f); // light blue sky color

	gluSphere(quadric, radius, slices, stacks);

	glPopMatrix();
}

void DrawSea(float width, float depth, float y)
{
	glPushMatrix();
	glTranslatef(0.0f, y, 0.0f);

	glEnable(GL_COLOR_MATERIAL);

	glBindTexture(GL_TEXTURE_2D, seaTexture);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	float w = width / 2.0f;
	float d = depth / 2.0f;

	glNormal3f(0.0f, 1.0f, 0.0f); // Upward normal for lighting

	float repeat = 30.0f; // Texture repeat factor

	glTexCoord2f(0.0f, repeat); glVertex3f(-w, 0.0f, -d);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0.0f, d);
	glTexCoord2f(repeat, 0.0f); glVertex3f(w, 0.0f, d);
	glTexCoord2f(repeat, repeat); glVertex3f(w, 0.0f, -d);
	glEnd();

	glDisable(GL_COLOR_MATERIAL);

	glPopMatrix();
}

// -------------------------------------------------------

void Display()
{
	//--------------------------------
	//	OpenGL drawing
	//--------------------------------

	// Clear Screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

	SetupCamera();
	SetupLighting();
	ResetMaterial();

	DrawLightIndicator();

	// NeZha
	//glEnable(GL_TEXTURE_2D);

	float characterOffSetX = 0.0f;
	float characterOffSetY = -0.13f;
	float characterOffSetZ = -1.0f;
	float characterSize = 1.0f;
	glPushMatrix();
	glTranslatef(characterOffSetX, characterOffSetY, characterOffSetZ);
	DrawCharacter();
	glPopMatrix();
	// END NeZha

	//glDisable(GL_TEXTURE_2D);

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	//--------------------------------
	//	End of OpenGL drawing
	//--------------------------------
}

//--------------------------------------------------------------------

// Windows Entry Point (called by OS, the values are passed in by OS)
// HINSTANCE = Instance number or index number (ID No) of windows app or form (just like map() we need to assign key with index for each child components)
// The 2nd HInstance is the ID for the parent window (just like ActiveX) (obsolete parameters) (When we run, we need a parent window to keep track of the child window)
// LPSTR = Long Pointer to a string (USEFUL) (like we can issue a command before the window runs)
// e.g. In CMD:
//		ping.exe www.google.com (it will ping to this website)
//
//		OR
//			"C:\Program Files\Google\Chrome\Application\chrome.exe"
//			we can right as
//			"C:\Program Files\Google\Chrome\Application\chrome.exe" --incognito
// 
// nCmdShow: OS will pass in (full screen, minimize, etc.)
int WINAPI WinMain(
	_In_ HINSTANCE hInst,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR cmdlparameter,
	_In_ int nCmdShow
)
{
	//if (cmdlparameter == "debugmode")
	//{
	//	// do something (e.g. privilege)
	//}

	// WNDCLASSEX = Windows Class Extended (Data Structure) (We'll specify what we need)
	WNDCLASSEX wc;	// data structure to store how you want the class to be
	ZeroMemory(&wc, sizeof(WNDCLASSEX)); // To fill up the memory with zeros based on the size of WNDCLASSEX to &wc

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL); // ask OS for the ID, same with passing the hInst
	wc.lpfnWndProc = WindowProcedure;		// long pointer to a function
	wc.lpszClassName = CLASS_TITLE;			// long pointer to a zero terminated string (e.g. "Hello World" actually is "Hello World0")
	wc.style = CS_HREDRAW | CS_VREDRAW;	// 

	if (!RegisterClassEx(&wc)) return false; // We pass the wc to let the OS to build for us during runtime 
	// (because we dont have the proprietary code for window instantiation)

//	CW_USEDEFAULT = where should the window be displayed, if we put 0 and 0 is top left
//	800x800 = width x height

	HWND hWnd = CreateWindow(CLASS_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	// DC = Display context (pass it to handler like a pointer)
	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	InitPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	// to tell OS which instance we are specifying at (we may open many different window)
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	// Initialize textures used
	InitTextures();

	//--------------------------------
	//	End initialization
	//--------------------------------

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true) // message loop
	{
		// PeekMessage = to ask OS any message to the window
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg); // SEND THIS MESSAGE TO WindowProcedure
		}

		// Setup Projection

		Display();

		SwapBuffers(hdc);
	}

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------