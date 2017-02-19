#pragma once
/*
	Sprite.h		written by Louis Hofer

	This class is used to draw the contents of a given sprite sheet.
	It is also used to specify the hitbox of the sprite so animations within the sprite sheet can be more accurately reflected.
*/

#include <windows.h>
#include <Xinput.h>
#include "types.h"
#include "Object.h"
#include "baseTypes.h"
#include "glut.h"

class SpriteC
{
public:
	/* Public functions */
	SpriteC(char *spriteMapFilePath, float height, float width, int rows, int columns);
	~SpriteC();

	void render(Coord2D position, float u, float v, bool useBuffer = true);

	int getRows();

	GLuint getSpriteMap();

	/* Public data members */
	float mHeight, mWidth, mStartX, mStartY;

	Coord2D mHitBoxStart, mHitBoxEnd;

private:
	/* Private data members */
	GLuint mSpriteMap;

	int mRows, mColumns;

	/* Private constant data */
	const int numberOfPixelsAsBuffer = 2;

	const float horizontalRatio = (4000.0f / 1024.0f);
	const float verticalRatio = (4000.0f / 768.0f);
};