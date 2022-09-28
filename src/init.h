const std::string gameName = "KeepBoosting";

//int SCREEN_WIDTH = 256;
//int SCREEN_HEIGHT = 272;

int SCREEN_WIDTH = 512;
int SCREEN_HEIGHT = 544;

//int SCREEN_WIDTH = 768;
//int SCREEN_HEIGHT = 816;

int count_digit(int number) {
   return int(log10(number) + 1);
}

float roundMe(int var)
{
    // 37.66666 * 100 =3766.66
    // 3766.66 + .5 =3767.16    for rounding off value
    // then type cast to int so value is 3767
    // then divided by 100 so the value converted into 37.67
	int value = (int)(var * 100 + .5);

    return  (int)value / 100;
}

// Texture wrapper class
class LTexture {
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

#ifdef _SDL_TTF_H
	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor,
			TTF_Font *fontname);
#endif

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, int w, int h, SDL_Rect* clip = NULL,
			double angle = 0.0, SDL_Point* center = NULL,
			SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

class LWindow {
public:
	//Intializes internals
	LWindow();

	//Creates window
	bool init();

	//Creates renderer from internal window
	SDL_Renderer* createRenderer();

	//Handles window events
	void handleEvent(SDL_Event& e);

	//Deallocates internals
	void free();

	//Window dimensions
	int getWidth();
	int getHeight();

	//Window focii
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();

private:
	//Window data
	SDL_Window* mWindow;

	//Window dimensions
	int mWidth;
	int mHeight;

	//Window focus
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Our custom window
LWindow gWindow;

//The window renderer
SDL_Renderer* gRenderer = NULL;

LTexture::LTexture() {
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture() {
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path) {
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
		IMG_GetError());
	} else {
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE,
				SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n",
					path.c_str(), SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText,
		SDL_Color textColor, TTF_Font *fontname) {
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(fontname,
			textureText.c_str(), textColor);
	if (textSurface != NULL) {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			//printf(
			//		"Unable to create texture from rendered text! SDL Error: %s\n",
			//		SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	} else {
		//printf("Unable to render text surface! SDL_ttf Error: %s\n",
		//TTF_GetError());
	}

	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free() {
	//Free texture if it exists
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, int w, int h, SDL_Rect* clip, double angle,
				SDL_Point* center, SDL_RendererFlip flip) {


	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, w, h };

	//Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = w;
		renderQuad.h = h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center,
			flip);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

LWindow::LWindow() {
	//Initialize non-existant window
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
}

bool LWindow::init() {
	//Create window
	mWindow = SDL_CreateWindow(gameName.c_str(), SDL_WINDOWPOS_UNDEFINED,
	SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN );
	if (mWindow != NULL) {
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT;
	}

	return mWindow != NULL;
}

SDL_Renderer* LWindow::createRenderer() {
	return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void LWindow::handleEvent(SDL_Event& e) {
	//Window event occured
	if (e.type == SDL_WINDOWEVENT) {
		//Caption update flag
		bool updateCaption = false;

		switch (e.window.event) {
		//Get new dimensions and repaint on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent(gRenderer);
			break;

			//Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(gRenderer);
			break;

			//Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;

			//Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

			//Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;

			//Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;

			//Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			mMinimized = true;
			break;

			//Window maxized
		case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
			break;

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
			break;
		}

		//Update window caption with new data
		if (updateCaption) {
			std::stringstream caption;
			caption << gameName.c_str();

			SDL_SetWindowTitle(mWindow, caption.str().c_str());
		}
	}
	//Enter exit full screen on return key
	//else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
	else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11) {
		if (mFullScreen) {
			//SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
			//mFullScreen = false;
		} else {
			//SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
			//mFullScreen = true;
			//mMinimized = false;
		}
	}
}

void LWindow::free() {
	if (mWindow != NULL) {
		SDL_DestroyWindow(mWindow);
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth() {
	return mWidth;
}

int LWindow::getHeight() {
	return mHeight;
}

bool LWindow::hasMouseFocus() {
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus() {
	return mKeyboardFocus;
}

bool LWindow::isMinimized() {
	return mMinimized;
}

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init( SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		//Set texture filtering to linear
		SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0");

		/////////////////////////////
		////////// DRAWING //////////
		//Create window
		gWindow.init();
		//Create vsynced renderer for window
		gRenderer = gWindow.createRenderer();
		//Initialize renderer color
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		////////// DRAWING //////////
		/////////////////////////////

		//Initialize PNG loading
		int imgFlags = IMG_INIT_PNG;
		IMG_Init(imgFlags);
		TTF_Init();

		//SDL_Init(SDL_INIT_EVERYTHING);

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
			Mix_GetError());
			success = false;
		}

	}

	return success;
}

void checkMaxLimitDouble(double &value, double maxLimitNeg, double maxLimitNegNew, double maxLimitPos, double maxLimitPosNew){
	//Set max limits for one number (double)
	if (value <= maxLimitNeg){
		value = maxLimitNegNew;
	}
	if (value >= maxLimitPos){
		value= maxLimitPosNew;
	}
}

// Check collision between 2 objects
bool checkCollision(int x, int y, int w, int h, int x2, int y2, int w2, int h2){
	bool collide;
	if (x+w > x2 && x < x2 + w2 && y+h > y2 && y < y2 + h2){
		collide = true;
	}else{
		collide = false;
	}
	return collide;
}

void RenderRect(SDL_Renderer *gRenderer, float x, float y, float w, float h, SDL_Color color) {
	SDL_Rect tempRect = {x, y, w, h};
	SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, 255);
	SDL_RenderDrawRect(gRenderer, &tempRect);
}

void RenderFillRect(SDL_Renderer *gRenderer, float x, float y, float w, float h, SDL_Color color) {
	SDL_Rect tempRect = {x, y, w, h};
	SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(gRenderer, &tempRect);
}

// Colors
SDL_Color black = { 0, 0, 0, 255 };
SDL_Color blue = { 0, 0, 255, 255 };
SDL_Color green = { 0, 255, 0, 255 };
SDL_Color red = { 255, 0, 0, 255 };
SDL_Color white = { 255, 255, 255, 255 };
SDL_Color orange = { 180, 90, 20, 255 };

// Textures
LTexture gShip;
SDL_Rect rShip[3][4];

LTexture gPowerUp;
SDL_Rect rPowerUp[2];
int rPowerUpIndex = 0;
int rPowerUpTimer = 0;

LTexture gText;
LTexture gControls;

// BG Textures
LTexture gCloudsTransparent;
LTexture gClouds;
LTexture gDesertBG;

// Fonts
TTF_Font *gFont12 = NULL;
TTF_Font *gFont21 = NULL;

// Sound
Mix_Chunk *sGas = NULL;
Mix_Chunk *sGas2 = NULL;
Mix_Chunk *sGearUp = NULL;
Mix_Chunk *sGearDown = NULL;;
Mix_Chunk *sExplosion = NULL;
Mix_Chunk *sTime = NULL;
Mix_Music *sMusic = NULL;

bool loadMedia() {
	bool success = true;

	// Textures
	gShip.loadFromFile("resource/ship.png");
	gPowerUp.loadFromFile("resource/powerUp.png");
	gControls.loadFromFile("resource/controls.png");
	gCloudsTransparent.loadFromFile("resource/clouds-transparent.png");
	gClouds.loadFromFile("resource/clouds.png");
	gDesertBG.loadFromFile("resource/desertBG.png");

	// Clips

	// Powerup
	rPowerUp[0] = {0,0,16,16};
	rPowerUp[1] = {16,0,16,16};

	// Ship
	// left
	rShip[0][0] = {0,0,16,24};
	rShip[0][1] = {0,24,16,24};
	rShip[0][2] = {0,48,16,24};
	rShip[0][3] = {0,72,16,24};

	// center
	rShip[1][0] = {16,0,16,24};
	rShip[1][1] = {16,24,16,24};
	rShip[1][2] = {16,48,16,24};
	rShip[1][3] = {16,72,16,24};

	// right
	rShip[2][0] = {32,0,16,24};
	rShip[2][1] = {32,24,16,24};
	rShip[2][2] = {32,48,16,24};
	rShip[2][3] = {32,72,16,24};

	// Fonts
	gFont12 = TTF_OpenFont("resource/PressStart2P.ttf", 12);
	gFont21 = TTF_OpenFont("resource/PressStart2P.ttf", 21);

	// Audio
	sGas = Mix_LoadWAV("resource/gearNow.wav");
	sGas2 = Mix_LoadWAV("resource/gas2.wav");
	sGearUp = Mix_LoadWAV("resource/gearUp.wav");
	sGearDown = Mix_LoadWAV("resource/gearDown.wav");
	sExplosion = Mix_LoadWAV("resource/explosion.wav");
	sTime = Mix_LoadWAV("resource/snd_time.wav");
	sMusic = Mix_LoadMUS("resource/spaceship shooter.mp3");

	return success;
}

void close() {
	gShip.free();
	gText.free();
	gControls.free();
	gCloudsTransparent.free();
	gClouds.free();
	gDesertBG.free();
	TTF_CloseFont(gFont12);
	TTF_CloseFont(gFont21);
	Mix_FreeChunk(sGas);
	Mix_FreeChunk(sGas2);
	Mix_FreeChunk(sGearUp);
	Mix_FreeChunk(sGearDown);
	Mix_FreeChunk(sExplosion);
	Mix_FreeChunk(sTime);
	Mix_FreeMusic(sMusic);

	gFont12 = NULL;
	gFont21 = NULL;
	sGas 	= NULL;
	sGas2 	= NULL;
	sGearUp 	= NULL;
	sGearDown 	= NULL;
	sExplosion 	= NULL;
	sTime 	= NULL;
	sMusic 	= NULL;

	SDL_DestroyRenderer(gRenderer);
	gWindow.free();

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

// Level size
int mapX = 0;
int mapY = 0;
int mapW = SCREEN_WIDTH;
int mapH = SCREEN_HEIGHT;

// trashBin bin variables
int trashBinW = 120;
int trashBinH = 80;
int trashBinX = mapW * 1.00 - trashBinW - 5;
int trashBinY = mapH * 1.00 - trashBinH - 5;

// Stats bin variables
int statsW = 120;
int statsH = 80;
int statsX = 5;
int statsY = mapH * 1.00 - statsH - 5;

// Player variables
int w = 48;
int h = 48;
float x = 0+SCREEN_WIDTH/2 - w/2;
float y = SCREEN_HEIGHT * 0.76 - h/2;
bool moveL = false;
bool moveR = false;
bool moveU = false;
bool moveD = false;
float velX = 0.0, velY = 0.0;
float speedY = 1.38;
float fuel = 100;
const float fuelMax = 100;

// Player speed gears
bool gas = false;
int gear = 1;
float rpm = 0;

// Player animations
int sprite_index_x = 0;
int sprite_index_y = 0;
float sprite_index_timer = 0;

// AI variables
int w2 = 24;
int h2 = 24;
float x2 = mapW/2 - w2/2;
float y2 = mapH * 0.20 - h2/2;

/*
 * 0: Playing Game scene
 * 2: Lost scene
 */
int gameResult = 0;
int highscore = -1;
int previousHighScore = -1;
int score = 0;

bool protection = true;
int protectionTimer = 0;
bool quit = false;
bool hideTip = false;

// Background texture Y position
float bgY = 0;

void SaveHighScore() {
	bool saveHighscore = false;

	// Open highscore first to check value
	int tempScore = -1;
	std::fstream fileOpen("data/highscore.txt");
	fileOpen >> tempScore;
	fileOpen.close();

	// If current score is higher than previously saved score, save it
	if (score > tempScore) {
		saveHighscore = true;
	}

	// Now save high score
	if (saveHighscore) {
		std::ofstream fileSave;
		fileSave.open("data/highscore.txt");
		fileSave << score;
		fileSave.close();
	}
}

void LoadHighScore() {
	std::fstream fileTileDataL("data/highscore.txt");
	fileTileDataL >> previousHighScore;
	fileTileDataL.close();
}

void ContinueGame() {
	// Win, continue playing
	if (gameResult == 0) {
		// Load highscore
		LoadHighScore();

		// Play SFX
		Mix_PlayChannel(-1, sTime, 0);

		// Add player score
		score += 10;

		// Set gamemode to play game
		gameResult = 0;

		// Random size for house
		trashBinW = rand() % 130 + 20;
		trashBinH = rand() % 40 + 40;

		// Random x pos for House
		int num1;
		int num2;
		num1 = rand() % (560 - trashBinW);
		trashBinX = 50 + num1;

		// Random y pos for House
		num2 = rand() % (270 - trashBinH);
		trashBinY = 40 + num2;

		// Reset AI position
		x2 = trashBinX + trashBinW/2;
		y2 = trashBinW + trashBinH/2;

		// Reset protection
		protection = true;
		protectionTimer = 0;
	}

	// Lose, reset game
	else if (gameResult == 1)
	{
		// Save highscore
		SaveHighScore();

		LoadHighScore();

		// Play SFX
		Mix_PlayChannel(-1, sTime, 0);

		// Set gamemode to play game
		gameResult = 0;

		// Random size for house
		//trashBinW = rand() % 130 + 20;
		//trashBinH = rand() % 40 + 40;

		// Random x pos for House
		//int num1;
		//int num2;
		//num1 = rand() % (560 - trashBinW);
		//trashBinX = 50 + num1;

		// Random y pos for House
		//num2 = rand() % (270 - trashBinH);
		//trashBinY = 40 + num2;

		// Reset AI position
		x2 = (rand() % (SCREEN_WIDTH-w2*2));
		y2 = -500;

		// Reset Player
		gas = false;
		fuel = 100;
		score = 0;

		// Reset protection
		protection = true;
		protectionTimer = 0;
	}
}

void Update() {

	// Update BG
	if (gas) {

		// Burn gas
		if (fuel > 0) {
			fuel -= 0.25;
		} else {
			gas = false;
			Mix_Pause(3);
		}

		// Add score
		score += 1;

		// Increase speed
		speedY += 0.05;

		if (speedY >= 10 * gear) {
			speedY = 10 * gear;

			Mix_PlayChannel(-1, sGas, 0);
		}
	} else {
		if (speedY > 2) {
			speedY = speedY - speedY * 0.0015;
		}
	}

	bgY += speedY/2;

	if (bgY > SCREEN_HEIGHT) {
		bgY = 0;
	}

	// Game state: Playing game
	if (gameResult == 0) {

		// Fuel animations
		rPowerUpTimer += 15;
		if (rPowerUpTimer > 60) {
			rPowerUpTimer = 0;

			rPowerUpIndex++;
			if (rPowerUpIndex > 1) {
				rPowerUpIndex = 0;
			}
		}

		// Player animations

		// If no gas, do sprite [2]
		if (!gas) {
			sprite_index_timer += 15;
			if (sprite_index_timer > 60) {
				sprite_index_timer = 0;

				sprite_index_y++;
				if (sprite_index_y > 3) {
					sprite_index_y = 2;
				}
			}
		}

		// If gas do animations
		else {
			sprite_index_timer += 15;
			if (sprite_index_timer > 60) {
				sprite_index_timer = 0;

				sprite_index_y++;
				if (sprite_index_y > 1) {
					sprite_index_y = 0;
				}
			}
		}

		// Update Player controls
		if (moveU){
			velY -= 3;
		}else if (moveD){
			velY += 3;
		}
		if (moveL){
			velX -= 3;
		}else if (moveR){
			velX += 3;
		}

		// Max velocity
		if (velX > 6){
			velX = 6;
		}
		if (velX < -6){
			velX = -6;
		}
		if (velY > 6){
			velY = 6;
		}
		if (velY < -6){
			velY = -6;
		}

		// Player velocity
		x += velX;
		y += velY;

		// Drag coefficient
		if (!moveU && !moveD){
			velY = velY - velY * 0.5;
		}
		if (!moveL && !moveR){
			velX = velX - velX * 0.5;
		}

		// Player bounds
		if( x < 0 ){
			x = 0;
		}
		if( y < 0 ){
			y = 0;
		}
		if( x+w > mapW ){
			x = mapW-w ;
		}
		if( y+h  > mapH ){
			y = mapH-h ;
		}

		// If player protection on
		if (protection) {
			protectionTimer += 1;

			// Protect player for 1.5 seconds
			if (protectionTimer > 45) {
				protectionTimer = 0;
				protection = false;
			}
		}

		// Follow player
		{
			// Follow player
			float vX=0, vY=0;
			float bmx  = x+w/2;
			float bmy  = y+h/2;
			float distance = sqrt((bmx - x2-w2/2) * (bmx - x2-w2/2)+
								  (bmy - y2-h2/2) * (bmy - y2-h2/2));
			//float distanceY = sqrt((bmy - y2-h2/2) * (bmy - y2-h2/2));

			// Start following player
			/*if (distance > 10){
				if (hideTip) {

					// Move towards player
					vX 	= 5 * (bmx - x2-w2/2) / distance;
					vY 	= 5 * (bmy - y2-h2/2) / distance;
					x2 += vX;
					y2 += vY;
				}
			}*/

			// Add speed to fuel powerup
			y2 += 5 + speedY * 0.05;

			if (y2 > SCREEN_HEIGHT + h2) {
				x2 = (rand() % (SCREEN_WIDTH-w2*2));
				y2 = -500;
			}

			// Check if player is not protected first
			//if (!protection)
			//{
				// AI touched the garbage!
				if (checkCollision(x, y, w, h, x2, y2, w2, h2))
				{
					// Add fuel
					fuel += 50;
					if (fuel > 100) {
						fuel = 100;
					}

					// Change x & y position to somewhere random
					x2 = (rand() % (SCREEN_WIDTH-w2*2));
					y2 = -500;

					// Play SFX
					Mix_PlayChannel(-1, sTime, 0);

					// Show losing screen
					//gameResult = 1;
				}

				// If gas reaches 0, player loses and explodes
				if (fuel <= 0) {

					// Play SFX
					Mix_PlayChannel(-1, sExplosion, 0);

					// Show losing screen
					gameResult = 1;
				}

				// Event, Check if Player1 entered House (Arena)
				//else if (checkCollision(x, y, w, h, trashBinX, trashBinY, trashBinW, trashBinH))
				//{
				//	ContinueGame();
				///}
			//}
		}
	}

	if (score > 9999999) {
		score = 9999999;
	}
}

void Render() {
	// Render textures
	{
		// Render background textures
		gDesertBG.render(0, bgY-SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
		gDesertBG.render(0, bgY, SCREEN_WIDTH, SCREEN_HEIGHT);
		gDesertBG.render(0, bgY+SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);

		// Render Map
		//SDL_Rect tempMap = { mapX, mapY, mapW, mapH };
		//SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		//SDL_RenderDrawRect(gRenderer, &tempMap);

		// Render trash bin AKA mission
		SDL_Rect tempHouse = { trashBinX, trashBinY, trashBinW, trashBinH };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempHouse);

		// Render stats
		tempHouse = { statsX, statsY, statsW, statsH };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempHouse);

		// Render protection
		if (protection) {
			SDL_Rect tempR = { x, y, w, h };
			SDL_SetRenderDrawColor(gRenderer, 0, 200, 200, 255);
			SDL_RenderDrawRect(gRenderer, &tempR);
		}
		// Render Player1
		gShip.render(x, y, w, h, &rShip[sprite_index_x][sprite_index_y]);

		// Render Player2
		gPowerUp.render(x2, y2, w2, h2, &rPowerUp[rPowerUpIndex]);
	}

	// Render bars
	{
		// Combo window counter (on top of player)
		{
			const float yOffsetBar = 20;
			const float barHeight = 10;
			const float barWidth = statsW * 0.30;
			float uiX = statsX + (statsW * 0.08);
			float uiY = statsY + 60;
			int tempN = fuel;

			// Decay bar
			{
				// Health Decay bar, bg
				RenderFillRect(gRenderer, uiX, uiY, (barWidth*fuelMax)/fuelMax, barHeight, {60, 60, 60} );
			}

			// Main bar
			{
				RenderFillRect(gRenderer, uiX, uiY, (barWidth* (tempN) )/fuelMax, barHeight, {180, 180, 30} );
			}

			// Render current gear
			std::stringstream tempss;
			tempss << "Gas";
			gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
			gText.render(statsX + statsW - gText.getWidth() - 16, uiY, gText.getWidth(), gText.getHeight());
		}
	}

	// Playing game scene
	//if (gameResult == 0)
	//{
		// Render text above house
		std::stringstream tempss;
		tempss << "Mission";
		gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
		gText.render(trashBinX+trashBinW/2-gText.getWidth()/2, trashBinY-gText.getHeight()-2, gText.getWidth(), gText.getHeight());

			// Render text above player 1
			tempss.str(std::string());
			tempss << "Boosting!";
			gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
			gText.render(trashBinX + 5, trashBinY+5, gText.getWidth(), gText.getHeight());

		// Render Stats text
		tempss.str(std::string());
		tempss << "Stats";
		gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
		gText.render(statsX+statsW/2-gText.getWidth()/2, statsY-gText.getHeight()-2, gText.getWidth(), gText.getHeight());

			// Km text
			{
				// Render km text
				tempss.str(std::string());
				tempss << int(speedY);
				gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
				gText.render(statsX + 5, statsY+5, gText.getWidth(), gText.getHeight());

				// Render current km
				tempss.str(std::string());
				tempss << "km/H";
				gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
				gText.render(statsX + statsW - gText.getWidth() - 5, statsY+5, gText.getWidth(), gText.getHeight());
			}

			// Gear text
			{
				// Render gear text
				tempss.str(std::string());
				tempss << gear;
				gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
				gText.render(statsX + 5, statsY + gText.getHeight() +5, gText.getWidth(), gText.getHeight());

				// Render current gear
				tempss.str(std::string());
				tempss << "Gear";
				gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
				gText.render(statsX + statsW - gText.getWidth() - 5, statsY + gText.getHeight() +5, gText.getWidth(), gText.getHeight());
			}

		// Render text above player AI
		//tempss.str(std::string());
		//tempss << "fuel";
		//gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
		//gText.render(x2+w2/2-gText.getWidth()/2, y2 - gText.getHeight(), gText.getWidth(), gText.getHeight());

		// Render score text top-right of screen
		tempss.str(std::string());
		if (count_digit(previousHighScore) == 1) {
			tempss << "Highscore: 00000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 2) {
			tempss << "Highscore: 00000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 3) {
			tempss << "Highscore: 0000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 4) {
			tempss << "Highscore: 000" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 5) {
			tempss << "Highscore: 00" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 6) {
			tempss << "Highscore: 0" << previousHighScore;
		}
		else if (count_digit(previousHighScore) == 7) {
			tempss << "Highscore: " << previousHighScore;
		} else {
			tempss << "Highscore: 000000" << previousHighScore;
		}

		gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
		gText.render(gWindow.getWidth() - gText.getWidth() - 10, 4, gText.getWidth(), gText.getHeight());

		// Render score text top-right of screen
		tempss.str(std::string());
		if (count_digit(score) == 1) {
			tempss << "Score: 000000" << score;
		}
		else if (count_digit(score) == 2) {
			tempss << "Score: 00000" << score;
		}
		else if (count_digit(score) == 3) {
			tempss << "Score: 0000" << score;
		}
		else if (count_digit(score) == 4) {
			tempss << "Score: 000" << score;
		}
		else if (count_digit(score) == 5) {
			tempss << "Score: 00" << score;
		}
		else if (count_digit(score) == 6) {
			tempss << "Score: 0" << score;
		}
		else if (count_digit(score) == 7) {
			tempss << "Score: " << score;
		} else {
			tempss << "Score: 000000" << score;
		}
		gText.loadFromRenderedText(tempss.str().c_str(), white, gFont12);
		gText.render(gWindow.getWidth() - gText.getWidth() - 10, 16, gText.getWidth(), gText.getHeight());
	//}

	// Lost scene
	if (gameResult == 1) {
		std::stringstream tempss;
		tempss << "You lose. Boo hoo.";
		gText.loadFromRenderedText(tempss.str().c_str(), white, gFont21);
		gText.render(gWindow.getWidth()/2 - gText.getWidth()/2,
				 gWindow.getHeight() * 0.35 - gText.getHeight()-22,
					 gText.getWidth(), gText.getHeight());

		tempss.str(std::string());
		tempss << "Press Space to Start.";
		gText.loadFromRenderedText(tempss.str().c_str(), white, gFont21);
		gText.render(gWindow.getWidth()/2 - gText.getWidth()/2,
				 gWindow.getHeight() * 0.35 - gText.getHeight(),
					 gText.getWidth(), gText.getHeight());
	}

	// Show tip
	if (!hideTip) {
		gControls.render(0 ,0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

}
