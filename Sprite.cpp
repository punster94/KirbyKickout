/*
	Sprite.cpp		written by Louis Hofer

	This file contains the implementation of the functions prototyped in the SpriteC class.
*/

#include "Sprite.h"
#include "SOIL.h"

/* Public functions */
/*
	Upon creation of a sprite, the sprite sheet is loaded into memory.
	This can be slow if the sprite sheet is of a significant size.
*/
SpriteC::SpriteC(char *spriteMapFilePath, float height, float width, int rows, int columns)
{
	mSpriteMap = SOIL_load_OGL_texture(spriteMapFilePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	mHeight = height;
	mWidth = width;
	mStartX = 0;
	mStartY = 0;
	mRows = rows;
	mColumns = columns;
}

SpriteC::~SpriteC(){}

/*
	Renders the sprite given a position to be drawn and the row and column into the sheet.
	If the useBuffer flag is set, rendering will skip the drawing of a specified number of pixels all four sides of the texture.
*/
void SpriteC::render(Coord2D position, float u, float v, bool useBuffer)
{
	int bufferPixels = numberOfPixelsAsBuffer;

	if (!useBuffer)
		bufferPixels = 0;

	mStartX = u;
	mStartY = v;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mSpriteMap);
	glBegin(GL_QUADS);

	GLfloat xPositionLeft = ((position.x + bufferPixels) * horizontalRatio);
	GLfloat xPositionRight = ((position.x + mWidth - bufferPixels) * horizontalRatio);

	GLfloat yPositionTop = ((position.y + bufferPixels) * verticalRatio);
	GLfloat yPositionBottom = ((position.y - mHeight - bufferPixels) * verticalRatio);

	float horizontalBuffer = bufferPixels / (mColumns * mWidth);
	float verticalBuffer = bufferPixels / (mRows * mHeight);

	GLfloat xTextureCoord = (mStartX / mColumns);
	GLfloat yTextureCoord = ((mRows - mStartY - 1) / mRows);

	glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
	glTexCoord2f(xTextureCoord + horizontalBuffer, yTextureCoord + verticalBuffer);
	glVertex2f(xPositionLeft, yPositionBottom);
	glTexCoord2f(xTextureCoord + (1.0f / mColumns) - horizontalBuffer, yTextureCoord + verticalBuffer);
	glVertex2f(xPositionRight, yPositionBottom);
	glTexCoord2f(xTextureCoord + (1.0f / mColumns) - horizontalBuffer, yTextureCoord + (1.0f / mRows) - verticalBuffer);
	glVertex2f(xPositionRight, yPositionTop);
	glTexCoord2f(xTextureCoord + horizontalBuffer, yTextureCoord + (1.0f / mRows) - verticalBuffer);
	glVertex2f(xPositionLeft, yPositionTop);

	glEnd();
}

int SpriteC::getRows()
{
	return mRows;
}

GLuint SpriteC::getSpriteMap()
{
	return mSpriteMap;
}