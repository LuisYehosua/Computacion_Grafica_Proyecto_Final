//320255409 | 422130448
//13/05/2026
//Proyecto Final
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

//Audio
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
unsigned int loadCubemap(std::vector<std::string> faces);

// Robot keyframe functions
void resetElementsRobot();
void interpolationRobot();
void AnimationRobot();
void loadFromFileRobot(const char* filename);

// Perro keyframe functions 
void resetElementsPerro();
void interpolationPerro();
void AnimationPerro();
void loadFromFilePerro(const char* filename);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera (Y = 1.7 para altura de ojos)
Camera  camera(glm::vec3(0.0f, 1.7f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,2.0f, 0.0f),
	glm::vec3(0.0f,0.0f, 0.0f),
	glm::vec3(0.0f,0.0f,  0.0f),
	glm::vec3(0.0f,0.0f, 0.0f)
};

// LAMPARAS DEL TECHO
glm::vec3 lampPositions[] = {
	glm::vec3(17.849f, 18.173f,   4.872f),
	glm::vec3(17.488f, 18.173f,  15.971f),
	glm::vec3(17.686f, 18.173f,  -7.700f),
	glm::vec3(18.457f, 18.219f, -17.999f),
	glm::vec3(4.4002f, 18.205f, -18.156f),
	glm::vec3(4.5392f, 18.198f,  -7.913f),
	glm::vec3(4.6731f, 18.213f,   4.883f),
	glm::vec3(4.7584f, 18.188f,  15.869f)
};
const int NUM_LAMPS = sizeof(lampPositions) / sizeof(lampPositions[0]);

bool lampsOn = false;

//Audio
ma_engine audioEngine;
ma_sound musicaFondo;
ma_sound sonidoParo;

// Constantes del walking simulator
const float CAMERA_HEIGHT = 6.7f;
const float BOB_SPEED = 9.0f;
const float BOB_AMOUNT = 0.08f;
const float WALK_SPEED_MULT = 0.5f;
const float PITCH_LIMIT = 75.0f;

// Variables del parpadeo
float blinkTimer = 0.0f;
float nextBlinkTime = 3.0f;
bool isBlinking = false;
float blinkProgress = 0.0f;
const float BLINK_DURATION = 0.20f;


float vertices[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};


// Vertices del cubo del Skybox
float skyboxVertices[] = {
	-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
};


// Vertices del quad para el parpadeo
float quadVertices[] = {
	-1.0f, -1.0f,
	 1.0f, -1.0f,
	 1.0f,  1.0f,
	 1.0f,  1.0f,
	-1.0f,  1.0f,
	-1.0f, -1.0f
};


glm::vec3 Light1 = glm::vec3(0);
float rotBall = 0.0f;
bool AnimBall = false;

//Variables subir elementos
float sceneRiseY = -12.0f;
bool startSceneRise = false;
bool sceneArrived = false;

// Variables de postura del robot
float rBrazoD = 0.0f;
float rBrazoI = 0.0f;
float rPiernaD = 0.0f;
float rPiernaI = 0.0f;
float robotPosX = 5.0f;
float robotPosY = 0.0f;
float robotPosZ = 0.0f;
float robotRotY = 0.0f;

#define MAX_FRAMES_ROBOT 250
int iMaxStepsRobot = 60;
int iCurrStepsRobot = 0;

typedef struct {

	// Posición y rotación
	float posX, posY, posZ;
	float rotY;

	// Partes
	float brazoDeg;
	float brazoIeg;
	float piernaDeg;
	float piernaIeg;

	// Incrementos
	float incX, incY, incZ;
	float rotYInc;

	float brazoDInc;
	float brazoIInc;
	float piernaDInc;
	float piernaIInc;

} FRAME_ROBOT;

FRAME_ROBOT KeyFrameRobot[MAX_FRAMES_ROBOT];
int  frameIndexRobot = 0;
bool playRobot = false;
int  playIndexRobot = 0;

void resetElementsRobot()
{
	robotPosX = KeyFrameRobot[0].posX;
	robotPosY = KeyFrameRobot[0].posY;
	robotPosZ = KeyFrameRobot[0].posZ;

	robotRotY = KeyFrameRobot[0].rotY;

	rBrazoD = KeyFrameRobot[0].brazoDeg;
	rBrazoI = KeyFrameRobot[0].brazoIeg;

	rPiernaD = KeyFrameRobot[0].piernaDeg;
	rPiernaI = KeyFrameRobot[0].piernaIeg;
}

void interpolationRobot()
{
	int i = playIndexRobot;

	// Posición
	KeyFrameRobot[i].incX =
		(KeyFrameRobot[i + 1].posX - KeyFrameRobot[i].posX) / iMaxStepsRobot;

	KeyFrameRobot[i].incY =
		(KeyFrameRobot[i + 1].posY - KeyFrameRobot[i].posY) / iMaxStepsRobot;

	KeyFrameRobot[i].incZ =
		(KeyFrameRobot[i + 1].posZ - KeyFrameRobot[i].posZ) / iMaxStepsRobot;

	// Rotación robot
	KeyFrameRobot[i].rotYInc =
		(KeyFrameRobot[i + 1].rotY - KeyFrameRobot[i].rotY) / iMaxStepsRobot;

	// Extremidades
	KeyFrameRobot[i].brazoDInc =
		(KeyFrameRobot[i + 1].brazoDeg - KeyFrameRobot[i].brazoDeg) / iMaxStepsRobot;

	KeyFrameRobot[i].brazoIInc =
		(KeyFrameRobot[i + 1].brazoIeg - KeyFrameRobot[i].brazoIeg) / iMaxStepsRobot;

	KeyFrameRobot[i].piernaDInc =
		(KeyFrameRobot[i + 1].piernaDeg - KeyFrameRobot[i].piernaDeg) / iMaxStepsRobot;

	KeyFrameRobot[i].piernaIInc =
		(KeyFrameRobot[i + 1].piernaIeg - KeyFrameRobot[i].piernaIeg) / iMaxStepsRobot;
}

void AnimationRobot()
{
	if (!playRobot) return;

	if (iCurrStepsRobot >= iMaxStepsRobot)
	{
		// Asegurar posición EXACTA del frame destino
		robotPosX = KeyFrameRobot[playIndexRobot + 1].posX;
		robotPosY = KeyFrameRobot[playIndexRobot + 1].posY;
		robotPosZ = KeyFrameRobot[playIndexRobot + 1].posZ;

		robotRotY = KeyFrameRobot[playIndexRobot + 1].rotY;

		rBrazoD = KeyFrameRobot[playIndexRobot + 1].brazoDeg;
		rBrazoI = KeyFrameRobot[playIndexRobot + 1].brazoIeg;

		rPiernaD = KeyFrameRobot[playIndexRobot + 1].piernaDeg;
		rPiernaI = KeyFrameRobot[playIndexRobot + 1].piernaIeg;

		playIndexRobot++;

		if (playIndexRobot > frameIndexRobot - 2)
		{
			playRobot = false;
			playIndexRobot = 0;

			printf("Robot - animacion terminada\n");
		}
		else
		{
			iCurrStepsRobot = 0;
			interpolationRobot();
		}
	}
	else
	{
		// Posición
		robotPosX += KeyFrameRobot[playIndexRobot].incX;
		robotPosY += KeyFrameRobot[playIndexRobot].incY;
		robotPosZ += KeyFrameRobot[playIndexRobot].incZ;

		// Rotación
		robotRotY += KeyFrameRobot[playIndexRobot].rotYInc;

		// Extremidades
		rBrazoD += KeyFrameRobot[playIndexRobot].brazoDInc;
		rBrazoI += KeyFrameRobot[playIndexRobot].brazoIInc;

		rPiernaD += KeyFrameRobot[playIndexRobot].piernaDInc;
		rPiernaI += KeyFrameRobot[playIndexRobot].piernaIInc;

		iCurrStepsRobot++;
	}
}

void loadFromFileRobot(const char* filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) { printf("No se pudo abrir %s\n", filename); return; }
	file >> frameIndexRobot;
	for (int i = 0; i < frameIndexRobot; i++)
	{
		file >> KeyFrameRobot[i].posX
			>> KeyFrameRobot[i].posY
			>> KeyFrameRobot[i].posZ
			>> KeyFrameRobot[i].rotY

			>> KeyFrameRobot[i].brazoDeg
			>> KeyFrameRobot[i].brazoIeg
			>> KeyFrameRobot[i].piernaDeg
			>> KeyFrameRobot[i].piernaIeg;
	}
	file.close();
	printf("Animación robot cargada desde %s (%d keyframes)\n", filename, frameIndexRobot);
}

//PERRO KEYFRAME SYSTEM 

float pHead = 0.0f;
float pTail = 0.0f;
float pFLeftLeg = 0.0f;
float pFRightLeg = 0.0f;
float pBLeftLeg = 0.0f;
float pBRightLeg = 0.0f;
float pBodyRotZ = 0.0f;
float pRotDog = 0.0f;
float pPosX = 2.0f;
float pPosY = 1.7f;
float pPosZ = 3.0f;

#define MAX_FRAMES_PERRO 250
int iMaxStepsPerro = 60;
int iCurrStepsPerro = 0;

typedef struct {
	float posX, posY, posZ;
	float rotDog, bodyRotZ;
	float head, tail;
	float FLeftLeg, FRightLeg, BLeftLeg, BRightLeg;
	// Incrementos
	float incX, incY, incZ;
	float rotDogInc, bodyRotZInc;
	float headInc, tailInc;
	float FLeftLegInc, FRightLegInc, BLeftLegInc, BRightLegInc;
} FRAME_PERRO;

FRAME_PERRO KeyFramePerro[MAX_FRAMES_PERRO];
int  frameIndexPerro = 0;
bool playPerro = false;
int  playIndexPerro = 0;

void resetElementsPerro()
{
	pPosX = KeyFramePerro[0].posX;
	pPosY = KeyFramePerro[0].posY;
	pPosZ = KeyFramePerro[0].posZ;
	pRotDog = KeyFramePerro[0].rotDog;
	pBodyRotZ = KeyFramePerro[0].bodyRotZ;
	pHead = KeyFramePerro[0].head;
	pTail = KeyFramePerro[0].tail;
	pFLeftLeg = KeyFramePerro[0].FLeftLeg;
	pFRightLeg = KeyFramePerro[0].FRightLeg;
	pBLeftLeg = KeyFramePerro[0].BLeftLeg;
	pBRightLeg = KeyFramePerro[0].BRightLeg;
}

void interpolationPerro()
{
	int i = playIndexPerro;
	KeyFramePerro[i].incX = (KeyFramePerro[i + 1].posX - KeyFramePerro[i].posX) / iMaxStepsPerro;
	KeyFramePerro[i].incY = (KeyFramePerro[i + 1].posY - KeyFramePerro[i].posY) / iMaxStepsPerro;
	KeyFramePerro[i].incZ = (KeyFramePerro[i + 1].posZ - KeyFramePerro[i].posZ) / iMaxStepsPerro;
	KeyFramePerro[i].rotDogInc = (KeyFramePerro[i + 1].rotDog - KeyFramePerro[i].rotDog) / iMaxStepsPerro;
	KeyFramePerro[i].bodyRotZInc = (KeyFramePerro[i + 1].bodyRotZ - KeyFramePerro[i].bodyRotZ) / iMaxStepsPerro;
	KeyFramePerro[i].headInc = (KeyFramePerro[i + 1].head - KeyFramePerro[i].head) / iMaxStepsPerro;
	KeyFramePerro[i].tailInc = (KeyFramePerro[i + 1].tail - KeyFramePerro[i].tail) / iMaxStepsPerro;
	KeyFramePerro[i].FLeftLegInc = (KeyFramePerro[i + 1].FLeftLeg - KeyFramePerro[i].FLeftLeg) / iMaxStepsPerro;
	KeyFramePerro[i].FRightLegInc = (KeyFramePerro[i + 1].FRightLeg - KeyFramePerro[i].FRightLeg) / iMaxStepsPerro;
	KeyFramePerro[i].BLeftLegInc = (KeyFramePerro[i + 1].BLeftLeg - KeyFramePerro[i].BLeftLeg) / iMaxStepsPerro;
	KeyFramePerro[i].BRightLegInc = (KeyFramePerro[i + 1].BRightLeg - KeyFramePerro[i].BRightLeg) / iMaxStepsPerro;
}

void AnimationPerro()
{
	if (!playPerro) return;

	if (iCurrStepsPerro >= iMaxStepsPerro)
	{
		pPosX = KeyFramePerro[playIndexPerro + 1].posX;
		pPosY = KeyFramePerro[playIndexPerro + 1].posY;
		pPosZ = KeyFramePerro[playIndexPerro + 1].posZ;

		pRotDog = KeyFramePerro[playIndexPerro + 1].rotDog;
		pBodyRotZ = KeyFramePerro[playIndexPerro + 1].bodyRotZ;

		pHead = KeyFramePerro[playIndexPerro + 1].head;
		pTail = KeyFramePerro[playIndexPerro + 1].tail;

		pFLeftLeg = KeyFramePerro[playIndexPerro + 1].FLeftLeg;
		pFRightLeg = KeyFramePerro[playIndexPerro + 1].FRightLeg;

		pBLeftLeg = KeyFramePerro[playIndexPerro + 1].BLeftLeg;
		pBRightLeg = KeyFramePerro[playIndexPerro + 1].BRightLeg;
		playIndexPerro++;
		if (playIndexPerro > frameIndexPerro - 2)
		{
			printf("Perro - animación terminada\n");
			playIndexPerro = 0;
			playPerro = false;
		}
		else
		{
			iCurrStepsPerro = 0;
			interpolationPerro();
		}
	}
	else
	{
		pPosX += KeyFramePerro[playIndexPerro].incX;
		pPosY += KeyFramePerro[playIndexPerro].incY;
		pPosZ += KeyFramePerro[playIndexPerro].incZ;
		pRotDog += KeyFramePerro[playIndexPerro].rotDogInc;
		pBodyRotZ += KeyFramePerro[playIndexPerro].bodyRotZInc;
		pHead += KeyFramePerro[playIndexPerro].headInc;
		pTail += KeyFramePerro[playIndexPerro].tailInc;
		pFLeftLeg += KeyFramePerro[playIndexPerro].FLeftLegInc;
		pFRightLeg += KeyFramePerro[playIndexPerro].FRightLegInc;
		pBLeftLeg += KeyFramePerro[playIndexPerro].BLeftLegInc;
		pBRightLeg += KeyFramePerro[playIndexPerro].BRightLegInc;
		iCurrStepsPerro++;
	}
}

void loadFromFilePerro(const char* filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) { printf("No se pudo abrir %s\n", filename); return; }
	file >> frameIndexPerro;
	for (int i = 0; i < frameIndexPerro; i++)
	{
		file >> KeyFramePerro[i].posX
			>> KeyFramePerro[i].posY
			>> KeyFramePerro[i].posZ
			>> KeyFramePerro[i].rotDog
			>> KeyFramePerro[i].bodyRotZ
			>> KeyFramePerro[i].head
			>> KeyFramePerro[i].tail
			>> KeyFramePerro[i].FLeftLeg
			>> KeyFramePerro[i].FRightLeg
			>> KeyFramePerro[i].BLeftLeg
			>> KeyFramePerro[i].BRightLeg;
	}
	file.close();
	printf("Animación perro cargada desde %s (%d keyframes)\n", filename, frameIndexPerro);
}

// ===========================================================================
//  MUJER KEYFRAME SYSTEM
// ===========================================================================
float mujerPosX = 3.0f, mujerPosY = 2.5f, mujerPosZ = -24.0f;
float mujerRotY = 1.0f;
float mujerRotX = 0.0f;

float mCabeza = 0.0f;

float mHombroD = 0.0f;
float mHombroDZ = 0.0f;
float mCodoD = 0.0f;
float mManoD = 0.0f;

float mHombroI = 0.0f;
float mHombroIZ = 0.0f;
float mCodoI = 0.0f;
float mManoI = 0.0f;

float mPiernaD = 0.0f;
float mPieD = 0.0f;

float mPiernaI = 0.0f;
float mPieI = 0.0f;

#define MAX_FRAMES_MUJER 250
int iMaxStepsMujer = 60;
int iCurrStepsMujer = 0;

typedef struct {
	float posX, posY, posZ;
	float rotY, rotX;
	float cabeza;
	float hombroD, hombroDZ, codoD, manoD;
	float hombroI, hombroIZ, codoI, manoI;
	float piernaD, pieD;
	float piernaI, pieI;
	// incrementos
	float incX, incY, incZ;
	float rotYInc, rotXInc;
	float cabezaInc;
	float hombroDInc, hombroDZInc, codoDInc, manoDInc;
	float hombroIInc, hombroIZInc, codoIInc, manoIInc;
	float piernaDInc, pieDInc;
	float piernaIInc, pieIInc;
} FRAME_MUJER;

FRAME_MUJER KeyFrameMujer[MAX_FRAMES_MUJER];
int  frameIndexMujer = 0;
bool playMujer = false;
int  playIndexMujer = 0;

void saveFrameMujer()
{
	if (frameIndexMujer >= MAX_FRAMES_MUJER) { printf("Max keyframes mujer\n"); return; }
	printf("Mujer - guardando keyframe %d\n", frameIndexMujer);
	KeyFrameMujer[frameIndexMujer].posX = mujerPosX;
	KeyFrameMujer[frameIndexMujer].posY = mujerPosY;
	KeyFrameMujer[frameIndexMujer].posZ = mujerPosZ;
	KeyFrameMujer[frameIndexMujer].rotY = mujerRotY;
	KeyFrameMujer[frameIndexMujer].rotX = mujerRotX;
	KeyFrameMujer[frameIndexMujer].cabeza = mCabeza;
	KeyFrameMujer[frameIndexMujer].hombroD = mHombroD;
	KeyFrameMujer[frameIndexMujer].hombroDZ = mHombroDZ;
	KeyFrameMujer[frameIndexMujer].codoD = mCodoD;
	KeyFrameMujer[frameIndexMujer].manoD = mManoD;
	KeyFrameMujer[frameIndexMujer].hombroI = mHombroI;
	KeyFrameMujer[frameIndexMujer].hombroIZ = mHombroIZ;
	KeyFrameMujer[frameIndexMujer].codoI = mCodoI;
	KeyFrameMujer[frameIndexMujer].manoI = mManoI;
	KeyFrameMujer[frameIndexMujer].piernaD = mPiernaD;
	KeyFrameMujer[frameIndexMujer].pieD = mPieD;
	KeyFrameMujer[frameIndexMujer].piernaI = mPiernaI;
	KeyFrameMujer[frameIndexMujer].pieI = mPieI;
	frameIndexMujer++;
}

void resetElementsMujer()
{
	mujerPosX = KeyFrameMujer[0].posX;
	mujerPosY = KeyFrameMujer[0].posY;
	mujerPosZ = KeyFrameMujer[0].posZ;
	mujerRotY = KeyFrameMujer[0].rotY;
	mujerRotX = KeyFrameMujer[0].rotX;
	mCabeza = KeyFrameMujer[0].cabeza;
	mHombroD = KeyFrameMujer[0].hombroD;
	mHombroDZ = KeyFrameMujer[0].hombroDZ;
	mCodoD = KeyFrameMujer[0].codoD;
	mManoD = KeyFrameMujer[0].manoD;
	mHombroI = KeyFrameMujer[0].hombroI;
	mHombroIZ = KeyFrameMujer[0].hombroIZ;
	mCodoI = KeyFrameMujer[0].codoI;
	mManoI = KeyFrameMujer[0].manoI;
	mPiernaD = KeyFrameMujer[0].piernaD;
	mPieD = KeyFrameMujer[0].pieD;
	mPiernaI = KeyFrameMujer[0].piernaI;
	mPieI = KeyFrameMujer[0].pieI;
}

void interpolationMujer()
{
	int i = playIndexMujer;
	KeyFrameMujer[i].incX = (KeyFrameMujer[i + 1].posX - KeyFrameMujer[i].posX) / iMaxStepsMujer;
	KeyFrameMujer[i].incY = (KeyFrameMujer[i + 1].posY - KeyFrameMujer[i].posY) / iMaxStepsMujer;
	KeyFrameMujer[i].incZ = (KeyFrameMujer[i + 1].posZ - KeyFrameMujer[i].posZ) / iMaxStepsMujer;
	KeyFrameMujer[i].rotYInc = (KeyFrameMujer[i + 1].rotY - KeyFrameMujer[i].rotY) / iMaxStepsMujer;
	KeyFrameMujer[i].rotXInc = (KeyFrameMujer[i + 1].rotX - KeyFrameMujer[i].rotX) / iMaxStepsMujer;
	KeyFrameMujer[i].cabezaInc = (KeyFrameMujer[i + 1].cabeza - KeyFrameMujer[i].cabeza) / iMaxStepsMujer;
	KeyFrameMujer[i].hombroDInc = (KeyFrameMujer[i + 1].hombroD - KeyFrameMujer[i].hombroD) / iMaxStepsMujer;
	KeyFrameMujer[i].hombroDZInc = (KeyFrameMujer[i + 1].hombroDZ - KeyFrameMujer[i].hombroDZ) / iMaxStepsMujer;
	KeyFrameMujer[i].codoDInc = (KeyFrameMujer[i + 1].codoD - KeyFrameMujer[i].codoD) / iMaxStepsMujer;
	KeyFrameMujer[i].manoDInc = (KeyFrameMujer[i + 1].manoD - KeyFrameMujer[i].manoD) / iMaxStepsMujer;
	KeyFrameMujer[i].hombroIInc = (KeyFrameMujer[i + 1].hombroI - KeyFrameMujer[i].hombroI) / iMaxStepsMujer;
	KeyFrameMujer[i].hombroIZInc = (KeyFrameMujer[i + 1].hombroIZ - KeyFrameMujer[i].hombroIZ) / iMaxStepsMujer;
	KeyFrameMujer[i].codoIInc = (KeyFrameMujer[i + 1].codoI - KeyFrameMujer[i].codoI) / iMaxStepsMujer;
	KeyFrameMujer[i].manoIInc = (KeyFrameMujer[i + 1].manoI - KeyFrameMujer[i].manoI) / iMaxStepsMujer;
	KeyFrameMujer[i].piernaDInc = (KeyFrameMujer[i + 1].piernaD - KeyFrameMujer[i].piernaD) / iMaxStepsMujer;
	KeyFrameMujer[i].pieDInc = (KeyFrameMujer[i + 1].pieD - KeyFrameMujer[i].pieD) / iMaxStepsMujer;
	KeyFrameMujer[i].piernaIInc = (KeyFrameMujer[i + 1].piernaI - KeyFrameMujer[i].piernaI) / iMaxStepsMujer;
	KeyFrameMujer[i].pieIInc = (KeyFrameMujer[i + 1].pieI - KeyFrameMujer[i].pieI) / iMaxStepsMujer;
}

void AnimationMujer()
{
	if (!playMujer) return;
	if (iCurrStepsMujer >= iMaxStepsMujer)
	{
		mujerPosX = KeyFrameMujer[playIndexMujer + 1].posX;
		mujerPosY = KeyFrameMujer[playIndexMujer + 1].posY;
		mujerPosZ = KeyFrameMujer[playIndexMujer + 1].posZ;
		mujerRotY = KeyFrameMujer[playIndexMujer + 1].rotY;
		mujerRotX = KeyFrameMujer[playIndexMujer + 1].rotX;
		mCabeza = KeyFrameMujer[playIndexMujer + 1].cabeza;
		mHombroD = KeyFrameMujer[playIndexMujer + 1].hombroD;
		mHombroDZ = KeyFrameMujer[playIndexMujer + 1].hombroDZ;
		mCodoD = KeyFrameMujer[playIndexMujer + 1].codoD;
		mManoD = KeyFrameMujer[playIndexMujer + 1].manoD;
		mHombroI = KeyFrameMujer[playIndexMujer + 1].hombroI;
		mHombroIZ = KeyFrameMujer[playIndexMujer + 1].hombroIZ;
		mCodoI = KeyFrameMujer[playIndexMujer + 1].codoI;
		mManoI = KeyFrameMujer[playIndexMujer + 1].manoI;
		mPiernaD = KeyFrameMujer[playIndexMujer + 1].piernaD;
		mPieD = KeyFrameMujer[playIndexMujer + 1].pieD;
		mPiernaI = KeyFrameMujer[playIndexMujer + 1].piernaI;
		mPieI = KeyFrameMujer[playIndexMujer + 1].pieI;
		playIndexMujer++;
		if (playIndexMujer > frameIndexMujer - 2)
		{
			printf("Mujer - animación terminada\n"); playMujer = false; playIndexMujer = 0;
		}
		else
		{
			iCurrStepsMujer = 0; interpolationMujer();
		}
	}
	else
	{
		mujerPosX += KeyFrameMujer[playIndexMujer].incX;
		mujerPosY += KeyFrameMujer[playIndexMujer].incY;
		mujerPosZ += KeyFrameMujer[playIndexMujer].incZ;
		mujerRotY += KeyFrameMujer[playIndexMujer].rotYInc;
		mujerRotX += KeyFrameMujer[playIndexMujer].rotXInc;
		mCabeza += KeyFrameMujer[playIndexMujer].cabezaInc;
		mHombroD += KeyFrameMujer[playIndexMujer].hombroDInc;
		mHombroDZ += KeyFrameMujer[playIndexMujer].hombroDZInc;
		mCodoD += KeyFrameMujer[playIndexMujer].codoDInc;
		mManoD += KeyFrameMujer[playIndexMujer].manoDInc;
		mHombroI += KeyFrameMujer[playIndexMujer].hombroIInc;
		mHombroIZ += KeyFrameMujer[playIndexMujer].hombroIZInc;
		mCodoI += KeyFrameMujer[playIndexMujer].codoIInc;
		mManoI += KeyFrameMujer[playIndexMujer].manoIInc;
		mPiernaD += KeyFrameMujer[playIndexMujer].piernaDInc;
		mPieD += KeyFrameMujer[playIndexMujer].pieDInc;
		mPiernaI += KeyFrameMujer[playIndexMujer].piernaIInc;
		mPieI += KeyFrameMujer[playIndexMujer].pieIInc;
		iCurrStepsMujer++;
	}
}

void saveToFileMujer(const char* filename)
{
	std::ofstream file(filename);
	if (!file.is_open()) { printf("No se pudo crear %s\n", filename); return; }
	file << frameIndexMujer << "\n";
	for (int i = 0; i < frameIndexMujer; i++)
		file << KeyFrameMujer[i].posX << " " << KeyFrameMujer[i].posY << " "
		<< KeyFrameMujer[i].posZ << " " << KeyFrameMujer[i].rotY << " "
		<< KeyFrameMujer[i].rotX << " " << KeyFrameMujer[i].cabeza << " "
		<< KeyFrameMujer[i].hombroD << " " << KeyFrameMujer[i].hombroDZ << " "
		<< KeyFrameMujer[i].codoD << " " << KeyFrameMujer[i].manoD << " "
		<< KeyFrameMujer[i].hombroI << " " << KeyFrameMujer[i].hombroIZ << " "
		<< KeyFrameMujer[i].codoI << " " << KeyFrameMujer[i].manoI << " "
		<< KeyFrameMujer[i].piernaD << " " << KeyFrameMujer[i].pieD << " "
		<< KeyFrameMujer[i].piernaI << " " << KeyFrameMujer[i].pieI << "\n";
	file.close();
	printf("Animación mujer guardada en %s\n", filename);
}

void loadFromFileMujer(const char* filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) { printf("No se pudo abrir %s\n", filename); return; }
	file >> frameIndexMujer;
	for (int i = 0; i < frameIndexMujer; i++)
		file >> KeyFrameMujer[i].posX >> KeyFrameMujer[i].posY >> KeyFrameMujer[i].posZ
		>> KeyFrameMujer[i].rotY >> KeyFrameMujer[i].rotX >> KeyFrameMujer[i].cabeza
		>> KeyFrameMujer[i].hombroD >> KeyFrameMujer[i].hombroDZ >> KeyFrameMujer[i].codoD
		>> KeyFrameMujer[i].manoD >> KeyFrameMujer[i].hombroI >> KeyFrameMujer[i].hombroIZ
		>> KeyFrameMujer[i].codoI >> KeyFrameMujer[i].manoI >> KeyFrameMujer[i].piernaD
		>> KeyFrameMujer[i].pieD >> KeyFrameMujer[i].piernaI >> KeyFrameMujer[i].pieI;
	file.close();
	printf("Animación mujer cargada desde %s (%d keyframes)\n", filename, frameIndexMujer);
}

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
	srand((unsigned int)time(NULL));

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Final | 320255409 | 422130448", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
	Shader skyboxShader("Shader/skybox.vs", "Shader/skybox.frag");
	Shader blinkShader("Shader/blink.vs", "Shader/blink.frag");

	Model Facultad((char*)"Models/FacultadCOMPLETOYA.obj");

	//Mujer escaleras
	Model CabezaM((char*)"Models/Mujer/Cabeza.obj");
	Model CuerpoM((char*)"Models/Mujer/Cuerpo.obj");
	Model CodoDM((char*)"Models/Mujer/CodoD.obj");
	Model SoloHombroD((char*)"Models/Mujer/SoloHombro.obj");
	Model HombroDM((char*)"Models/Mujer/HombroD.obj");
	Model CodoIM((char*)"Models/Mujer/CodoI.obj");
	Model ManoIM((char*)"Models/Mujer/ManoI.obj");
	Model ManoDM((char*)"Models/Mujer/ManoD.obj");
	Model SoloHombroI((char*)"Models/Mujer/SoloHombroI.obj");
	Model HombroIM((char*)"Models/Mujer/HombroI.obj");
	Model PieDM((char*)"Models/Mujer/PieD.obj");
	Model PiernaDM((char*)"Models/Mujer/PiernaD.obj");
	Model PieIM((char*)"Models/Mujer/PieI.obj");
	Model PiernaIM((char*)"Models/Mujer/PiernaI.obj");

	// --- Inicializar arreglos de keyframes ---
	for (int i = 0; i < MAX_FRAMES_ROBOT; i++) {
		KeyFrameRobot[i] = { 0,0,0,0, 0,0,0,0 };
	}
	for (int i = 0; i < MAX_FRAMES_PERRO; i++) {
		KeyFramePerro[i] = { 0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0 };
	}

	// VAO/VBO de los cubitos
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// VAO/VBO del Skybox
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	// VAO/VBO del quad de parpadeo
	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindVertexArray(0);


	// Cargar skybox
	std::vector<std::string> facesDia = {
		"Models/Skybox/dia/right.jpg",
		"Models/Skybox/dia/left.jpg",
		"Models/Skybox/dia/top.jpg",
		"Models/Skybox/dia/bottom.jpg",
		"Models/Skybox/dia/back.jpg",
		"Models/Skybox/dia/front.jpg"
	};
	unsigned int cubemapDia = loadCubemap(facesDia);

	skyboxShader.Use();
	glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);


	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// --- Inicializar Motor de Audio ---
	if (ma_engine_init(NULL, &audioEngine) != MA_SUCCESS) {
		std::cout << "Error al iniciar el motor de audio" << std::endl;
	}

	// Musica de fondo
	ma_sound_init_from_file(&audioEngine, "Audio/Fly_Frank.mp3", MA_SOUND_FLAG_STREAM, NULL, NULL, &musicaFondo);
	ma_sound_set_looping(&musicaFondo, MA_TRUE);
	ma_sound_start(&musicaFondo);

	// Sonido para modo paro
	ma_sound_init_from_file(&audioEngine, "Audio/Paro.mp3", MA_SOUND_FLAG_STREAM, NULL, NULL, &sonidoParo);
	ma_sound_set_looping(&sonidoParo, MA_TRUE);
	ma_sound_set_volume(&sonidoParo, 0.8f);

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		DoMovement();
		AnimationRobot();
		AnimationPerro();
		AnimationMujer();
		// Animación de aparición de escena
		if (startSceneRise && !sceneArrived)
		{
			sceneRiseY += 0.08f;

			if (sceneRiseY >= 0.0f)
			{
				sceneRiseY = 0.0f;
				sceneArrived = true;
			}
		}

		// Logica del parpadeo
		blinkTimer += deltaTime;
		float blinkAlpha = 0.0f;

		if (isBlinking)
		{
			blinkProgress += deltaTime / BLINK_DURATION;
			if (blinkProgress >= 1.0f)
			{
				isBlinking = false;
				blinkProgress = 0.0f;
				blinkTimer = 0.0f;
				nextBlinkTime = 3.0f + (rand() % 90) / 10.0f;
			}
			else
			{
				blinkAlpha = sin(blinkProgress * 3.14159f);
			}
		}
		else if (blinkTimer >= nextBlinkTime)
		{
			isBlinking = true;
			blinkProgress = 0.0f;
		}


		glClearColor(0.4f, 0.7f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);


		glm::mat4 modelTemp = glm::mat4(1.0f);

		lightingShader.Use();
		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);

		// Point light 1 (SPACE)
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

		// Lamparas rojas
		glm::vec3 lampColor = lampsOn ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f);
		for (int i = 0; i < NUM_LAMPS; i++)
		{
			std::string idx = std::to_string(i + 1);
			std::string base = "pointLights[" + idx + "].";

			glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "position").c_str()),
				lampPositions[i].x, lampPositions[i].y, lampPositions[i].z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "ambient").c_str()),
				lampColor.x * 0.1f, lampColor.y * 0.1f, lampColor.z * 0.1f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "diffuse").c_str()),
				lampColor.x, lampColor.y, lampColor.z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "specular").c_str()),
				lampColor.x, lampColor.y, lampColor.z);
			glUniform1f(glGetUniformLocation(lightingShader.Program, (base + "constant").c_str()), 1.0f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, (base + "linear").c_str()), 0.022f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, (base + "quadratic").c_str()), 0.0019f);
		}

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

		glm::mat4 view = camera.GetViewMatrix();

		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);
		// Facultad
		model = modelTemp;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Facultad.Draw(lightingShader);

		// ---- MUJER ESCALERAS ----
		glm::mat4 mujerBase = glm::mat4(1.0f);
		mujerBase = glm::translate(mujerBase, glm::vec3(mujerPosX, mujerPosY + sceneRiseY, mujerPosZ));
		mujerBase = glm::rotate(mujerBase, glm::radians(mujerRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		mujerBase = glm::rotate(mujerBase, glm::radians(mujerRotX), glm::vec3(1.0f, 0.0f, 0.0f));
		mujerBase = glm::scale(mujerBase, glm::vec3(3.0f, 3.0f, 3.0f));

		// Cuerpo
		glm::mat4 modelCuerpo = mujerBase;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCuerpo));
		CuerpoM.Draw(lightingShader);
		SoloHombroD.Draw(lightingShader);
		SoloHombroI.Draw(lightingShader);

		// Cabeza (Y = decir que no)
		glm::mat4 modelCabeza = modelCuerpo;
		modelCabeza = glm::translate(modelCabeza, glm::vec3(0.0f, 5.0f, 0.0f));
		modelCabeza = glm::rotate(modelCabeza, glm::radians(mCabeza), glm::vec3(0.0f, 1.0f, 0.0f));
		modelCabeza = glm::translate(modelCabeza, glm::vec3(0.0f, -5.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCabeza));
		CabezaM.Draw(lightingShader);

		// Hombro D (X = adelante/atrás, Z = lateral)
		glm::mat4 modelHombroD = modelCuerpo;
		modelHombroD = glm::translate(modelHombroD, glm::vec3(-0.06f, 1.42f, 0.0f));
		modelHombroD = glm::rotate(modelHombroD, glm::radians(mHombroD), glm::vec3(0.0f, 1.0f, 0.0f));
		modelHombroD = glm::rotate(modelHombroD, glm::radians(mHombroDZ), glm::vec3(0.0f, 0.0f, 1.0f));
		modelHombroD = glm::translate(modelHombroD, glm::vec3(0.06f, -1.42f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelHombroD));
		HombroDM.Draw(lightingShader);

		// Codo D (hereda hombro)
		glm::mat4 modelCodoD = modelHombroD;
		modelCodoD = glm::translate(modelCodoD, glm::vec3(-0.4f, 1.42f, 0.03f));
		modelCodoD = glm::rotate(modelCodoD, glm::radians(mCodoD), glm::vec3(0.0f, 1.0f, 0.0f));
		modelCodoD = glm::translate(modelCodoD, glm::vec3(0.4f, -1.42f, -0.03f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCodoD));
		CodoDM.Draw(lightingShader);

		// Mano D (hereda codo, muñeca en Y)
		glm::mat4 modelManoD = modelCodoD;
		modelManoD = glm::translate(modelManoD, glm::vec3(-0.6f, 1.42f, 0.3f));
		modelManoD = glm::rotate(modelManoD, glm::radians(mManoD), glm::vec3(0.0f, 0.0f, 1.0f));
		modelManoD = glm::translate(modelManoD, glm::vec3(0.6f, -1.42f, -0.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelManoD));
		ManoDM.Draw(lightingShader);

		// Hombro I
		glm::mat4 modelHombroI = modelCuerpo;
		modelHombroI = glm::translate(modelHombroI, glm::vec3(0.06f, 1.42f, 0.0f));
		modelHombroI = glm::rotate(modelHombroI, glm::radians(mHombroI), glm::vec3(0.0f, 1.0f, 0.0f));
		modelHombroI = glm::rotate(modelHombroI, glm::radians(mHombroIZ), glm::vec3(0.0f, 0.0f, 1.0f));
		modelHombroI = glm::translate(modelHombroI, glm::vec3(-0.06f, -1.42f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelHombroI));
		HombroIM.Draw(lightingShader);

		// Codo I (hereda hombro)
		glm::mat4 modelCodoI = modelHombroI;
		modelCodoI = glm::translate(modelCodoI, glm::vec3(0.3f, 1.42f, 0.04f));
		modelCodoI = glm::rotate(modelCodoI, glm::radians(mCodoI), glm::vec3(0.0f, 1.0f, 0.0f));
		modelCodoI = glm::translate(modelCodoI, glm::vec3(-0.3f, -1.42f, -0.04f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCodoI));
		CodoIM.Draw(lightingShader);

		// Mano I (hereda codo)
		glm::mat4 modelManoI = modelCodoI;
		modelManoI = glm::translate(modelManoI, glm::vec3(0.6f, 1.41f, 0.4f));
		modelManoI = glm::rotate(modelManoI, glm::radians(mManoI), glm::vec3(0.0f, 0.0f, 1.0f));
		modelManoI = glm::translate(modelManoI, glm::vec3(-0.6f, -1.41f, -0.4f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelManoI));
		ManoIM.Draw(lightingShader);

		// Pierna D
		glm::mat4 modelPiernaD = modelCuerpo;
		modelPiernaD = glm::translate(modelPiernaD, glm::vec3(-0.1f, 1.0f, 0.0f));
		modelPiernaD = glm::rotate(modelPiernaD, glm::radians(mPiernaD), glm::vec3(1.0f, 0.0f, 0.0f));
		modelPiernaD = glm::translate(modelPiernaD, glm::vec3(0.1f, -1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPiernaD));
		PiernaDM.Draw(lightingShader);

		// Pie D (hereda pierna)
		glm::mat4 modelPieD = modelPiernaD;
		modelPieD = glm::translate(modelPieD, glm::vec3(-0.1f, 0.5f, 0.1f));
		modelPieD = glm::rotate(modelPieD, glm::radians(mPieD), glm::vec3(1.0f, 0.0f, 0.0f));
		modelPieD = glm::translate(modelPieD, glm::vec3(0.1f, -0.5f, -0.1f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPieD));
		PieDM.Draw(lightingShader);

		// Pierna I
		glm::mat4 modelPiernaI = modelCuerpo;
		modelPiernaI = glm::translate(modelPiernaI, glm::vec3(0.1f, 0.9f, 0.0f));
		modelPiernaI = glm::rotate(modelPiernaI, glm::radians(mPiernaI), glm::vec3(1.0f, 0.0f, 0.0f));
		modelPiernaI = glm::translate(modelPiernaI, glm::vec3(-0.1f, -0.9f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPiernaI));
		PiernaIM.Draw(lightingShader);

		// Pie I (hereda pierna)
		glm::mat4 modelPieI = modelPiernaI;
		modelPieI = glm::translate(modelPieI, glm::vec3(0.1f, 0.5f, 0.1f));
		modelPieI = glm::rotate(modelPieI, glm::radians(mPieI), glm::vec3(1.0f, 0.0f, 0.0f));
		modelPieI = glm::translate(modelPieI, glm::vec3(-0.1f, -0.5f, -0.1f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPieI));
		PieIM.Draw(lightingShader);


		// Lamp shader
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[0]);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"), 1.0f, 1.0f, 1.0f);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);


		if (lampsOn)
		{
			glUniform3f(glGetUniformLocation(lampShader.Program, "lightColor"), 1.0f, 0.0f, 0.0f);
			glBindVertexArray(VAO);
			for (int i = 0; i < NUM_LAMPS; i++)
			{
				model = glm::mat4(1);
				model = glm::translate(model, lampPositions[i]);
				model = glm::scale(model, glm::vec3(0.6f, 0.2f, 1.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			glBindVertexArray(0);
		}


		// Skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.Use();
		glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(skyboxView));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapDia);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);


		// Quad de parpadeo encima de todo
		if (blinkAlpha > 0.01f)
		{
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			blinkShader.Use();
			glUniform1f(glGetUniformLocation(blinkShader.Program, "alpha"), blinkAlpha);

			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}


		glfwSwapBuffers(window);
	}

	// Apagar audio
	ma_sound_uninit(&musicaFondo);
	ma_sound_uninit(&sonidoParo);
	ma_engine_uninit(&audioEngine);

	glfwTerminate();
	return 0;
}


// Funcion para cargar un cubemap
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, channels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = SOIL_load_image(faces[i].c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			SOIL_free_image_data(data);
		}
		else
		{
			std::cout << "Fallo al cargar textura del cubemap: " << faces[i] << std::endl;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


void DoMovement()
{
	static float bobTime = 0.0f;

	bool isWalking = keys[GLFW_KEY_W] || keys[GLFW_KEY_S] ||
		keys[GLFW_KEY_A] || keys[GLFW_KEY_D] ||
		keys[GLFW_KEY_UP] || keys[GLFW_KEY_DOWN] ||
		keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_RIGHT];

	// Velocidad realista
	float walkDelta = deltaTime * WALK_SPEED_MULT;

	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
		camera.ProcessKeyboard(FORWARD, walkDelta);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
		camera.ProcessKeyboard(BACKWARD, walkDelta);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
		camera.ProcessKeyboard(LEFT, walkDelta);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
		camera.ProcessKeyboard(RIGHT, walkDelta);

	// Cabeceo al caminar
	if (isWalking)
	{
		bobTime += deltaTime * BOB_SPEED;
		float bob = sin(bobTime) * BOB_AMOUNT;
		camera.SetPositionY(CAMERA_HEIGHT + bob);
	}
	else
	{
		bobTime = 0.0f;
		camera.SetPositionY(CAMERA_HEIGHT);
	}
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) keys[key] = true;
		else if (action == GLFW_RELEASE) keys[key] = false;
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active) Light1 = glm::vec3(0.2f, 0.8f, 1.0f);
		else Light1 = glm::vec3(0);
	}

	// Q  -> play / stop animación robot
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		if (!playRobot && frameIndexRobot > 1)
		{
			resetElementsRobot();
			interpolationRobot();
			playRobot = true;
			playIndexRobot = 0;
			iCurrStepsRobot = 0;
			printf("Robot - reproduciendo animación\n");
		}
		else
		{
			playRobot = false;
			printf("Robot - animación detenida\n");
		}
	}

	// R  -> cargar animación robot desde archivo
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		loadFromFileRobot("animacion_robot.txt");
		loadFromFilePerro("animacion_perro.txt");
		loadFromFileMujer("animacion_mujer.txt");
	}

	// E  -> play / stop animación perro
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		if (!playPerro && frameIndexPerro > 1)
		{
			resetElementsPerro();
			interpolationPerro();
			playPerro = true;
			playIndexPerro = 0;
			iCurrStepsPerro = 0;
			printf("Perro - reproduciendo animacion\n");
		}
		else
		{
			playPerro = false;
			printf("Perro - animacion detenida\n");
		}
	}

	//Luces modo paro L
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		lampsOn = !lampsOn;
		if (lampsOn)
		{
			ma_sound_start(&sonidoParo);
			ma_sound_set_volume(&musicaFondo, 0.2f);
		}
		else
		{
			ma_sound_stop(&sonidoParo);
			ma_sound_set_volume(&musicaFondo, 0.5f);
		}
	}

	//Suben stands y personajes
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		startSceneRise = true;

	//Mujer
		// Guardar keyframe
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
		saveFrameMujer();
	// Guardar archivo
	if (key == GLFW_KEY_F && action == GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
		saveToFileMujer("animacion_mujer.txt");

	// Play/Stop
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		if (!playMujer && frameIndexMujer > 1)
		{
			resetElementsMujer(); interpolationMujer();
			playMujer = true; playIndexMujer = 0; iCurrStepsMujer = 0;
			printf("Mujer - reproduciendo\n");
		}
		else { playMujer = false; printf("Mujer - detenida\n"); }
	}
	// Guardar archivo mujer (F)
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		saveToFileMujer("animacion_mujer.txt");
	if (action == GLFW_PRESS)
	{
		// Posición y rotación base
		if (key == GLFW_KEY_1) mujerPosZ -= 0.1f;   // adelante
		if (key == GLFW_KEY_2) mujerPosZ += 0.1f;   // atrás
		if (key == GLFW_KEY_3) mujerPosY += 0.1f;   // subir
		if (key == GLFW_KEY_4) mujerPosY -= 0.1f;   // bajar
		if (key == GLFW_KEY_5) mujerRotY += 1.0f;    // girar Y
		if (key == GLFW_KEY_6) mujerRotY -= 1.0f;
		if (key == GLFW_KEY_7) mujerRotX += 1.0f;    // inclinar cuerpo (caída)
		if (key == GLFW_KEY_8) mujerRotX -= 1.0f;

		// Cabeza (decir que no = Y)
		if (key == GLFW_KEY_0) mCabeza += 5.0f;
		if (key == GLFW_KEY_P) mCabeza -= 5.0f;

		// Hombro D (X = adelante/atrás)
		if (key == GLFW_KEY_F1) mHombroD += 5.0f;
		if (key == GLFW_KEY_F2) mHombroD -= 5.0f;
		// Hombro D (Z = lateral)
		if (key == GLFW_KEY_F3) mHombroDZ += 5.0f;
		if (key == GLFW_KEY_F4) mHombroDZ -= 5.0f;

		// Codo D
		if (key == GLFW_KEY_F5) mCodoD += 5.0f;
		if (key == GLFW_KEY_F6) mCodoD -= 5.0f;

		// Mano D (muñeca)
		if (key == GLFW_KEY_F7) mManoD += 5.0f;
		if (key == GLFW_KEY_F8) mManoD -= 5.0f;

		// Hombro I
		if (key == GLFW_KEY_F1) mHombroI -= 5.0f;
		if (key == GLFW_KEY_F2) mHombroI += 5.0f;
		// Hombro I (Z = lateral)
		if (key == GLFW_KEY_F3) mHombroIZ -= 5.0f;
		if (key == GLFW_KEY_F4) mHombroIZ += 5.0f;

		// Codo I
		if (key == GLFW_KEY_F5) mCodoI -= 5.0f;
		if (key == GLFW_KEY_F6) mCodoI += 5.0f;

		// Mano I (muñeca)
		if (key == GLFW_KEY_F7) mManoI -= 5.0f;
		if (key == GLFW_KEY_F8) mManoI += 5.0f;
		// Pierna D
		if (key == GLFW_KEY_Z) mPiernaD += 5.0f;
		if (key == GLFW_KEY_X) mPiernaD -= 5.0f;

		// Pie D (tobillo)
		if (key == GLFW_KEY_C) mPieD += 5.0f;
		if (key == GLFW_KEY_V) mPieD -= 5.0f;

		// Pierna I
		if (key == GLFW_KEY_B) mPiernaI += 5.0f;
		if (key == GLFW_KEY_N) mPiernaI -= 5.0f;

		// Pie I (tobillo)
		if (key == GLFW_KEY_M) mPieI += 5.0f;
		if (key == GLFW_KEY_COMMA)  mPieI -= 5.0f;
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);

	// Limitar mirada vertical
	if (camera.GetPitch() > PITCH_LIMIT)
	{
		camera.SetPitch(PITCH_LIMIT);
	}
	if (camera.GetPitch() < -PITCH_LIMIT)
	{
		camera.SetPitch(-PITCH_LIMIT);
	}
}

