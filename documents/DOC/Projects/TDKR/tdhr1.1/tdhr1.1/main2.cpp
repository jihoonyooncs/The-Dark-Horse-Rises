#include <SDL.h>
#include <glew.h>
#include <GL/gl.h>
#include <GL/GLU.h>
#include <fmod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <stack>
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm> 
#include <ctime>
#include "structandfunctions.h"
#include "backgrounds.h"
#undef main
using namespace std;
#include <fstream>
#include <iterator>
#include <string>
#define FRAME_VALUES 10

//GLOBAL VARIABLES
char shouldExit = 0;
unsigned char kbPrevState[SDL_NUM_SCANCODES] = { 0 };
const unsigned char* kbState = NULL;
Uint32 lastFrameMs;
Uint32 currentFrameMs = SDL_GetTicks();
double physicsDeltaTime = 1 / 100.0f;
int physicsDeltaMs = 10;
Uint32 lastPhysicsFrameMs;
Uint32 lastPhysicsFrameMs2;
double deltaTime = 0;
double interpolation = 0;

int main(void){

	if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Window* window = SDL_CreateWindow("TestSDL",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 550, SDL_WINDOW_OPENGL);
	if (!window) {
		fprintf(stderr, "Could not create window. ErrorCode=%\n",
			SDL_GetError());
		SDL_Quit();
		return 1;
	}
	SDL_GL_CreateContext(window);
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		fprintf(stderr, "Could not initialize glew. ErrorCode=%s\n",
			glewGetErrorString(glewError));
		SDL_Quit();
		return 1;
	}
	if (!GLEW_VERSION_3_0) {
		fprintf(stderr, "OpenGL max supported version is too low.\n");
		SDL_Quit();
		return 1;
	}
	glViewport(0, 0, 800, 550);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, 800, 550, 0, 0, 100);
	glEnable(GL_TEXTURE_2D);
	kbState = SDL_GetKeyboardState(NULL);

#include "images.h"

	//====================================PLAYER PROPERTIES ==============================================
	//====================================PLAYER PROPERTIES ==============================================

	GLuint playerTextures[] = { hr0, hr1, hr2, hr3, hr4, hr5, hr6, hj0, hj1, hj2, hj3, hj4, hj5, hj6, ht0, ht1, ht2, ht3,
		ht4, ht5, ht6, ht7, ht8, ht9, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, bhr1, bhr2, bhr3, bhr4, bhr5, bhr6, bhr7,
		bhj1, bhj2, bhj3, bhj4, bhj5, bhj6, bhj7, bht1, bht2, bht3, bht4, bht5, bht6, bht7, bht8, bht9, bht10, bht11, ht10, ht11, ht12,
		bhorsestill1, bhorsestill2, bhorsestill3, bhorsestill4, bhorsestill5, man1, man2, man3, man4, man5 }; //PLAYER TEXTURES FOR ANIMATION
	double gbt = .48;
	AnimDef horseFarewell{ 0, { { 63, gbt }, { 64, gbt }, { 65, gbt }, { 66, gbt }, { 67, 1.5 } }, 5 };
	AnimDef playerWalk = { 1, { { 0, .1 }, { 1, .1 }, { 2, .1 }, { 3, .1 }, { 4, .1 }, { 5, .1 }, { 6, .1 }, }, 7 };
	AnimDef playerWalkB = { 1, { { 35, .5 }, { 36, .5 }, { 37, .5 }, { 38, .5 }, { 39, .5 }, { 40, .5 }, { 41, .5 } }, 7 };
	AnimDef horseJumpp = { 0, { { 7, .08 }, { 8, .1 }, { 9, .12 }, { 10, .08 }, { 11, .08 }, { 12, .07 }, { 13, .05 } }, 7 };
	AnimDef horseJumpB = { 0, { { 42, .08 }, { 43, .1 }, { 44, .1 }, { 45, .08 }, { 46, .08 }, { 47, .07 }, { 48, .05 } }, 7 };
	AnimDef horseTwirlB = { 0, { { 49, .02 }, { 50, .02 }, { 51, .02 }, { 52, .02 }, { 53, .02 }, { 54, .02 }, { 55, .02 }, { 56, .02 }, { 57, .02 },
	{ 58, .02 }, { 59, .02 } }, 11 };
	AnimDef horseTwirl = { 0,
	{ { 14, .025 }, { 15, .025 }, { 16, .025 }, { 17, .025 }, { 18, .025 }, { 19, .025 }, { 20, .025 }, { 21, .025 }, { 22, .025 }, { 23, .025 },
	{ 60, .025 }, { 61, .025 }, { 62, .025 }, { 62, 1 } }, 14 };
	AnimDef horseExplosion = { 0, { { 25, .01 }, { 26, 1 }, { 27, .07 }, { 28, .07 }, { 29, .07 }, { 30, .07 }, { 31, .07 }, { 32, .07 }, { 33, .07 }, { 34, 1 }, { 34, .05 } }, 11 };
	AnimData playerData;
	Player player = {
		{ &playerWalkB, 0, 0, true },
		100, 100, player_width, player_height,
		0, {}, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	AnimDef ownerDef{ 0, { { 68, gbt }, { 69, gbt }, { 70, gbt }, { 71, gbt }, { 72, 1.5 } }, 5 };
	AnimData ownerData{ &ownerDef, 0, gbt, 0 };
	Object owner{ 0, 617, 365, 55, 87, ownerData };

	//OBJECT ANIMATIONS
	GLuint objectTextures[] = { f1, f2, m1, m2, m3, m1, m1, m1, m1, spike, spikedown, evilcloud, sp1, sp2, sp3, sp4, sp5, fb1, fb2, fb3, fb4, fb5, fb6,
		fb7, fb8, fb9, fb10, f3, f4, spike2, spikedown2, bird1, bird2, bird3, bird4, bird5, bird6, bird7, bird8, bird9, bird10, bird11,
		bird12, bird13, title1, title2, title3, title4, title5, title6, title7, title8, title9, title10, title11, title12, title13, title14,
		title15, title16, flypu, magpu, shieldpu, puff1, puff2, puff3, puff4, puff5, puff6, puff7, puff8, puff9, puff10, jumppuff1, jumppuff2, jumppuff3,
		jumppuff4, jumppuff5, jumppuff6, jumppuff7, jumppuff8, jumppuff9, jumppuff10, shield1, shield2, shield3, shield4, shield5, shield6, shield7
		, shield8, shield9, shield10, shield11, shield12, shield13, shield14, shield15, shield16, empty, ringex1, ringex2, ringex3, ringex4, ringex5, ringex6, };
	AnimDef mushStill = { 1, { { 2, .27 }, { 3, .27 }, { 4, .27 }, { 3, .27 } }, 4 };
	AnimData mushData = { &mushStill, 0, 0, 1 };
	AnimDef spikeFrames{ 0, { { 10, .1 }, { 9, .1 } }, 2 };
	AnimDef uspikeFrame{ 0, { { 30, .1 }, { 29, .1 } }, 2 };
	AnimData spikeData = { &spikeFrames, 0, 0, 0 };
	AnimData uspikeData{ &uspikeFrame, 0, 0, 0, };
	AnimDef eCloudFrame = { 0, { 11, .1 }, 1 };
	AnimData eCloudData = { &eCloudFrame, 0, 0, 0 };
	AnimDef springStill = { 1, { { 12, .2 }, { 13, .2 }, { 14, .2 }, { 13, .2 } }, 4 };
	AnimDef springExtend = { 0, { { 15, .1 }, { 16, .15 }, { 15, .1 }, { 14, .1 } }, 4 };
	AnimData springData = { &springStill, 0, 0, 1 };
	AnimDef fireballDef = { 1, { { 17, .07 }, { 18, .07 }, { 19, .07 }, { 20, .07 }, { 21, .07 }, { 22, .07 }, { 23, .07 }, { 24, .07 }, { 25, .07 }, { 26, .07 } }, 10 };
	AnimData fireballData = { &fireballDef, 0, 0, 1 };
	AnimDef flyDef = { 0, { 2, .1 }, 1 };
	AnimData flyData = { &flyDef, 0, 0, 0 };
	AnimDef magDef = { 0, { 6, .1 }, 1 };
	AnimData magData = { &magDef, 0, 0, 0 };
	AnimDef flowerDef = { 0, { { 0, 1 }, { 1, 1 } }, 2 };
	AnimData flowerData = { &flowerDef, 0, 0, 0 };
	AnimDef nFlowerDef = { 0, { { 28, 1 }, { 27, 1 } }, 2 };
	AnimData nFlowerData = { &nFlowerDef, 0, 0, 0 };

	AnimDef powerupDef{ 0, { { 60, .1 }, { 61, .1 }, { 62, .1 } }, 3 };
	AnimData powerupData{ &powerupDef, 0, 0, 0 };

	double puffLength = .07;
	AnimDef puffDef{ 0, { { 63, puffLength }, { 64, puffLength }, { 65, puffLength }, { 66, puffLength }, { 67, puffLength }, { 68, puffLength }, { 69, puffLength },
	{ 70, puffLength }, { 71, puffLength }, { 72, puffLength } }, 10 };
	AnimData puffData{ &puffDef, 0, puffLength, 1, };
	AnimDef jpuffDef{ 0, { { 73, puffLength }, { 74, puffLength }, { 75, puffLength }, { 76, puffLength }, { 77, puffLength }, { 78, puffLength }, { 79, puffLength },
	{ 80, puffLength }, { 81, puffLength }, { 82, puffLength } }, 10 };
	AnimData jpuffData{ &jpuffDef, 0, puffLength, 1 };

	double ringLength = .064;
	AnimDef ringDef{ 0, { { 100, ringLength }, { 101, ringLength }, { 102, ringLength }, { 103, ringLength }, { 104, ringLength }, { 105, ringLength }, }, 6 };
	AnimData ringData{ &ringDef, 0, ringLength, 0 };
	Object ringExplosion{ 0, 0, 0, 800, 550, ringData };

	double shieldLength = .1;
	AnimDef shieldDef{ 1, { { 83, shieldLength }, { 84, shieldLength }, { 85, shieldLength }, { 86, shieldLength }, { 87, shieldLength }, { 88, shieldLength },
	{ 89, shieldLength }, { 90, shieldLength }, { 91, shieldLength }, { 92, shieldLength }, { 93, shieldLength }, { 94, shieldLength }, { 95, shieldLength },
	{ 96, shieldLength }, { 97, shieldLength }, { 98, shieldLength }, { 99, 2 } }, 17 };
	AnimData shieldData{ &shieldDef, 0, shieldLength, 1 };
	Object shieldOb = { 0, 0, 0, 150, 110, shieldData };

	double hf = .12;
	AnimDef titleDef{ 1, { { 44, hf }, { 45, hf }, { 46, hf }, { 47, hf }, { 48, hf }, { 49, hf }, { 50, hf }, { 51, hf }, { 52, hf }, { 53, hf },
	{ 54, hf }, { 55, hf }, { 56, hf }, { 57, hf }, { 58, hf }, { 59, hf } }, 16 };
	AnimData titleData{ &titleDef, 0, 0, 1 };
	AnimDef birdDef{ 1, { { 31, hf }, { 32, hf }, { 33, hf }, { 34, hf }, { 35, hf }, { 36, hf }, { 37, hf }, { 38, hf }, { 39, hf }, { 40, hf },
	{ 41, hf }, { 42, hf }, { 43, hf } }, 13 };
	AnimData birdData{ &birdDef, 0, 0, 1 };

	Object horsie{ 0, 0, 0, 800, 550, titleData };
	Object birdie{ 0, 0, 0, 170, 130, birdData };

	//coin animation
	GLuint stars[] = { co1, co2, co3, co4, co5, co6, co7, co8, co9, co10 };
	AnimDef coinDef = { 1, { { 0, .1 }, { 1, .1 }, { 2, .1 }, { 3, .1 }, { 4, .1 }, { 5, .1 }, { 6, .1 }, { 7, .1 }, { 8, .1 }, { 9, .1 } }, 10 };
	AnimData coinData = { &coinDef, 0, 0, 1 };

	//fade out/in
	AnimDef fadeIn = { 0, { { 0, 1.2 }, { 1, .07 }, { 2, .07 }, { 3, .07 }, { 4, .07 }, { 5, .07 }, { 6, .07 }, { 6, .07 } }, 8 };
	AnimDef fadeOut = { 0, { { 6, .07 }, { 5, .07 }, { 4, .07 }, { 3, .07 }, { 2, .07 }, { 1, .07 }, { 0, .07 }, { 0, .07 } }, 8 };
	AnimData fadeData = { &fadeOut, 0, 0, 0 };
	Fader fader = { fadeData };
	Fader* faderp = &fader;

	//set camera starting point
	Camera cam = { 0, 0 };
	AABB tileBox[15][600];
	AABB tileBox2[15][600];
	const AABB* tileBoxPointer[15][600];
	const AABB* tileBoxPointer2[15][600];

	int jumpstack = 2;
	int addFlower = 1;
	bool fall = true;
	double cloudTimer = rand() % 12 + 6;;
	Cloud cloudList[3] = { { -200, 0, c1 }, { 200, 0, c2 }, { 700, 0, c3 } };
	int cloudCounter = 0;
	std::vector<Diamond> diamondList;
	GameState gameState = GS_TitleScreen;
	int selectorPos = 1;
	int selectorPos2 = 1;
	bool pause = false;
	int fadeOutEvent = 0;
	int fadeInEvent = 0;
	double distance = 0;  //GLOBAL VARIABLE FOR DISTANCE
	int score = 0; //GLOBAL VARIABLE FOR SCOREb
	double screenShakeH; //FOR SCREEN SHAKE
	double screenShakeV;
	bool flipH = true; //FOR SCREEN SHAKE

	int inCount = 2; //COUNTER FOR BG'S
	bool swapped = false; //FOR THE BG'S SWAPPING

	int lvl = 0;
	std::vector<int> lookBack;
	int lvl1counter = 5;
	int lvl2cd = 2;

	double endDis = 0;
	double endDisStep = 0;
	double endScore = 0;
	double endBonus = 0;
	double endFinal = 0;

	int endFade;
	int animState = 0;

	std::vector<std::vector<int>> example; //FOR COINS
	std::vector<std::vector<int>> example2; //FOR PLATFORMS
	double delay = 0; //delay at beginning. for testing purposes

	double hscount = .4; //for high score files
	int hspos = 5;	//for high score files
	int copyOnce = 0;	//for high score files

	double powerbar = 0;
	int pbwidth = 0;

	int feverfirst = 0; //only draw platforms on first
	double pegasusCounter = 0;
	double tc1 = 0;	//transformation delay
	int shield = 0; //SHIELD COUNT

	double invincibleCounter = 0;	//temporary invincibility after losing shield
	int invincibleStep = 2;
	bool visible = true;

	Object powerup{ 0, -200, -200, 0, 0 };
	double powerupcounter = 20 + rand() % 20;
	int puspeed = 1;

	int bonus = 0;
	bool bonusEvent = false;
	int totalBonus = 0;

	std::vector<Number> tempNum;
	Number speedupscreen{ speeed, 800, 550, 800, 0 };
	speedupscreen.dx = 800;
	bool speedupevent = false;

	bool dieonce = false;
	bool oneApplause = false;
	double shieldTimer = 0;

	std::vector<Object> puffBuffer;

	bool fadeOnce = true; //title music fade when switching gamestates

	bool startAnim = true;	//the animation at the start
	bool drawOwner = true;
	bool lvl1Music = true;
	double initxmov = 0;

	std::vector<Sparkle> sparkleTrail;
	bool onceperpause = true;
	int speedupOnce = 1; //run sounds at speed up only once

	bool nick1 = true;
	bool nick2 = true;
	int what = 0;

	//SOUND
	FMOD_SYSTEM* fmod;
	FMOD_System_Create(&fmod);
	FMOD_System_Init(fmod, 100, FMOD_INIT_NORMAL, 0);
	FMOD_CHANNEL* bgChan = NULL;
	FMOD_Channel_SetLoopCount(bgChan, -1);
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &bgChan);
	FMOD_CHANNEL* lvl0chan = NULL;
	FMOD_Channel_SetLoopCount(lvl0chan, -1);
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &lvl0chan);
	FMOD_CHANNEL* lvl1chan = NULL;
	FMOD_Channel_SetLoopCount(lvl1chan, -1);
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &lvl1chan);
	FMOD_CHANNEL* lvl2chan = NULL;
	FMOD_Channel_SetLoopCount(lvl2chan, -1);
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &lvl2chan);
	FMOD_CHANNEL* coinChan = 0;
	FMOD_Channel_SetLoopCount(coinChan, 0);
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &coinChan);
	FMOD_CHANNEL* fx = NULL;
	FMOD_Channel_SetLoopCount(fx, 0);
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &fx);
	FMOD_SOUND* titlemusic; FMOD_System_CreateStream(fmod, "titlemusic.flac", FMOD_LOOP_NORMAL, 0, &titlemusic);
	bool playTitleMusic = true;
	FMOD_SOUND* coin1; FMOD_System_CreateSound(fmod, "coin1.wav", FMOD_LOOP_OFF, 0, &coin1);
	FMOD_SOUND* coin2; FMOD_System_CreateSound(fmod, "coin2.wav", FMOD_LOOP_OFF, 0, &coin2);
	FMOD_SOUND* coin3; FMOD_System_CreateSound(fmod, "coin3.wav", FMOD_LOOP_OFF, 0, &coin3);
	FMOD_SOUND* coin4; FMOD_System_CreateSound(fmod, "coin4.wav", FMOD_LOOP_OFF, 0, &coin4);
	FMOD_SOUND* coin5; FMOD_System_CreateSound(fmod, "coin5.wav", FMOD_LOOP_OFF, 0, &coin5);
	FMOD_SOUND* coin6; FMOD_System_CreateSound(fmod, "coin6.wav", FMOD_LOOP_OFF, 0, &coin6);
	FMOD_SOUND* coin7; FMOD_System_CreateSound(fmod, "coin7.wav", FMOD_LOOP_OFF, 0, &coin7);
	FMOD_SOUND* coin8; FMOD_System_CreateSound(fmod, "coin8.wav", FMOD_LOOP_OFF, 0, &coin8);
	FMOD_SOUND* coin9; FMOD_System_CreateSound(fmod, "coin9.wav", FMOD_LOOP_OFF, 0, &coin9);
	FMOD_SOUND* coin10; FMOD_System_CreateSound(fmod, "coin10.wav", FMOD_LOOP_OFF, 0, &coin10);
	FMOD_SOUND* coins[] = { coin1, coin2, coin3, coin4, coin5, coin6, coin7, coin8, coin9, coin10 };
	FMOD_SOUND* pop; FMOD_System_CreateSound(fmod, "pop.ogg", FMOD_LOOP_OFF, 0, &pop);
	FMOD_SOUND* death; FMOD_System_CreateSound(fmod, "death.flac", FMOD_LOOP_OFF, 0, &death);
	FMOD_SOUND* transformation; FMOD_System_CreateSound(fmod, "transformation.aif", FMOD_LOOP_OFF, 0, &transformation);
	FMOD_SOUND* applause; FMOD_System_CreateSound(fmod, "applause.ogg", FMOD_LOOP_OFF, 0, &applause);
	FMOD_SOUND* fireballwarning; FMOD_System_CreateSound(fmod, "fireballwarning.ogg", FMOD_LOOP_OFF, 0, &fireballwarning);
	FMOD_SOUND* fireballpass; FMOD_System_CreateSound(fmod, "fireballpass.wav", FMOD_LOOP_OFF, 0, &fireballpass);
	FMOD_SOUND* jump; FMOD_System_CreateSound(fmod, "jump.wav", FMOD_LOOP_OFF, 0, &jump);
	FMOD_SOUND* djump; FMOD_System_CreateSound(fmod, "djump.wav", FMOD_LOOP_OFF, 0, &djump);
	FMOD_SOUND* feversound; FMOD_System_CreateSound(fmod, "fever.mp3", FMOD_LOOP_OFF, 0, &feversound);
	FMOD_SOUND* powerupsound; FMOD_System_CreateSound(fmod, "powerup.mp3", FMOD_LOOP_OFF, 0, &powerupsound);
	FMOD_SOUND* flowersound; FMOD_System_CreateSound(fmod, "flower.wav", FMOD_LOOP_OFF, 0, &flowersound);
	FMOD_SOUND* select; FMOD_System_CreateSound(fmod, "title.ogg", FMOD_LOOP_OFF, 0, &select);
	FMOD_SOUND* diamondsound; FMOD_System_CreateSound(fmod, "diamond.mp3", FMOD_LOOP_OFF, 0, &diamondsound);
	FMOD_SOUND* lv0m; FMOD_System_CreateStream(fmod, "lvl0music2.mp3", FMOD_LOOP_NORMAL, 0, &lv0m);
	FMOD_SOUND* lv2m; FMOD_System_CreateStream(fmod, "lvl2song1.mp3", FMOD_LOOP_NORMAL, 0, &lv2m);
	FMOD_SOUND* lv1m; FMOD_System_CreateStream(fmod, "lvl2song2.mp3", FMOD_LOOP_NORMAL, 0, &lv1m);
	FMOD_SOUND* transformationsong; FMOD_System_CreateStream(fmod, "transformationsong.mp3", FMOD_LOOP_NORMAL, 0, &transformationsong);

	//=======================================================START OF GAME LOOP=====================================================
	while (!shouldExit){
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		memcpy(kbPrevState, kbState, sizeof(kbPrevState));
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_Event event;
		lastFrameMs = currentFrameMs;								 //TIMING
		double temp = deltaTime;									 //TIMING
		currentFrameMs = SDL_GetTicks();							 //TIMING
		deltaTime = (currentFrameMs - lastFrameMs) / 1000.0f;		 //TIMING
		if (deltaTime > .03 && deltaTime < 1) deltaTime = temp;		 //TIMING
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		FMOD_System_Update(fmod); //update sound


		bool up, down, left, right = false;
		if (kbState[SDL_SCANCODE_UP]) { up = true; }
		if (kbState[SDL_SCANCODE_DOWN]) { down = true; }
		if (kbState[SDL_SCANCODE_LEFT]) { left = true; }
		if (kbState[SDL_SCANCODE_RIGHT]) { right = true; }

		//EVENTS
		bool spaceBool = false;
		while (SDL_PollEvent(&event)){
			switch (event.type) {
			case SDL_KEYDOWN:
				break;
			case SDL_QUIT:
				shouldExit = 1;
			}
		}

		/*switch (gameState) {
		case GS_TitleScreen:
		break;
		case GS_Play:*/

		if ((int)(currentFrameMs - lastPhysicsFrameMs) > 100){
			lastPhysicsFrameMs = currentFrameMs;
		}
		//=================================================DO-WHILE LOOP=====================================================
		do{
			lastPhysicsFrameMs += physicsDeltaMs;	//'Increment' physics ms
			updatePlayer(&player, physicsDeltaTime);
			if (up) player.y -= physicsDeltaTime * 100;
			if (down) player.y += physicsDeltaTime * 100;
			if (right) player.x += physicsDeltaTime * 100;
			if (left) player.x -= physicsDeltaTime * 100;
		} while (lastPhysicsFrameMs + physicsDeltaMs < currentFrameMs);
		//============================================END OF DO-WHILE LOOP====================================================

		glDrawSprite(ui, 0, 0, 800, 550);
		playerDraw(&player, playerTextures, player.x, player.y);

		//reset values
		up = false;
		down = false;
		left = false;
		right = false;

		/*
			break;
			case GS_End:
			break;
			*/

		SDL_GL_SwapWindow(window);
	}
	//=======================================================END OF GAME LOOP=========================================================

	SDL_Quit();
	return 0;
}