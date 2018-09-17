#include <stdio.h>
#include <string>
#include <SDL.h>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;

const int TILE_WIDTH = 200;
const int TILE_HEIGHT = 200;

class LTexture
{
public:
	LTexture();
	~LTexture();
	bool loadFromFile(std::string path);
	void free();
	void render(int x, int y);
	int getWidth();
	int getHeight();

private:
	SDL_Texture* mTexture;
	int mWidth, mHeight;
};

class LTile
{
public:
	LTile();
	void setPosition(int x, int y);
	void handleEvent(SDL_Event* e);
	void render();

private:
	int mPosX, mPosY;
	bool xHit;
	bool oHit;
	bool turnFinished;
};

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
LTexture gGrid;
LTexture gX;
LTexture gO;
LTile gTiles[9];
int gCurrentPlayer = 0;

bool init();
bool loadMedia();
void close();

int main(int argc, char* args[])
{
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			bool quit = false;
			SDL_Event e;

			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					for (int i = 0; i < 9; i++)
					{
						gTiles[i].handleEvent(&e);
					}
				}

				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);
				gGrid.render(0, 0);
				for (int i = 0; i < 9; i++)
				{
					gTiles[i].render();
				}
				SDL_RenderPresent(gRenderer);
			}
		}
	}
	close();
	return 0;
}

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	free();
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0, 0xFF));
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}
		SDL_FreeSurface(loadedSurface);
	}
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::render(int x, int y)
{
	SDL_Rect RenderQuad = { x, y, mWidth, mHeight };
	SDL_RenderCopy(gRenderer, mTexture, NULL, &RenderQuad);
}

LTile::LTile()
{
	mPosX = 0;
	mPosY = 0;
	xHit = false;
	oHit = false;
	turnFinished = false;
}

void LTile::setPosition(int x, int y)
{
	mPosX = x;
	mPosY = y;
}

void LTile::handleEvent(SDL_Event *e)
{
	if (!turnFinished)
	{
		if (e->type == SDL_MOUSEBUTTONDOWN)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			bool inside = true;
			if (x < mPosX)
			{
				inside = false;
			}
			else if (x > mPosX + TILE_WIDTH)
			{
				inside = false;
			}
			else if (y < mPosY)
			{
				inside = false;
			}
			else if (y > mPosY + TILE_HEIGHT)
			{
				inside = false;
			}

			if (inside)
			{

				if (gCurrentPlayer == 0)
				{
					xHit = true;
					turnFinished = true;
					gCurrentPlayer = 1;
				}
				else
				{
					oHit = true;
					turnFinished = true;
					gCurrentPlayer = 0;
				}
			}
		}
	}
}

void LTile::render()
{
	if (xHit)
	{
		gX.render(mPosX, mPosY);
	}
	if (oHit)
	{
		gO.render(mPosX, mPosY);
	}
}

bool init()
{
	bool success = true;
	if (SDL_INIT_VIDEO < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}
		gWindow = SDL_CreateWindow("Tic Tac Toe :DDD", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}
	return success;
}

bool loadMedia()
{
	bool success = true;
	if (!gGrid.loadFromFile("grid.bmp"))
	{
		printf("Failed to load grid texture!\n");
		success = false;
	}
	if (!gX.loadFromFile("x.bmp"))
	{
		printf("Failed to load X texture!\n");
		success = false;
	}
	if (!gO.loadFromFile("o.bmp"))
	{
		printf("Failed to load O texture!\n");
		success = false;
	}
	gTiles[0].setPosition(0, 0);
	gTiles[1].setPosition(TILE_WIDTH, 0);
	gTiles[2].setPosition(TILE_WIDTH * 2, 0);
	gTiles[3].setPosition(0, TILE_HEIGHT);
	gTiles[4].setPosition(TILE_WIDTH, TILE_HEIGHT);
	gTiles[5].setPosition(TILE_WIDTH * 2, TILE_HEIGHT);
	gTiles[6].setPosition(0, TILE_HEIGHT * 2);
	gTiles[7].setPosition(TILE_WIDTH, TILE_HEIGHT * 2);
	gTiles[8].setPosition(TILE_WIDTH * 2, TILE_HEIGHT * 2);
	return success;
}

void close()
{
	gGrid.free();
	gX.free();
	gO.free();

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gRenderer = NULL;
	gWindow = NULL;

	SDL_Quit();
}