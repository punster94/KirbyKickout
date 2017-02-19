#define GAME_CPP
#include <windows.h>											// Header File For Windows
#include <stdio.h>												// Header File For Standard Input / Output
#include <stdarg.h>												// Header File For Variable Argument Routines
#include <math.h>												// Header File For Math Operations
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include "glut.h"
#include "baseTypes.h"
#include "openglframework.h"	
#include "gamedefs.h"
#include "collInfo.h"
#include "object.h"
#include "random.h"
#include "gameObjects.h"
#include "openGLStuff.h"
#include "game.h"
#include "StateManager.h"
#include "PlayerManager.h"
#include "ScreenManager.h"
#include "SoundManager.h"

// Declarations
const char8_t CGame::mGameTitle[]="Kirby Kickout";
CGame* CGame::sInstance=NULL;
BOOL Initialize (GL_Window* window, Keys* keys)					// Any OpenGL Initialization Goes Here
{
	initOpenGLDrawing(window,keys,0.0f, 0.0f, 0.0f);
	CGame::CreateInstance();
	CGame::GetInstance()->init();
	return TRUE;						
}

void CGame::init()
{
	ScreenManagerC::CreateInstance();
	PlayerManagerC::CreateInstance();
	SoundManagerC::CreateInstance();

	ScreenManagerC::GetInstance()->init();
	SoundManagerC::GetInstance()->init();
}
void CGame::UpdateFrame(DWORD milliseconds)			
{
	keyProcess();
	ScreenManagerC::GetInstance()->update(milliseconds);
}

void CGame::DrawScene(void)											
{
	startOpenGLDrawing();
	ScreenManagerC::GetInstance()->renderScreen();
}

CGame *CGame::CreateInstance()
{
	sInstance = new CGame();
	return sInstance;
}
void CGame::shutdown()
{
	ScreenManagerC::GetInstance()->shutdown();
	SoundManagerC::GetInstance()->shutdown();
}
void CGame::DestroyGame(void)
{
	delete ScreenManagerC::GetInstance();
	delete PlayerManagerC::GetInstance();
	delete SoundManagerC::GetInstance();
}