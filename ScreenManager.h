#pragma once
/*
	ScreenManager.h		written by Louis Hofer

	This is a singleton class that is responsible for managing the state of the game.
	It renders the background screen every frame as well as initiating any processes between game states.
*/

#include "glut.h"
#include "Sprite.h"

/*
	Screen State enumeration to specify the current game state.
*/
namespace ScreenState
{
	enum ScreenState { Invalid, StartScreen, LoadingScreen, ControlScreen, GameScreen, EndScreen, MaxState };
}

class ScreenManagerC
{
public:
	/* Public functions */
	static ScreenManagerC	*CreateInstance();
	static ScreenManagerC	*GetInstance() { return sInstance; };
	~ScreenManagerC() {};

	void init();
	void update(DWORD milliseconds);
	void shutdown();
	void renderScreen();

	void returnToMainMenu();

private:
	/* Private functions */
	ScreenManagerC() {};
	void startScreenUpdate();
	void controlScreenUpdate();
	void loadingScreenUpdate();
	void gameScreenUpdate(DWORD milliseconds);
	void endScreenUpdate();
	void getControllerState();
	void renderStartScreen();
	void renderControlScreen();
	void renderLoadingScreen();
	void renderGameScreen();
	void renderEndScreen();
	void renderComponent(GLuint texture,float startX, float startY, float endX, float endY, int bufferPixels = 0);

	GLuint loadTexture(char *path);

	/* Private data members */
	static ScreenManagerC *sInstance;
	bool mWasRendered;
	bool mInputReceived;

	int mButtonProgression;

	GLuint mStartScreenTexture;
	GLuint mControlScreenTexture;
	GLuint mGameScreenTexture;
	GLuint mEndScreenTexture;
	GLuint mStartScreenButtonTexture1;
	GLuint mStartScreenButtonTexture2;
	GLuint mStartScreenButtonTexture3;
	GLuint mLoadingScreenTexture;

	SpriteC *mWinningPlayerSprite;
	SpriteC *mDigits;

	ScreenState::ScreenState mCurrentScreenState;

	XINPUT_STATE mControllerState;
	XINPUT_STATE mPreviousControllerState;

	/* Private constant data */
	const short numButtons = 3;
	const short deadValue = 15000;

	const float horizontalRatio = (4000.0f / 1024.0f);
	const float verticalRatio = (4000.0f / 768.0f);

	char *startScreenPath = "Screens/TitleScreen.png";
	char *controlScreenPath = "Screens/ControlScreen.png";
	char *gameScreenPath = "Screens/GameScreen.png";
	char *loadingScreenPath = "Screens/LoadingScreen.png";
	char *endScreenPath = "Screens/EndScreen.png";
	char *startScreenButton1Path = "Screens/StartButtons1.png";
	char *startScreenButton2Path = "Screens/StartButtons2.png";
	char *startScreenButton3Path = "Screens/StartButtons3.png";
	char *winningPlayerPath = "SpriteSheets/WinningPlayer.png";
	char *digitsPath = "SpriteSheets/digits.png";
};