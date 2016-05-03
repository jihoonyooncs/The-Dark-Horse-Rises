#define _CRT_SECURE_NO_DEPRECATE
#include <SDL.h>
#include <glew.h>
#include <wglew.h>
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
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#undef main
using namespace std;

#include <fstream>
#include <iterator>
#include <string>

//GLOBAL VARIABLES
char shouldExit = 0;
unsigned char kbPrevState[SDL_NUM_SCANCODES] = { 0 };
const unsigned char* kbState = NULL;
Uint32 lastFrameMs;
Uint32 currentFrameMs = SDL_GetTicks();
float physicsDeltaTime = 1 / 100.0f;
int physicsDeltaMs = 10;
Uint32 lastPhysicsFrameMs;
float deltaTime = 0;

/*==============================================MAINNNNNNNNNNNNN===========================================================*/
int main(void) {
	glewInit();

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
	float gbt = .48;
	AnimDef horseFarewell{ 0, { { 63, gbt }, { 64, gbt }, { 65, gbt }, { 66, gbt }, { 67, 1.5 } }, 5 };
	AnimDef playerWalk = { 1, { { 0, .1 }, { 1, .1 }, { 2, .1 }, { 3, .1 }, { 4, .1 }, { 5, .1 }, { 6, .1 }, }, 7 };
	AnimDef playerWalkB = { 1, { { 35, .1 }, { 36, .1 }, { 37, .1 }, { 38, .1 }, { 39, .1 }, { 40, .1 }, { 41, .1 } }, 7 };
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
		-200, 100, player_width, player_height,
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

	float puffLength = .07;
	AnimDef puffDef{ 0, { { 63, puffLength }, { 64, puffLength }, { 65, puffLength }, { 66, puffLength }, { 67, puffLength }, { 68, puffLength }, { 69, puffLength },
	{ 70, puffLength }, { 71, puffLength }, { 72, puffLength } }, 10 };
	AnimData puffData{ &puffDef, 0, puffLength, 1, };
	AnimDef jpuffDef{ 0, { { 73, puffLength }, { 74, puffLength }, { 75, puffLength }, { 76, puffLength }, { 77, puffLength }, { 78, puffLength }, { 79, puffLength },
	{ 80, puffLength }, { 81, puffLength }, { 82, puffLength } }, 10 };
	AnimData jpuffData{ &jpuffDef, 0, puffLength, 1 };

	float ringLength = .064;
	AnimDef ringDef{ 0, { { 100, ringLength }, { 101, ringLength }, { 102, ringLength }, { 103, ringLength }, { 104, ringLength }, { 105, ringLength }, }, 6 };
	AnimData ringData{ &ringDef, 0, ringLength, 0 };
	Object ringExplosion{ 0, 0, 0, 800, 550, ringData };

	float shieldLength = .1;
	AnimDef shieldDef{ 1, { { 83, shieldLength }, { 84, shieldLength }, { 85, shieldLength }, { 86, shieldLength }, { 87, shieldLength }, { 88, shieldLength },
	{ 89, shieldLength }, { 90, shieldLength }, { 91, shieldLength }, { 92, shieldLength }, { 93, shieldLength }, { 94, shieldLength }, { 95, shieldLength },
	{ 96, shieldLength }, { 97, shieldLength }, { 98, shieldLength }, { 99, 2 } }, 17 };
	AnimData shieldData{ &shieldDef, 0, shieldLength, 1 };
	Object shieldOb = { 0, 0, 0, 150, 110, shieldData };

	float hf = .12;
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
	float cloudTimer = rand() % 12 + 6;;
	Cloud cloudList[3] = { { -200, 0, c1 }, { 200, 0, c2 }, { 700, 0, c3 } };
	int cloudCounter = 0;
	std::vector<Diamond> diamondList;
	GameState gameState = GS_TitleScreen;
	int selectorPos = 1;
	int selectorPos2 = 1;
	bool pause = false;
	int fadeOutEvent = 0;
	int fadeInEvent = 0;
	float distance = 0;  //GLOBAL VARIABLE FOR DISTANCE
	int score = 0; //GLOBAL VARIABLE FOR SCOREb
	float screenShakeH; //FOR SCREEN SHAKE
	float screenShakeV;
	bool flipH = true; //FOR SCREEN SHAKE

	int inCount = 2; //COUNTER FOR BG'S
	bool swapped = false; //FOR THE BG'S SWAPPING

	int lvl = 0;
	std::vector<int> lookBack;
	int lvl1counter = 5;
	int lvl2cd = 2;

	float endDis = 0;
	float endDisStep = 0;
	float endScore = 0;
	float endBonus = 0;
	float endFinal = 0;

	int endFade;
	int animState = 0;

	std::vector<std::vector<int>> example; //FOR COINS
	std::vector<std::vector<int>> example2; //FOR PLATFORMS

	float hscount = .4; //for high score files
	int hspos = 5;	//for high score files
	int copyOnce = 0;	//for high score files

	float powerbar = 0;
	int pbwidth = 0;

	int feverfirst = 0; //only draw platforms on first
	float pegasusCounter = 0;
	float tc1 = 0;	//transformation delay
	int shield = 0; //SHIELD COUNT

	float invincibleCounter = 0;	//temporary invincibility after losing shield
	int invincibleStep = 2;
	bool visible = true;

	Object powerup{ 0, -200, -200, 0, 0 };
	float powerupcounter = 20 + rand() % 20;
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
	float shieldTimer = 0;

	std::vector<Object> puffBuffer;

	bool fadeOnce = true; //title music fade when switching gamestates

	bool startAnim = true;	//the animation at the start
	bool drawOwner = true;
	bool lvl1Music = true;
	float initxmov = 0;

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
	FMOD_CHANNEL* bgChan;
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &bgChan);
	FMOD_Channel_SetLoopCount(bgChan, -1);
	FMOD_CHANNEL* lvl0chan;
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &lvl0chan);
	FMOD_Channel_SetLoopCount(lvl0chan, -1);
	FMOD_CHANNEL* lvl1chan;
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &lvl1chan);
	FMOD_Channel_SetLoopCount(lvl1chan, -1);
	FMOD_CHANNEL* lvl2chan;
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &lvl2chan);
	FMOD_Channel_SetLoopCount(lvl2chan, -1);
	FMOD_CHANNEL* coinChan = 0;
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &coinChan);
	FMOD_Channel_SetLoopCount(coinChan, 0);
	FMOD_CHANNEL* fx;
	FMOD_System_PlaySound(fmod, NULL, NULL, true, &fx);
	FMOD_Channel_SetLoopCount(fx, 0);
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
	//	FMOD_SOUND*; FMOD_System_CreateSound(fmod, "", FMOD_LOOP_OFF, 0, &);
	//	FMOD_SOUND*; FMOD_System_CreateSound(fmod, "", FMOD_LOOP_OFF, 0, &);

	FMOD_SOUND* lv0m; FMOD_System_CreateStream(fmod, "lvl0music2.mp3", FMOD_LOOP_NORMAL, 0, &lv0m);
	FMOD_SOUND* lv2m; FMOD_System_CreateStream(fmod, "lvl2song1.mp3", FMOD_LOOP_NORMAL, 0, &lv2m);
	FMOD_SOUND* lv1m; FMOD_System_CreateStream(fmod, "lvl2song2.mp3", FMOD_LOOP_NORMAL, 0, &lv1m);
	FMOD_SOUND* transformationsong; FMOD_System_CreateStream(fmod, "transformationsong.mp3", FMOD_LOOP_NORMAL, 0, &transformationsong);
	//	FMOD_SOUND*; FMOD_System_CreateStream(fmod, "", FMOD_LOOP_NORMAL, 0, &);
	//	FMOD_SOUND*; FMOD_System_CreateStream(fmod, "", FMOD_LOOP_NORMAL, 0, &);
	//	FMOD_SOUND*; FMOD_System_CreateStream(fmod, "", FMOD_LOOP_NORMAL, 0, &);


	//===================================================MAIN GAME LOOP===========================================================
	while (!shouldExit) {
		wglSwapIntervalEXT(1);
		srand(time(NULL));
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		SDL_Event event;
		lastFrameMs = currentFrameMs;
		float temp = deltaTime;
		memcpy(kbPrevState, kbState, sizeof(kbPrevState));
		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		//TIMING
		currentFrameMs = SDL_GetTicks();
		deltaTime = (currentFrameMs - lastFrameMs) / 1000.0f;
		if (deltaTime > .03 && deltaTime < 1) deltaTime = temp;

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		//update sound
		FMOD_System_Update(fmod);

		//KEYBOARD
		//	if(kbState[SDL_SCANCODE_W] && !pause && gameState == GS_Play) { cam.y -= deltaTime * 750; }
		if (kbState[SDL_SCANCODE_D] && !pause && gameState == GS_Play) { cam.x += deltaTime * 750; }
		if (kbState[SDL_SCANCODE_A] && !pause && gameState == GS_Play) { cam.x -= deltaTime * 750; }
		//	if(kbState[SDL_SCANCODE_S] && !pause && gameState == GS_Play) { cam.y += deltaTime * 700; }

		if (kbState[SDL_SCANCODE_UP] && !pause && gameState == GS_Play) { player.y -= deltaTime * 750; player.yvel = 0; }
		if (kbState[SDL_SCANCODE_DOWN] && !pause && gameState == GS_Play) { player.y += deltaTime * 750; player.yvel = 0; }
		if (kbState[SDL_SCANCODE_LEFT] && !pause && gameState == GS_Play) { player.x -= deltaTime * 750; }
		if (kbState[SDL_SCANCODE_RIGHT] && !pause && gameState == GS_Play) { player.x += deltaTime * 750; }

		bool spaceBool = false;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (gameState) {
				case GS_Play:

					/*	if(event.key.keysym.sym == SDLK_o) {
							example2.back()[1] -= 5;
							}
							if(event.key.keysym.sym == SDLK_l) {
							example2.back()[1] += 5;
							}
							if(event.key.keysym.sym == SDLK_k) {
							if(example2.size() > 0) example2.pop_back();
							}
							if(event.key.keysym.sym == SDLK_m) {
							if(example2.size() > 0) example2.clear();
							}
							if(event.key.keysym.sym == SDLK_p) {
							ofstream outputFile("./example.txt");
							outputFile << "{";
							for(int i = 0; i < example2.size(); i++) {
							outputFile << "{";
							std::copy(example2[i].begin(), example2[i].end(), ostream_iterator<int>(outputFile, ","));
							if(i < example2.size() - 1) outputFile << "},";
							else outputFile << "}";
							}
							outputFile << "}" << endl;
							}

							if(event.key.keysym.sym == SDLK_s) {
							if(example.size() > 0) example.pop_back();
							}
							if(event.key.keysym.sym == SDLK_c) {
							if(example.size() > 0) example.clear();
							}
							if(event.key.keysym.sym == SDLK_i) {
							ofstream outputFile("./example.txt");
							outputFile << "{";
							for(int i = 0; i < example.size(); i++) {
							outputFile << "{";
							std::copy(example[i].begin(), example[i].end(), ostream_iterator<int>(outputFile, ","));
							if(i < example.size() - 1) outputFile << "},";
							else outputFile << "}";
							}
							outputFile << "}" << endl;
							}
							*/

					if (event.key.keysym.sym == SDLK_SPACE && jumpstack > 0 && !pause && !player.dead) { //set space actions
						spaceBool = true;
						if (animState == 1) {
							if (jumpstack == 3 && abs(player.yvel) < 2 && !player.flying && !(player.fever && player.feverCounter < 50) && !startAnim) {
								if (lvl != 2) player.yvel = 1770 * physicsDeltaTime;
								else player.yvel = 1830 * physicsDeltaTime;
								animSet(&player.anim, &horseJumpp);
								FMOD_System_PlaySound(fmod, jump, NULL, false, &fx);
								FMOD_Channel_SetVolumeRamp(fx, false);
								FMOD_Channel_SetVolume(fx, 0.15f);
								puffBuffer.push_back({ 1, player.x + 50, player.y, 120, 120, jpuffData });
							}
							else if (jumpstack == 2 && !player.flying && !(player.fever && player.feverCounter < 50)) {
								player.yvel = 1300 * physicsDeltaTime;
								player.x += 10; player.y -= 30; player.w += 20; player.h += 65;
								player.hitbox = { player.x + 64 - cam.x, player.y + 80 - cam.y, 74, 70 };
								animSet(&player.anim, &horseTwirl);
								FMOD_System_PlaySound(fmod, djump, NULL, false, &fx);
								FMOD_Channel_SetVolumeRamp(fx, false);
								FMOD_Channel_SetVolume(fx, 0.07f);
							}
							else if (jumpstack == 1 && !player.flying && !(player.fever && player.feverCounter < 50)) {
								player.yvel = 1200 * physicsDeltaTime;
								if (player.anim.def != &horseTwirl) {
									player.x += 10; player.y -= 30; player.w += 20; player.h += 65;
								}
								if (player.anim.def != &horseTwirl) player.hitbox = { player.x + 64 - cam.x, player.y + 80 - cam.y, 74, 70 };
								animSet(&player.anim, &horseTwirl);
								FMOD_System_PlaySound(fmod, djump, NULL, false, &fx);
								FMOD_Channel_SetVolumeRamp(fx, false);
								FMOD_Channel_SetVolume(fx, 0.07f);
							}
						}
						else if (jumpstack == 2 && abs(player.yvel) < 2 && !player.flying && !(player.fever && player.feverCounter < 50) && !startAnim) {
							if (lvl != 2) player.yvel = 1770 * physicsDeltaTime;
							else player.yvel = 1830 * physicsDeltaTime;
							animSet(&player.anim, &horseJumpB);
							FMOD_System_PlaySound(fmod, jump, NULL, false, &fx);
							FMOD_Channel_SetVolumeRamp(fx, false);
							FMOD_Channel_SetVolume(fx, 0.15f);
							puffBuffer.push_back({ 1, player.x + 50, player.y, 120, 120, jpuffData });
						}
						else if (jumpstack == 1 && !player.flying && !(player.fever && player.feverCounter < 50)) {
							player.yvel = 1300 * physicsDeltaTime;
							animSet(&player.anim, &horseTwirlB);
							player.x += 10; player.y -= 30; player.w += 20; player.h += 65;
							player.hitbox = { player.x + 64 - cam.x, player.y + 80 - cam.y, 74, 70 };
							FMOD_System_PlaySound(fmod, djump, NULL, false, &fx);
							FMOD_Channel_SetVolumeRamp(fx, false);
							FMOD_Channel_SetVolume(fx, 0.07f);
						}
						jumpstack--;
					}
					if (event.key.keysym.sym == SDLK_p && !player.dead && !startAnim) {
						FMOD_Channel_SetPaused(lvl1chan, 1);
						pause = true;
					}
					if (pause && event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_DOWN) {
						FMOD_System_PlaySound(fmod, select, NULL, false, &fx);
						selectorPos2 = selectorPos2 > 2 ? 1 : ++selectorPos2;
					}
					else if (pause && event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_UP) {
						FMOD_System_PlaySound(fmod, select, NULL, false, &fx);
						selectorPos2 = selectorPos2 < 2 ? 3 : --selectorPos2;
					}
					break;
				case GS_TitleScreen:
					if (pause == true && event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_DOWN) {
						FMOD_System_PlaySound(fmod, select, NULL, false, &fx);
						selectorPos = selectorPos == 1 ? 2 : 1;
					}
					else if (pause == true && event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_UP) {
						FMOD_System_PlaySound(fmod, select, NULL, false, &fx);
						selectorPos = selectorPos == 1 ? 2 : 1;
					}

					if (selectorPos == 1 && (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) && fadeOutEvent == 0) {
						fadeOutEvent = 1;
					}
					else if (selectorPos == 3 && (kbState[SDL_SCANCODE_RETURN] || kbState[SDL_SCANCODE_SPACE]) && fadeOutEvent == 0) {
						shouldExit = 1;
					}
					break;
				case GS_End:
					if (event.key.keysym.sym == SDLK_r && endFade == 0) {
						endFade = 2;
						fader.anim.def = &fadeOut;
						fader.anim.curFrame = 0;
					}
					else if (event.key.keysym.sym == SDLK_m && endFade == 0) {
						endFade = 1;
						fader.anim.def = &fadeOut;
						fader.anim.curFrame = 0;
					}
					else if (event.key.keysym.sym == SDLK_q && endFade == 0) shouldExit = 1;
					break;
				default:
					break;
				}
				break;
				/*	case SDL_MOUSEBUTTONDOWN:
						if(event.button.button == SDL_BUTTON_LEFT && gameState != GS_TitleScreen) {
						//Get the mouse offsets
						int x = event.button.x + cam.x - tempBG[0].xmov;
						int y = event.button.y;
						example.push_back({ x, y });
						}
						if(event.button.button == SDL_BUTTON_RIGHT && gameState != GS_TitleScreen) {
						int x = event.button.x + cam.x - tempBG[0].xmov;
						int y = event.button.y;
						example2.push_back({ x, y });
						for(int i = 0; i < example2.size(); i++) {
						if(abs(( example2[i][0] + 63 ) - x) < 20 && abs(example2[i][1] - y) < 20) {
						example2.back()[0] -= ( x - ( example2[i][0] + 63 ) );
						example2.back()[1] -= ( y - example2[i][1] );
						}
						}
						}
						break;*/
			case SDL_QUIT:
				shouldExit = 1;
			}
		}

		switch (gameState) {
		case GS_TitleScreen:
			if (deltaTime < .1) {
				if (playTitleMusic == true) {
					FMOD_Channel_SetPaused(bgChan, 0);
					FMOD_System_PlaySound(fmod, titlemusic, NULL, false, &bgChan);
					playTitleMusic = false;
				}

				updateObject(&birdie, deltaTime);
				updateObject(&horsie, deltaTime);
				objectDraw(&horsie, objectTextures, 0, 0);
				objectDraw(&birdie, objectTextures, 170, 110);

				glDrawSprite(titleOverlay, 0, 0, 800, 550);
				if (selectorPos == 1) glDrawSprite(selector, 595, 440, 30, 30);
				else if (selectorPos == 2) glDrawSprite(selector, 595, 479, 30, 30);
				if (fadeOutEvent > 0) {
					if (fadeOnce) {
						unsigned long long parentclock;
						FMOD_Channel_GetDSPClock(bgChan, NULL, &parentclock);
						FMOD_Channel_AddFadePoint(bgChan, parentclock, 1.0f);
						FMOD_Channel_AddFadePoint(bgChan, parentclock + 10096, 0.0f);
						fadeOnce = false;
					}
					if (fader.anim.curFrame != 7) {
						fader.anim.def = &fadeOut;
						fader.anim.isPlaying = true;
						animDraw(&faderp->anim, fade, 0, 0, 800, 550);
						animTick(&faderp->anim, deltaTime);
					}
					else {
						tempBG = { bg1, bg2 };
						if (!startAnim) tempBG[0].xmov = -900;
						tempBG[1].xmov = 2000;
						lvl = 0;
						jumpstack = 2;
						lvl1counter = 5;
						lvl2cd = 2;
						score = 0;
						distance = 0;
						inCount = 2;
						swapped = false;
						pause = false;
						addFlower = 2;
						if (player.anim.def == &horseTwirlB || player.anim.def == &horseTwirl) {
							player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
							player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
							animSet(&player.anim, &playerWalkB);
						}
						player.anim.def = &playerWalkB;
						if (startAnim) player.x = -200;
						else player.x = 40;
						player.y = 250;
						player.dead = false;
						fader.anim.def = &fadeIn;
						fader.anim.curFrame = 0;
						cam.x = 0;
						gameState = GS_Play;
						backdrop[0].x = 0;
						backdrop[1].x = 0;
						endDis = 0;
						endDisStep = 0;
						endScore = 0;
						endBonus = 0;
						endFinal = 0;
						hscount = .4;
						hspos = 5;
						copyOnce = 0;
						powerbar = 0;
						animState = 0;
						invincibleCounter = 0;
						invincibleStep = 2;
						visible = true;
						shield = 5;
						player.fever = false;
						powerup = { 0, -200, -200, 0, 0 };
						powerupcounter = 20 + rand() % 20;
						puspeed = 1;
						bonus = 0;
						bonusEvent = false;
						totalBonus = 0;
						tempNum.clear();
						speedupscreen = { speeed, 800, 550, 800, 0, 0 };
						speedupevent = false;
						speedupscreen.dx = 800;
						lookBack.clear();
						dieonce = false;
						oneApplause = false;
						shieldTimer = 0;
						animDraw(&faderp->anim, fade, 0, 0, 800, 550);
						fadeOutEvent = 0;
						fader.anim.def = &fadeIn;
						fader.anim.curFrame = 0;
						FMOD_Channel_Stop(lvl0chan);
						FMOD_Channel_Stop(lvl1chan);
						FMOD_Channel_Stop(lvl2chan);
						FMOD_Channel_Stop(bgChan);
						FMOD_Channel_SetPaused(lvl0chan, 0);
						FMOD_Channel_SetPaused(lvl1chan, 1);
						FMOD_Channel_SetPaused(lvl2chan, 1);
						FMOD_Channel_SetPaused(bgChan, 1);
						nick1 = true;
						nick2 = true;
						what = 0;
						FMOD_System_PlaySound(fmod, lv0m, NULL, false, &lvl0chan);
						initxmov = 0;
					}
				}
			}
			break;
		case GS_Play:{
			if (deltaTime < .1) {
				if (lvl1Music) {
					unsigned long long parentclock1;
					FMOD_Channel_GetDSPClock(lvl0chan, NULL, &parentclock1);
					FMOD_Channel_AddFadePoint(lvl0chan, parentclock1, 0.0f);
					FMOD_Channel_AddFadePoint(lvl0chan, parentclock1 + 10096, 1.0f);
					FMOD_System_PlaySound(fmod, lv0m, NULL, false, &lvl0chan);
					lvl1Music = false;
				}
				if (startAnim) {
					if (player.x < 250) {
						player.x += 250 * deltaTime;
						if (player.x >= 250) {
							player.anim.def = &horseFarewell;
							player.anim.curFrame = 0;
							player.anim.timeToNextFrame = gbt;
							owner.anim.isPlaying = true;
						}
					}
					else {
						updateObject(&owner, deltaTime);
						if (!owner.anim.isPlaying) {
							startAnim = false;
							player.anim.def = &playerWalkB;
							player.anim.curFrame = 0;
							player.anim.timeToNextFrame = .1;
						}
					}
				}
				if (!startAnim && player.x > 40 && !pause) player.x -= 50 * deltaTime;
				if (animState == 0 && jumpstack == 2 && abs(player.yvel) > 3 && !player.flying && !pause) { //change animation if horse falls off edge
					animSet(&player.anim, &horseJumpB);
					player.anim.curFrame = 4;
					player.anim.isPlaying = true;
					jumpstack = 1;
				}
				else if (animState == 1 && jumpstack == 3 && abs(player.yvel) > 3 && !player.flying && !pause) {
					animSet(&player.anim, &horseJumpp);
					player.anim.curFrame = 4;
					player.anim.isPlaying = true;
					jumpstack = 2;
				}
				if (player.anim.def == &horseTwirlB && player.anim.curFrame == 10 && !player.flying && !pause) { //end of double jump
					animSet(&player.anim, &horseJumpB);
					player.anim.curFrame = 4;
					player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
					player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
				}
				else if (player.anim.def == &horseTwirl && player.anim.curFrame == 13 && !player.flying && !pause) {
					animSet(&player.anim, &horseJumpp);
					player.anim.curFrame = 4;
					player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
					player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
				}


				if (pause) FMOD_Channel_SetPaused(bgChan, 1);
				else FMOD_Channel_SetPaused(bgChan, 0);
				//FEVER TIMEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
				if (pbwidth >= 282 && !player.flying) {
					FMOD_Channel_Stop(lvl0chan);
					FMOD_Channel_Stop(lvl1chan);
					FMOD_Channel_Stop(lvl2chan);
					FMOD_Channel_SetPaused(lvl0chan, 1);
					FMOD_Channel_SetPaused(lvl1chan, 1);
					FMOD_Channel_SetPaused(lvl2chan, 1);
					FMOD_Channel_SetPaused(bgChan, 0);
					shield = 0;
					player.magnet = false;
					FMOD_Channel_SetPaused(bgChan, 0);
					FMOD_System_PlaySound(fmod, transformationsong, NULL, false, &bgChan);
					FMOD_System_PlaySound(fmod, feversound, NULL, false, &fx);
					if (player.anim.def == &horseTwirlB) {
						player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
						player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
					}
					player.anim.def = &horseJumpB;
					player.anim.curFrame = 2;
					player.anim.isPlaying = false;
					player.fevery = player.y - 20;
					player.fever = true;
					diamondList.clear();
					for (int i = 0; i < tempBG[0].starMap.size(); i++) {
						tempBG[0].starMap[i].starList.clear();
					}
					tempBG[0] = fever;
					tempBG[0].xmov = 1400;
					powerbar = 0;
					if (player.anim.def == &horseTwirlB) {
						player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
						player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
						animSet(&player.anim, &playerWalkB);
					}
					tc1 = 19.5;
					invincibleCounter = 0;
					ringExplosion.anim.isPlaying = true;
					ringExplosion.anim.curFrame = 0;
				}
				if (player.fever && player.feverCounter >= 50) {
					if (!player.anim.isPlaying) {
						player.anim.isPlaying = true;
					}
					if (tc1 <= 0) {
						ringExplosion.anim.isPlaying = true;
						ringExplosion.anim.curFrame = 0;
						if (player.anim.def == &playerWalkB) player.anim.def = &playerWalk;
						else if (player.anim.def == &horseJumpB) player.anim.def = &horseJumpp;
						else if (player.anim.def == &horseTwirlB) player.anim.def = &horseTwirl;
						shield++;
						animState = 1;
						tc1 = 8000;
						FMOD_System_PlaySound(fmod, transformation, NULL, false, &fx);
						FMOD_Channel_SetVolumeRamp(fx, false);
						FMOD_Channel_SetVolume(fx, 2.5f);
					}
					if (tempBG[1].xmov < 1200) {
						unsigned long long parentclock4;
						FMOD_Channel_GetDSPClock(bgChan, NULL, &parentclock4);
						FMOD_Channel_AddFadePoint(bgChan, parentclock4, 1.0f);
						FMOD_Channel_AddFadePoint(bgChan, parentclock4 + 10096, 0.0f);
						unsigned long long parentclock3;
						FMOD_Channel_GetDSPClock(lvl2chan, NULL, &parentclock3);
						FMOD_Channel_AddFadePoint(lvl2chan, parentclock3, 0.0f);
						FMOD_Channel_AddFadePoint(lvl2chan, parentclock3 + 10096, 1.0f);
						FMOD_System_PlaySound(fmod, lv2m, NULL, false, &lvl2chan);
						player.fever = false;
						player.feverCounter = 0;
						if (lvl == 1) lvl1counter--;
						feverfirst = 0;
						pegasusCounter = 45 + rand() % 15;
					}
				}
				if (pegasusCounter > 0 && !pause && !player.dead) {
					pegasusCounter -= deltaTime;
					if (pegasusCounter <= 0) {
						ringExplosion.anim.isPlaying = true;
						ringExplosion.anim.curFrame = 0;
						FMOD_System_PlaySound(fmod, transformation, NULL, false, &fx);
						if (player.anim.def == &playerWalk) animSet(&player.anim, &playerWalkB); //change these animations to pegasus->black
						else if (player.anim.def == &horseJumpp) animSet(&player.anim, &horseJumpB);
						else if (player.anim.def == &horseTwirl) animSet(&player.anim, &horseTwirlB);
						pegasusCounter = 0;
						animState = 0;
						shield--;
					}
				}


				if (!pause) updatePlayer(&player, deltaTime); //Update Player
				if (player.anim.def != &horseTwirl && player.anim.def != &horseTwirlB)
					player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 }; //set player hitbox

				//======================================DRAW MOVING PLATFORMS/Background=======================================================
				//far backdrop
				if (!player.fever) {
					glDrawSprite(backdrop[0].img, backdrop[0].x - cam.x, 0 - cam.y, backdrop[0].width, 550);
					if (!pause && !startAnim) backdrop[0].x -= initxmov / 6.67;
					int j;
					backdrop[0].x + backdrop[0].width - cam.x < 800 ? glDrawSprite(backdrop[0].img, backdrop[0].x + backdrop[0].width - cam.x, 0 - cam.y, backdrop[0].width, 550) : j = 0;
					backdrop[0].x = backdrop[0].x + backdrop[0].width - cam.x < 0 ? backdrop[0].x + backdrop[0].width : backdrop[0].x;
				}
				else if (!startAnim){
					glDrawSprite(backdrop[2].img, backdrop[2].x - cam.x, 0 - cam.y, backdrop[2].width, 550);
					if (!pause) backdrop[2].x -= initxmov / 8;
					if (backdrop[2].x + backdrop[2].width - cam.x < 800)
						glDrawSprite(backdrop[2].img, backdrop[2].x + backdrop[2].width - cam.x, 0 - cam.y, backdrop[2].width, 550);
					if (backdrop[2].x + backdrop[2].width < 0) backdrop[2].x = backdrop[2].x + backdrop[2].width;

					glDrawSprite(backdrop[3].img, backdrop[3].x - cam.x, 0 - cam.y, backdrop[3].width, 550);
					if (!pause) backdrop[3].x -= initxmov / 5;
					if (backdrop[3].x + backdrop[3].width - cam.x < 800)
						glDrawSprite(backdrop[3].img, backdrop[3].x + backdrop[3].width - cam.x, 0 - cam.y, backdrop[3].width, 550);
					if (backdrop[3].x + backdrop[3].width < 0) backdrop[3].x = backdrop[3].x + backdrop[3].width;
				}

				//Cloud Generator
				if (!pause && !player.fever) cloudTimer -= deltaTime;
				for (int i = 0; i < 3; i++) {
					if (!player.fever) {
						glDrawSprite(cloudList[i].img, cloudList[i].x - cam.x, cloudList[i].y - cam.y, 160, 60);
						if (!pause && !startAnim) cloudList[i].x -= 40 * deltaTime;
					}
				}
				if (cloudTimer <= 0 && !player.fever && !pause) {
					cloudList[cloudCounter].img = clouds[rand() % 3];
					cloudList[cloudCounter].x = 800;
					cloudList[cloudCounter].y = rand() % 80 - 5;
					cloudTimer = rand() % 12 + 6;
					cloudCounter = cloudCounter == 2 ? 0 : ++cloudCounter;
				}

				//closer backdrop
				if (!player.fever) {
					glDrawSprite(backdrop[1].img, backdrop[1].x - cam.x, 300 - cam.y, backdrop[1].width, 250);
					if (!pause && !startAnim) backdrop[1].x -= initxmov / 3.33;
					int j;
					backdrop[1].x + backdrop[1].width - cam.x < 800 ? glDrawSprite(backdrop[1].img, backdrop[1].x + backdrop[1].width - cam.x, 300 - cam.y, backdrop[1].width, 250) : j = 0;
					backdrop[1].x = backdrop[1].x + backdrop[1].width - cam.x < 0 ? backdrop[1].x + backdrop[1].width : backdrop[1].x;
				}//*/

				//SWAP PLATFORMS WHEN ONE REACHES THE END OF LEFT SCREEN
				if (tempBG[0].xmov + tempBG[0].maxLength * tw + 150 - cam.x < 0 && tempBG[0].maxLength != 0) {
					tempBG[0] = tempBG[1];
					if (lvl == 0) {
						tempBG[1] = initialLevel[inCount];
						tempBG[1].xmov = tempBG[0].xmov + tempBG[0].maxLength * tw;
						if (inCount < initialLevel.size() - 1) {
							inCount++;
						}
						else {
							lvl++;
						}
					}
					else if (lvl == 1) {
						inCount = rand() % lvl1.size();
						for (int i = 0; i < lookBack.size(); i++) {
							if (inCount == lookBack[i]) {
								inCount = rand() % lvl1.size();
								i = 0;
							}
						}
						lookBack.push_back(inCount);
						if (lookBack.size() > 3) lookBack.erase(lookBack.begin());
						tempBG[1] = lvl1[inCount];
						tempBG[1].xmov = tempBG[0].xmov + tempBG[0].maxLength*tw;
						if (--lvl1counter == 0) {
							lookBack.clear();
							lvl++;
						}
					}
					else if (lvl == 2) {
						inCount = rand() % lvl2.size();
						for (int i = 0; i < lookBack.size(); i++) {
							if (inCount == lookBack[i]) {
								inCount = rand() % lvl2.size();
								i = 0;
							}
						}
						lookBack.push_back(inCount);
						if (lookBack.size() > 3) lookBack.erase(lookBack.begin());
						lvl2cd = lvl2cd > 0 ? --lvl2cd : 0;
						tempBG[1] = lvl2[inCount];
						tempBG[1].xmov = tempBG[0].xmov + tempBG[0].maxLength*tw;
					}
					addFlower = 1;
					swapped = true;
				}//*/

				//TREES
				for (int e = 0; e < 2; e++) {
					for (int i = 0; i < tempBG[e].tileMap.size(); i++) {
						for (int j = 0; j < tempBG[e].tileMap[i].size(); j++) {
							float a = j * tw;
							float b = i * th;
							if ((!swapped || e == 1) && tempBG[e].tileMap[i][j] == 96 && addFlower == 1) {
								tempBG[e].treeList.push_back({ a, b - 310 + rand() % 100 });
							}
						}
					}
					for (int i = 0; i < tempBG[e].treeList.size(); i++) { //DRAW THE TREES
						glDrawSprite(tree, tempBG[e].xmov + tempBG[e].treeList[i].x - cam.x, tempBG[e].treeList[i].y - cam.y, 207, 382);
					}
				}

				//COINS
				for (int e = 0; e < 2; e++) {
					for (int a = 0; a < tempBG[e].starMap.size(); a++) {
						for (int i = 0; i < tempBG[e].starMap[a].map.size(); i++) {
							if ((e == 0 && player.fever && feverfirst == 0) || ((e == 1 || !swapped) && addFlower == 1)) {
								float sx = tempBG[e].starMap[a].map[i][0] - 12;
								float sy = tempBG[e].starMap[a].map[i][1] - 12;
								coinData.curFrame = rand() % 10;
								tempBG[e].starMap[a].starList.push_back({ coinData, sx, sy, 24, 24 });
								tempBG[e].starMap[a].count++;
							}
						}
					}
				}for (int e = 0; e < 2; e++) {//DRAW COINS
					for (int a = 0; a < tempBG[e].starMap.size(); a++) {
						for (int i = 0; i < tempBG[e].starMap[a].starList.size(); i++) {
							float sx = tempBG[e].xmov + tempBG[e].starMap[a].starList[i].x - cam.x;
							float sx2 = tempBG[e].starMap[a].starList[i].x - cam.x;
							float sy = tempBG[e].starMap[a].starList[i].y - cam.y;
							if (!tempBG[e].starMap[a].starList[i].set && ((!player.flying && !player.magnet && !tempBG[e].starMap[a].starList[i].finishing) ||
								tempBG[e].xmov + tempBG[e].starMap[a].starList[i].x - cam.x > 600)) {
								if (!pause && !player.dead) updateStar(&tempBG[e].starMap[a].starList[i], &player, deltaTime, 0);
								starDraw(&tempBG[e].starMap[a].starList[i], stars, sx, sy);
								tempBG[e].starMap[a].starList[i].hitbox = { sx + 4, sy + 4, 16, 16 };
							}
							else if ((player.flying || player.magnet) && tempBG[e].xmov + tempBG[e].starMap[a].starList[i].x - cam.x > player.x ||
								tempBG[e].starMap[a].starList[i].finishing) {
								if (!tempBG[e].starMap[a].starList[i].set) {
									tempBG[e].starMap[a].starList[i].bx = sx;
									tempBG[e].starMap[a].starList[i].by = sy;
									tempBG[e].starMap[a].starList[i].set = true;
									tempBG[e].starMap[a].starList[i].count = 1;
								}
								if (!pause && !player.dead) updateStar(&tempBG[e].starMap[a].starList[i], &player, deltaTime, 1);
								starDraw(&tempBG[e].starMap[a].starList[i], stars, tempBG[e].starMap[a].starList[i].x, sy);
								tempBG[e].starMap[a].starList[i].hitbox = { sx2 + 4, sy + 4, 16, 16 };
							}
						}
					}
				}

				//DRAW THE TILES AND OBJECTS!!!!!
				for (int e = 0; e < 2; e++) {
					if (e == 1) tempBG[e].xmov = tempBG[0].xmov + tempBG[0].maxLength * tw;
					for (int i = 0; i < tempBG[e].tileMap.size(); i++) {
						for (int j = 0; j < tempBG[e].tileMap[i].size(); j++) {
							float a1 = tempBG[e].xmov + j * tw;
							float a = j * tw;
							float b = i * th;
							int get = tempBG[e].tileMap[i][j];
							tempBG[e].maxLength = tempBG[e].tileMap[i].size() > tempBG[e].maxLength ? tempBG[e].tileMap[i].size() : tempBG[e].maxLength;
							if (get > 0 && get <= 90) {
								get--;
								if (e == 0) { //first array in temp storage
									tileBox[i][j] = tilelist[get].hitbox;
									tileBox[i][j].x = tilelist[get].leftpad + a1 - cam.x;
									tileBox[i][j].y = tilelist[get].toppad + b - cam.y;
									tileBox[i][j].w = tw - tilelist[get].leftpad - tilelist[get].rightpad;
									tileBox[i][j].h = th - tilelist[get].toppad;
									tileBoxPointer[i][j] = &tileBox[i][j];
								}
								else if (e == 1) {	//second array in temp storage
									tileBox2[i][j] = tilelist[get].hitbox;
									tileBox2[i][j].x = tilelist[get].leftpad + a1 - cam.x;
									tileBox2[i][j].y = tilelist[get].toppad + b - cam.y;
									tileBox2[i][j].w = tw - tilelist[get].leftpad - tilelist[get].rightpad;
									tileBox2[i][j].h = th - tilelist[get].toppad;
									tileBoxPointer2[i][j] = &tileBox2[i][j];
								}
								//int tbg = tempBG[e].maxLength;
								//tempBG[e].maxLength = tbg < j ? j : tbg;  //set max length
								if (a1 > cam.x - 140 && a1 < cam.x + 850) { //OPTIMIZATION
									glDrawSprite(pics[get], a1 - cam.x, b - cam.y, tw, th);
								}
							}
							else {
								switch (get) {
								case -1: //FLOWER
									if (addFlower == 1 && (e == 1 || !swapped)) tempBG[e].objectList.push_back({ -1, a, b + 7 - rand() % 10, 40, 70, flowerData });
									break;
								case -2: //MUSH
									if (addFlower == 1 && (e == 1 || !swapped)) {
										tempBG[e].objectList.push_back({ -2, a, b - 8, 80, 80, mushData });
									}
									break;
								case -3: //SPIKE
									if (addFlower == 1 && (e == 1 || !swapped)) tempBG[e].objectList.push_back({ -3, a, b + 2, 40, 70, spikeData });
									break;
								case -4: //EVIL CLOUD
									if (addFlower == 1 && (e == 1 || !swapped)) tempBG[e].objectList.push_back({ -4, a, b, 180, 100, eCloudData });
									break;
								case -5: //SPRING
									if (addFlower == 1 && (e == 1 || !swapped)) tempBG[e].objectList.push_back({ -5, a, b - 46, 80, 120, springData });
									break;
								case -6: //FIREBALLS
									if (addFlower == 1 && (e == 1 || !swapped)) tempBG[e].objectList.push_back({ -6, a - 36, b - 36, 200, 200, fireballData });
									break;
								case 91: //NFLOWER
									if (addFlower == 1) {
										nFlowerData.curFrame = rand() % 2;
										tempBG[e].objectList.push_back({ 91, a, b + 52, 129, 28, nFlowerData });
									}
									break;
								case 92: //UPSIDE DOWN SPIKE
									if (addFlower == 1 && (e == 1 || !swapped)) tempBG[e].objectList.push_back({ 92, a, b, 40, 70, uspikeData });
									break;
								default:
									break;
								}
							}
						}
					}
					//UPDATE/ DRAW OBJECTS
					for (int i = 0; i < tempBG[e].objectList.size(); i++) {
						int val = tempBG[e].objectList[i].id;
						float x = tempBG[e].objectList[i].x;
						float y = tempBG[e].objectList[i].y;
						bool deleted = false;
						switch (val) {
						case -1: //FLOWER
							tempBG[e].objectList[i].hitbox = { tempBG[e].xmov + x + 5 - cam.x, y + 5 - cam.y, 33, 68 };
							if (tempBG[e].xmov + x - cam.x < player.x + 150 && player.flying) {
								tempBG[e].objectList[i].anim.curFrame = 1;
								if (!tempBG[e].objectList[i].flowerOpened) {
									score += 25 + rand() % 10;
									tempBG[e].objectList[i].flowerOpened = true;
								}
							}
							break;
						case -2: //MUSH
							tempBG[e].objectList[i].hitbox = { tempBG[e].xmov + x + 15 - cam.x, y + 19 - cam.y, 38, 37 };
							break;
						case -3: //SPIKE
							tempBG[e].objectList[i].hitbox = { tempBG[e].xmov + x + 28 - cam.x, y + 51 - cam.y, 22, 35 };
							if (tempBG[e].xmov + x - cam.x < 600) tempBG[e].objectList[i].anim.curFrame = 1;
							break;
						case -4: //EVIL CLOUD
							tempBG[e].objectList[i].hitbox = { tempBG[e].xmov + x + 26 - cam.x, y + 19 - cam.y, 92, 50 };
							break;
						case -5: //SPRING
							tempBG[e].objectList[i].hitbox = { tempBG[e].xmov + x + 5 - cam.x, y + 57 - cam.y, 45, 39 };
							if (tempBG[e].objectList[i].anim.def == &springExtend && tempBG[e].objectList[i].anim.curFrame == 3)
								animSet(&tempBG[e].objectList[i].anim, &springStill);
							break;
						case -6: //FIREBALLS
							if (tempBG[e].xmov + tempBG[e].objectList[i].x - cam.x < 1100 && !tempBG[e].objectList[i].soundPlayed) {
								FMOD_System_PlaySound(fmod, fireballwarning, NULL, false, &fx);
								tempBG[e].objectList[i].soundPlayed = true;
							}
							if (tempBG[e].xmov + tempBG[e].objectList[i].x - cam.x < 820) {
								if (!pause && ((player.anim.def == &horseExplosion && player.anim.curFrame != 1) || player.anim.def != &horseExplosion)) {
									if (lvl != 2) tempBG[e].objectList[i].x -= 250 * deltaTime;
									else tempBG[e].objectList[i].x -= 350 * deltaTime;
								}
								tempBG[e].objectList[i].hitbox = { tempBG[e].xmov + 20 + x - cam.x, y + 86 - cam.y, 39, 33 };
							}
							if (tempBG[e].xmov + tempBG[e].objectList[i].x - cam.x < 300 && !tempBG[e].objectList[i].soundPlayed2) {
								FMOD_System_PlaySound(fmod, fireballpass, NULL, false, &fx);
								tempBG[e].objectList[i].soundPlayed2 = true;
							}
							if (tempBG[e].xmov + tempBG[e].objectList[i].x - cam.x < -200) {
								tempBG[e].objectList.erase(tempBG[e].objectList.begin() + i);
								i--;
								deleted = true;
							}
							break;
						case 92: //UPSIDE DOWN SPIKE
							tempBG[e].objectList[i].hitbox = { tempBG[e].xmov + x + 12 - cam.x, y - cam.y, 18, 47 };
							if (tempBG[e].xmov + x - cam.x < 600) tempBG[e].objectList[i].anim.curFrame = 1;
							break;
						default:
							break;
						}
						//delete items that pass by player in flight
						if (val <= -2 && val > -4 && player.flying && tempBG[e].xmov + tempBG[e].objectList[i].x < player.x + 100) {
							if (val == -2 || val == -3) {
								FMOD_System_PlaySound(fmod, pop, NULL, false, &fx);
								diamondList.push_back({ dia, tempBG[e].xmov + x + 40, y, 40, 40, {}, e, 0, 0 });
								diamondList.back().a = tempBG[e].xmov + tempBG[e].objectList[i].x + 30;
								diamondList.back().b = tempBG[e].objectList[i].y;
								diamondList.back().py = player.y;
							}
							puffBuffer.push_back({ 2, tempBG[e].xmov + tempBG[e].objectList[i].x - cam.x, tempBG[e].objectList[i].y, 100, 100, puffData });
							tempBG[e].objectList.erase(tempBG[e].objectList.begin() + i);
							i--;
							deleted = true;
						}
						//delete items on screen after flight ends
						else if ((val <= -2 && val > -4 || val == -6) && player.flyOver && tempBG[e].xmov + tempBG[e].objectList[i].x - cam.x < 780) {
							if (val == -2 || val == -3) {
								FMOD_System_PlaySound(fmod, pop, NULL, false, &fx);
								diamondList.push_back({ dia, tempBG[e].xmov + tempBG[e].objectList[i].x + 30, tempBG[e].objectList[i].y, 30, 30, {}, e, 0, 0 });
								diamondList.back().a = tempBG[e].xmov + tempBG[e].objectList[i].x + 30;
								diamondList.back().b = tempBG[e].objectList[i].y;
								diamondList.back().py = player.y;
							}
							puffBuffer.push_back({ 2, tempBG[e].xmov + tempBG[e].objectList[i].x - cam.x, tempBG[e].objectList[i].y, 100, 100, puffData });
							tempBG[e].objectList.erase(tempBG[e].objectList.begin() + i);
							i--;
							deleted = true;
						}
						if (!deleted) {
							if (!pause) updateObject(&tempBG[e].objectList[i], deltaTime);
							float a = tempBG[e].xmov + tempBG[e].objectList[i].x;
							if (a > cam.x - 500 && a < cam.x + 850) { //OPTIMIZATION
								objectDraw(&tempBG[e].objectList[i], objectTextures, tempBG[e].xmov + x - cam.x, y - cam.y);
							}
						}
					}
				}
				//for deleting all items at the end of flight
				if (player.flyOver) {
					jumpstack = 2;
					player.flyOver = false;
				}
				if (diamondList.size() > 0) { //draw the flying diamonds
					for (int i = 0; i < diamondList.size(); i++) {
						int pos = diamondList[i].pos;	//bg1 or bg2
						if (!pause) updateDiamond(&diamondList[i], &player, deltaTime);
						glDrawSprite(dia, diamondList[i].x - cam.x, diamondList[i].y - cam.y, 40, 40);
						diamondList[i].hitbox = { diamondList[i].x + 6 - cam.x, diamondList[i].y + 9 - cam.y, 28, 21 };
						if (diamondList[i].x - cam.x < -50) {
							diamondList.erase(diamondList.begin() + i);
						}
					}
				}
				//DRAW FLOATING PLATFORMS
				for (int e = 0; e < 2; e++) {
					for (int i = 0; i < tempBG[e].platforms.size(); i++) {
						for (int j = 0; j < tempBG[e].platforms[i].size(); j++) {
							float x = tempBG[e].platforms[i][j][0];
							float y = tempBG[e].platforms[i][j][1];
							if (j == 0 && (addFlower == 1 && (e == 1 || !swapped) || e == 0 && player.fever && feverfirst == 0))
								tempBG[e].platformList.push_back({ 1, x, y, { tempBG[e].xmov + x + 32 - cam.x, y + 6 - cam.y, 31, 52 } });
							else if (j == tempBG[e].platforms[i].size() - 1 && (addFlower == 1 && (e == 1 || !swapped) || e == 0 && player.fever && feverfirst == 0))
								tempBG[e].platformList.push_back({ 4, x, y, { tempBG[e].xmov + x - cam.x, y + 6 - cam.y, 63, 52 } });
							else if (addFlower == 1 && (e == 1 || !swapped) || (e == 0 && player.fever && feverfirst == 0))
								tempBG[e].platformList.push_back({ 2 + rand() % 2, x, y, { tempBG[e].xmov + x - cam.x, y + 6 - cam.y, 63, 52 } });
						}
					}
				}
				for (int e = 0; e < 2; e++) { //DRAW PLATFORMS/HITBOXES
					for (int i = 0; i < tempBG[e].platformList.size(); i++) {
						float a = tempBG[e].xmov + tempBG[e].platformList[i].x;
						if (a > cam.x - 140 && a < cam.x + 850) { //OPTIMIZATION
							glDrawSprite(plats[tempBG[e].platformList[i].id - 1], tempBG[e].xmov + tempBG[e].platformList[i].x - cam.x, tempBG[e].platformList[i].y, 63, 63);
						}
						if (!(player.fever && i == 3 || (player.fever && i == 4))) {
							switch (tempBG[e].platformList[i].id) {
							case 1:
								tempBG[e].platformList[i].hitbox = { tempBG[e].xmov + tempBG[e].platformList[i].x + 32 - cam.x, tempBG[e].platformList[i].y + 6, 31, 52 };
								break;
							case 2:
								tempBG[e].platformList[i].hitbox = { tempBG[e].xmov + tempBG[e].platformList[i].x - cam.x, tempBG[e].platformList[i].y + 6, 63, 52 };
								break;
							case 3:
								tempBG[e].platformList[i].hitbox = { tempBG[e].xmov + tempBG[e].platformList[i].x - cam.x, tempBG[e].platformList[i].y + 6, 63, 52 };
								break;
							case 4:
								tempBG[e].platformList[i].hitbox = { tempBG[e].xmov + tempBG[e].platformList[i].x - cam.x, tempBG[e].platformList[i].y + 6, 63, 52 };
								break;
							default:
								break;
							}
						}
					}
				}

				// Increment feverFirst. I think this is a variable that keeps the first platform after fever from messing up?
				if (player.fever && !pause) {
					feverfirst++;
				}

				if (!startAnim && lvl == 0 && initxmov < 400 * deltaTime) initxmov += 10 * deltaTime;
				//SHIFT PLATFORMS LEFT
				if (!pause && ((player.anim.def == &horseExplosion && player.anim.curFrame != 1) || player.anim.def != &horseExplosion && !startAnim)) {
					if ((lvl == 0 || (lvl == 1 && lvl1counter > 3)) && !player.fever) {
						tempBG[0].xmov -= initxmov;
						tempBG[1].xmov -= initxmov;
						puspeed = 1;
					}
					else if ((lvl == 1 && lvl1counter <= 3 || (lvl == 2 && lvl2cd > 0)) && !player.fever) {
						tempBG[0].xmov -= 550 * deltaTime;
						tempBG[1].xmov -= 550 * deltaTime;
						if (nick1) {
							what = 1;
							nick1 = false;
						}
						puspeed = 2;
					}
					else if (lvl == 2 && lvl2cd == 0 && !player.fever) {
						if (player.flying) {
							tempBG[0].xmov -= 1200 * deltaTime;
							tempBG[1].xmov -= 1200 * deltaTime;
						}
						else {
							tempBG[0].xmov -= 750 * deltaTime;
							tempBG[1].xmov -= 750 * deltaTime;
						}
						if (nick2) {
							what = 2;
							nick2 = false;
						}
						puspeed = 3;
					}
					else if (player.fever) {
						tempBG[0].xmov -= 1000 * deltaTime;
					}
				}

				//PLAYER DEATH FROM FALLING
				if ((player.y >= 500 || player.x <= -200) && !player.dead) {
					player.dead = true;
					player.anim.def = &horseExplosion;
					player.anim.curFrame = 0;
					player.anim.isPlaying = true;
					fadeOutEvent = 1;
					fader.anim.def = &fadeOut;
					fader.anim.curFrame = 0;
					screenShakeH = .4;
					player.h += 50;
					player.w += 40;
				}
				//PLAYER COME BACK FROM BEING PUSH LEFT
				if (player.x < 35) {
					player.x += 30 * deltaTime;
				}
				//*/


				//drawing the stars
				for (int i = 0; i < example.size(); i++) {
					glDrawSprite(co10, tempBG[0].xmov + example[i][0] - 12 - cam.x, example[i][1] - 12, 24, 24);
				}
				//drawing the platforms
				for (int i = 0; i < example2.size(); i++) {
					if (i == 0) glDrawSprite(p1, tempBG[0].xmov + example2[i][0] - cam.x, example2[i][1], 63, 63);
					else if (i == example2.size() - 1) glDrawSprite(p4, tempBG[0].xmov + example2[i][0] - cam.x, example2[i][1], 63, 63);
					else glDrawSprite(p2, tempBG[0].xmov + example2[i][0] - cam.x, example2[i][1], 63, 63);
				}

				//POWERUP GENERATOR
				int pupic = rand() % 2 + 1;
				if (powerupcounter < 0) {
					powerupcounter = 60 + rand() % 20; //CHANGE THIS
					if (animState == 0) {
						powerup = { pupic + 1, 900, 100 + rand() % 200, 70, 70, powerupData };
						powerup.anim.curFrame = pupic;
					}
					else {
						powerup = { 1, 900, 100 + rand() % 200, 70, 70, powerupData };
						powerup.anim.curFrame = 0;
					}
				}
				if (!player.fever && !startAnim) {
					powerupcounter -= deltaTime;
					if (!pause && ((player.anim.def == &horseExplosion && player.anim.curFrame != 1) || player.anim.def != &horseExplosion)) {
						if (puspeed == 1) powerup.x -= initxmov;
						else if (puspeed == 2)powerup.x -= 550 * deltaTime;
						else if (puspeed == 3) powerup.x -= 750 * deltaTime;
					}
					if (powerup.x > cam.x - 200 && powerup.id != 0) {
						objectDraw(&powerup, objectTextures, powerup.x, powerup.y);
						powerup.hitbox = { powerup.x, powerup.y, 70, 70 };
					}
					else powerup = { 0, -200, -200, 0, 0 };
				}

				//INVINCIBILITY
				if (invincibleCounter > 0 && !pause) {
					invincibleCounter -= deltaTime;
					if (invincibleStep > 0) invincibleStep--;
					else {
						invincibleStep = 2;
						visible = visible ? false : true;
					}
				}
				else if (!pause) {
					invincibleCounter = 0;
					visible = true;
				}

				//DRAW THE OWNER
				if (startAnim) objectDraw(&owner, playerTextures, tempBG[0].xmov + owner.x, owner.y);
				else if (tempBG[0].xmov + owner.x > -500 && drawOwner) objectDraw(&owner, playerTextures, tempBG[0].xmov + owner.x, owner.y);
				else drawOwner = false;


				//DRAW PLAYER
				if (visible) {
					playerDraw(&player, playerTextures, player.x - cam.x, player.y - cam.y);
				}

				glDrawSprite(overlay, 0, 0, 800, 550);
				//DRAW PUFFS
				if (!pause && puffBuffer.size() > 0) {
					for (int i = 0; i < puffBuffer.size(); i++) {
						updateObject(&puffBuffer[i], deltaTime);
						if (puspeed == 1) puffBuffer[i].x -= initxmov;
						else if (puspeed == 2) puffBuffer[i].x -= 550 * deltaTime;
						else if (puspeed == 3) puffBuffer[i].x -= 750 * deltaTime;
						else if (player.fever) puffBuffer[i].x -= 1000 * deltaTime;
						objectDraw(&puffBuffer[i], objectTextures, puffBuffer[i].x, puffBuffer[i].y);
						if (!puffBuffer[i].anim.isPlaying) puffBuffer.erase(puffBuffer.begin() + i);
					}
				}

				if (shield > 0 && !pause) {
					shieldOb.x = player.x;
					shieldOb.y = player.y;
					if (player.anim.def == &horseTwirlB || player.anim.def == &horseTwirlB) {
						shieldOb.x = player.x;
						shieldOb.y = player.y + 55;
					}
					updateObject(&shieldOb, deltaTime);
					objectDraw(&shieldOb, objectTextures, shieldOb.x - cam.x + 30, shieldOb.y - cam.x);
				}
				//ShieldTimer tick
				if (shieldTimer > 0 && !pause) {
					shieldTimer -= deltaTime;
					if (shieldTimer <= 0) {
						shield--;
					}
				}

				if (sparkleTrail.size() > 0) {
					for (int i = 0; i < sparkleTrail.size(); i++) {
						if (i % 2 == 0) glDrawSprite(sparkle, sparkleTrail[i].x - cam.x, sparkleTrail[i].y, sparkleTrail[i].w, sparkleTrail[i].h);
						else glDrawSprite(sparkle2, sparkleTrail[i].x - cam.x, sparkleTrail[i].y, sparkleTrail[i].w, sparkleTrail[i].h);
						if (!pause) {
							if (puspeed == 1) sparkleTrail[i].x -= 400 * deltaTime;
							else if (puspeed == 2) sparkleTrail[i].x -= 550 * deltaTime;
							else if (puspeed == 3) sparkleTrail[i].x -= 750 * deltaTime;
							sparkleTrail[i].w -= 50 * deltaTime;
							sparkleTrail[i].h -= 50 * deltaTime;
							sparkleTrail[i].y += 10 * deltaTime;
						}
						if (sparkleTrail[i].x < -100) sparkleTrail.erase(sparkleTrail.begin() + i);
					}
				}



				//BONUS EVENT
				if (bonusEvent == true) {
					std::stack<Number> boStack;
					int bo = bonus;
					int scoretens;
					int scoretenCount = 1;
					for (scoretens = 1; bo / scoretens != 0; scoretens *= 10, scoretenCount++);
					scoretens /= 10;
					scoretenCount--;
					while (scoretens != 0) {
						boStack.push(numbers[bo / scoretens]);
						bo %= scoretens;
						scoretens /= 10;
						scoretenCount--;
					}
					int bosize = boStack.size();
					int bowidth = 0;
					for (int i = 0; i < bosize; i++) {
						Number temp = boStack.top();
						tempNum.push_back(temp);
						if (i % 3 == 0 && i != 0) {
							bowidth += 10;
							tempNum.push_back({ scorecomma, 15, 50, 460 - bowidth, 15, -70 });
						}
						bowidth += temp.w;
						tempNum.push_back({ temp.img, temp.w, 50, 460 - bowidth, 15, -70 });
						boStack.pop();
					}
					tempNum.push_back({ bonusText, 200, 41, 460 - bowidth - 200, 15, -70 });
					bonusEvent = false;
				}
				for (int i = 0; i < tempNum.size(); i++) {
					updateNumber(&tempNum[i], deltaTime);
					glDrawSprite(tempNum[i].img, tempNum[i].x, tempNum[i].y, tempNum[i].w, tempNum[i].h);
					if (tempNum[i].count2 == 30) tempNum.erase(tempNum.begin() + i);
				}

				//RING EXPLOSION
				if (ringExplosion.anim.isPlaying) {
					updateObject(&ringExplosion, deltaTime);
					objectDraw(&ringExplosion, objectTextures, 0, 0);
				}

				if (player.flying  && player.initialCounter > 7) {
					glDrawSprite(flyforce, player.x, player.y - 40, 250, 200);
				}

				//SPEED UP EVENT
				if (what == 1 || what == 2) {
					if (puspeed == 2) {
						unsigned long long parentclock5;
						FMOD_Channel_GetDSPClock(lvl0chan, NULL, &parentclock5);
						FMOD_Channel_AddFadePoint(lvl0chan, parentclock5, 1.0f);
						FMOD_Channel_AddFadePoint(lvl0chan, parentclock5 + 10096, 0.0f);
						unsigned long long parentclock6;
						FMOD_Channel_GetDSPClock(lvl1chan, NULL, &parentclock6);
						FMOD_Channel_AddFadePoint(lvl1chan, parentclock6, 0.0f);
						FMOD_Channel_AddFadePoint(lvl1chan, parentclock6 + 10096, 1.0f);
						FMOD_System_PlaySound(fmod, lv1m, NULL, false, &lvl1chan);
					}
					else if (puspeed == 3) {
						unsigned long long parentclock7;
						FMOD_Channel_GetDSPClock(lvl1chan, NULL, &parentclock7);
						FMOD_Channel_AddFadePoint(lvl1chan, parentclock7, 1.0f);
						FMOD_Channel_AddFadePoint(lvl1chan, parentclock7 + 10096, 0.0f);
						unsigned long long parentclock8;
						FMOD_Channel_GetDSPClock(lvl2chan, NULL, &parentclock8);
						FMOD_Channel_AddFadePoint(lvl2chan, parentclock8, 0.0f);
						FMOD_Channel_AddFadePoint(lvl2chan, parentclock8 + 10096, 1.0f);
						FMOD_System_PlaySound(fmod, lv2m, NULL, false, &lvl2chan);
					}
					what = 3;
					speedupevent = true;
				}
				if (speedupevent) {
					speedRacer(&speedupscreen, deltaTime);
					glDrawSprite(speedupscreen.img, speedupscreen.x, speedupscreen.y, speedupscreen.w, speedupscreen.h);
				}
				if (speedupscreen.count2 == 50) {
					speedupscreen.count = 0;
					speedupscreen.count2 = 0;
					speedupevent = false;
					speedupscreen = { speeed, 800, 550, 800, 0, 0 };
					speedupscreen.dx = 800;
				}

				//DRAW UI
				glDrawSprite(ui, 0, 0, 800, 550);
				pbwidth = (powerbar * 282) / 1500;
				if (pbwidth > 282) pbwidth = 282;
				glDrawSprite(powerbarsp, 276, 516, pbwidth, 26);

				if (player.fever && player.feverCounter >= 200) {
					player.anim.isPlaying = true;
				}

				//CALCULATE/DRAW DISTANCE
				std::stack<Number> numStack;
				int dis = distance;
				int tens;
				int tenCount = 1;
				for (tens = 1; dis / tens != 0; tens *= 10, tenCount++);
				tens /= 10;
				tenCount--;
				while (tens != 0) {
					numStack.push(numbers[dis / tens]);
					dis %= tens;
					tens /= 10;
					tenCount--;
				}
				int numsize = numStack.size();
				int numwidth = 0;
				for (int i = 0; i < numsize; i++) {
					Number temp = numStack.top();
					if (i % 3 == 0 && i != 0) {
						numwidth += 15;
						glDrawSprite(comma, 715 - numwidth, 14, 15, 50);
					}
					numwidth += temp.w;
					glDrawSprite(temp.img, 715 - numwidth, 14, temp.w, 50);
					numStack.pop();
				}
				if (!pause && !player.dead && !player.fever && !startAnim) {
					switch (lvl) {
					case 0:
						distance = !player.flying ? distance += 8 * deltaTime : distance + 12 * deltaTime;
						break;
					case 1:
						distance = !player.flying ? distance += 11 * deltaTime : distance + 17 * deltaTime;
						break;
					case 2:
						distance = !player.flying ? distance += 15 * deltaTime : distance + 25 * deltaTime;
						break;
					default:
						break;
					}
				}
				else if (!pause && !player.dead && player.fever) distance += 20 * deltaTime;
				//DRAW SCORE
				std::stack<Number> scoStack;
				int sco = score;
				int scoretens;
				int scoretenCount = 1;
				for (scoretens = 1; sco / scoretens != 0; scoretens *= 10, scoretenCount++);
				scoretens /= 10;
				scoretenCount--;
				while (scoretens != 0) {
					scoStack.push(scoreNumbers[sco / scoretens]);
					sco %= scoretens;
					scoretens /= 10;
					scoretenCount--;
				}
				if (score == 0) {
					glDrawSprite(scoreNumbers[0].img, 155 - 21, 24, 17, 25);
				}
				else {
					int scosize = scoStack.size();
					int scowidth = 0;
					for (int i = 0; i < scosize; i++) {
						Number temp = scoStack.top();
						if (i % 3 == 0 && i != 0) {
							scowidth += 10;
							glDrawSprite(scorecomma, 155 - scowidth, 20, 10, 30);
						}
						scowidth += 17;
						glDrawSprite(temp.img, 155 - scowidth, 24, 17, 25);
						scoStack.pop();
					}
				}

				//DRAW PAUSE SCREEN
				if (pause) {
					if (onceperpause) {
						FMOD_Channel_SetPaused(lvl0chan, 1);
						FMOD_Channel_SetPaused(lvl1chan, 1);
						FMOD_Channel_SetPaused(lvl2chan, 1);
						onceperpause = false;
					}
					if (selectorPos2 == 1 && (kbState[SDL_SCANCODE_RETURN] || kbState[SDL_SCANCODE_SPACE])) {
						pause = false;
					}
					else if (selectorPos2 == 2 && (kbState[SDL_SCANCODE_RETURN] || kbState[SDL_SCANCODE_SPACE])) {
						if (fadeOutEvent == 0) {
							fadeOutEvent = 2;
							fader.anim.def = &fadeOut;
							fader.anim.curFrame = 0;
						}
					}
					else if (selectorPos2 == 3 && (kbState[SDL_SCANCODE_RETURN] || kbState[SDL_SCANCODE_SPACE])) {
						selectorPos = 1;
						gameState = GS_TitleScreen;
						fadeOnce = true;
						playTitleMusic = true;
					}
					glDrawSprite(pausescreen, 0, 0, 800, 550);
					if (selectorPos2 == 1) glDrawSprite(selector, 302, 200, 30, 30);
					else if (selectorPos2 == 2) glDrawSprite(selector, 302, 243, 30, 30);
					else if (selectorPos2 == 3) glDrawSprite(selector, 302, 285, 30, 30);
				}
				if (!pause) {
					onceperpause = true;
					selectorPos2 = 1;
					switch (lvl) {
					case 0:
						FMOD_Channel_SetPaused(lvl0chan, 0);
						break;
					case 1:
						if (lvl1counter > 3 && !player.fever) FMOD_Channel_SetPaused(lvl0chan, 0);
						if (animState == 0 && ((lvl == 1 && lvl1counter <= 3 || (lvl == 2 && lvl2cd > 0)) && !player.fever)) FMOD_Channel_SetPaused(lvl1chan, 0);
						else FMOD_Channel_SetPaused(lvl2chan, 0);
						break;
					case 2:
						FMOD_Channel_SetPaused(lvl2chan, 0);
						break;
					default:
						break;
					}
				}

				//FADE IN
				if (fader.anim.curFrame != 7 && fader.anim.def == &fadeIn) {
					fader.anim.isPlaying = true;
					animDraw(&faderp->anim, fade, 0, 0, 800, 550);
					animTick(&faderp->anim, deltaTime);
				}

				//FADE OUT
				if (fadeOutEvent > 0 && (player.anim.curFrame == 10 && player.anim.def == &horseExplosion || pause)) {
					if (fader.anim.curFrame != 7) {
						fader.anim.isPlaying = true;
						animDraw(&faderp->anim, fade, 0, 0, 800, 550);
						animTick(&faderp->anim, deltaTime);
					}
					else {
						if (fadeOutEvent == 2) {
							tempBG = { bg1, bg2 };
							if (!startAnim) tempBG[0].xmov = -900;
							tempBG[1].xmov = 2000;
							lvl = 0;
							jumpstack = 2;
							lvl1counter = 5;
							lvl2cd = 2;
							score = 0;
							bonus = 0;
							distance = 0;
							inCount = 2;
							swapped = false;
							pause = false;
							addFlower = 2;
							cam.x = 0;
							if (player.anim.def == &horseTwirlB || player.anim.def == &horseTwirlB) {
								player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
								player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
								animSet(&player.anim, &playerWalkB);
							}
							player.anim.def = &playerWalkB;
							fader.anim.def = &fadeIn;
							fader.anim.curFrame = 0;
							diamondList.clear();
							backdrop[0].x = 0;
							backdrop[1].x = 0;
							endDis = 0;
							endDisStep = 0;
							endScore = 0;
							endBonus = 0;
							endFinal = 0;
							hscount = .4;
							hspos = 5;
							copyOnce = 0;
							powerbar = 0;
							animState = 0;
							invincibleStep = 2;
							shield = 0;
							invincibleCounter = 0;
							visible = true;
							player.fever = false;
							powerup = { 0, -200, -200, 0, 0 };
							powerupcounter = 20 + rand() % 20;
							puspeed = 1;
							totalBonus = 0;
							bonusEvent = false;
							tempNum.clear();
							speedupscreen = { speeed, 800, 550, 800, 0, 0 };
							speedupevent = false;
							speedupscreen.dx = 800;
							lookBack.clear();
							dieonce = false;
							oneApplause = false;
							shieldTimer = 0;
							FMOD_Channel_Stop(lvl0chan);
							FMOD_Channel_Stop(lvl1chan);
							FMOD_Channel_Stop(lvl2chan);
							FMOD_Channel_Stop(bgChan);
							FMOD_Channel_SetPaused(lvl0chan, 0);
							FMOD_Channel_SetPaused(lvl1chan, 1);
							FMOD_Channel_SetPaused(lvl2chan, 1);
							FMOD_Channel_SetPaused(bgChan, 1);
							nick1 = true;
							nick2 = true;
							what = 0;
							FMOD_System_PlaySound(fmod, lv0m, NULL, false, &lvl0chan);
							player = {
								{ &playerWalkB, 0, 0, true },
								40, 300, player_width, player_height,
								0, {}, 0, 0, 0, 0, 0, 0, 0, 0, 0
							};
							initxmov = 0;
						}
						else {
							player.w -= 40;
							player.h -= 50;
							diamondList.clear();
							gameState = GS_End;
						}
						animDraw(&faderp->anim, fade, 0, 0, 800, 550);
						fadeOutEvent = 0;
						fader.anim.def = &fadeIn;
						fader.anim.curFrame = 0;
					}
				}
				//DEATH
				if (player.dead) {
					if (!dieonce) {
						FMOD_Channel_Stop(lvl0chan);
						FMOD_Channel_Stop(lvl1chan);
						FMOD_Channel_Stop(lvl2chan);
						FMOD_Channel_Stop(bgChan);
						FMOD_Channel_SetPaused(lvl0chan, 1);
						FMOD_Channel_SetPaused(lvl1chan, 1);
						FMOD_Channel_SetPaused(lvl2chan, 1);
						FMOD_Channel_SetPaused(bgChan, 1);
						FMOD_System_PlaySound(fmod, death, NULL, false, &fx);
						dieonce = true;
					}
					if (player.anim.curFrame != 1 & player.anim.curFrame != 0) {
						if (lvl == 0) player.x -= initxmov; //LEVEL 1
						else if (lvl == 1) player.x -= 550;
					}
					else if (screenShakeH > 0) {
						if (flipH) {
							cam.x += 16;
							flipH = false;
						}
						else {
							cam.x -= 15;
							flipH = true;
						}
						screenShakeH -= deltaTime;
					}
				}

				if (currentFrameMs - lastPhysicsFrameMs > 100) lastPhysicsFrameMs = currentFrameMs;
				//=======================================================PHYSICS============================================================
				int yfirst = 0; int xfirst = 0; int fallfirst = 0; int onceper = 0; int plato = 0;  int plato2 = 0; int plato3 = 0;
				do {
					lastPhysicsFrameMs += physicsDeltaMs;
					if (!pause && !player.dead) {
						if (++fallfirst == 1 && !player.flying && !(player.fever && player.feverCounter < 50)) { //fall
							if (lvl != 2) player.yvel -= 52 * deltaTime;
							else player.yvel -= 55 * deltaTime;
							player.y -= player.yvel;
							player.hitbox.y -= player.yvel;
						}//*/
						//collision with main tiles
						for (int e = 0; e < 2; e++) {
							for (int i = 0; i < tempBG[e].tileMap.size(); i++) {
								for (int j = 0; j < tempBG[e].tileMap[i].size(); j++) {
									if (tempBG[e].tileMap[i][j] > 0 && tempBG[e].tileMap[i][j] <= 90) {
										float a;
										if (e == 0) a = tileBox[i][j].x;
										else if (e == 1) a = tileBox2[i][j].x;
										if (a > -200 && a < 800) { //OPTIMIZATION
											bool box1 = false;
											bool box2 = false;
											if (e == 0) box1 = tempBG[0].tileMap[i][j] > 0 && AABBIntersect(&player.hitbox, tileBoxPointer[i][j]);
											if (e == 1) box2 = tempBG[1].tileMap[i][j] > 0 && AABBIntersect(&player.hitbox, tileBoxPointer2[i][j]);
											if ((e == 0 && box1) || (e == 1 && box2)) {
												float playerH = player.hitbox.y + player.hitbox.h;
												float tileH;
												if (box1) tileH = tileBox[i][j].y;
												else tileH = tileBox2[i][j].y;
												float ydif = abs(tileH - playerH);

												float playerW = player.hitbox.x + player.hitbox.w;
												float tileW;
												if (box1) tileW = tileBox[i][j].x;
												else tileW = tileBox2[i][j].x;
												float xdif = abs(tileW - playerW);
												if (playerH > tileH && ydif < 40 && ++yfirst == 1 && player.yvel <= 0) {
													if (i > 0 && tempBG[e].tileMap[i - 1].size() >= j && !(tempBG[e].tileMap[i - 1][j] > 0 && tempBG[e].tileMap[i - 1][j] <= 90)) {
														player.yvel = 0;
														player.y -= ydif;
														if (player.anim.def == &horseTwirlB) {
															player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
															player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
															player.anim.def = &playerWalkB;
														}
														else if (player.anim.def == &horseTwirl) {
															player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
															player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
															player.anim.def = &playerWalk;
														}
														if (player.anim.def == &horseJumpB) animSet(&player.anim, &playerWalkB);
														else if (player.anim.def == &horseJumpp) animSet(&player.anim, &playerWalk);
														if (animState == 1) jumpstack = 3;
														else jumpstack = 2;
														xfirst++;
													}
												}
												else if (playerW > tileW && ++xfirst == 1 && xdif < 30) {
													if (j > 0 && !(tempBG[e].tileMap[i][j - 1] > 0 && tempBG[e].tileMap[i][j - 1] <= 90) && playerH - tileH > 30)
														player.x -= xdif;
													else if (j == 0 && ydif != 0) {
														player.x -= xdif;
													}
												}
											}
										}
									}
								}
							}
							//collision with floating platforms and slopes
							float shift = 0;
							for (int i = 0; i < tempBG[e].platformList.size(); i++) {
								float x = tempBG[e].platformList[i].x;
								float y = tempBG[e].platformList[i].y;
								if (AABBIntersect(&player.hitbox, &tempBG[e].platformList[i].hitbox)) {
									float playerH = player.hitbox.y + player.hitbox.h;
									float tileH = tempBG[e].platformList[i].hitbox.y;
									float ydif = abs(tileH - playerH);

									if (tempBG[e].platformList[i].id != 1) {
										float dif = abs(tempBG[e].platformList[i].hitbox.y - tempBG[e].platformList[i - 1].hitbox.y);
										float tileXDif = tempBG[e].platformList[i].hitbox.x + tempBG[e].platformList[i].hitbox.w;
										float playerXDif = player.hitbox.x + player.hitbox.w;
										float math = (dif * (tileXDif - playerXDif)) / tempBG[e].platformList[i].hitbox.w;
										if (tempBG[e].platformList[i].hitbox.y < tempBG[e].platformList[i - 1].hitbox.y && plato == 0 && //UP SLOPE NOT BEGIN OR END
											(ydif < 30 && player.yvel <= 0) && tempBG[e].platformList[i].id != 4
											&& player.hitbox.y + player.hitbox.h > tempBG[e].platformList[i].hitbox.y + math && tileXDif > playerXDif) {
											shift = ((player.hitbox.y + player.hitbox.h) - (tempBG[e].platformList[i].hitbox.y + math));
											player.yvel = 0;
											if (player.anim.def == &horseTwirlB) {
												player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
												player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
												player.anim.def = &playerWalkB;
											}
											else if (player.anim.def == &horseTwirl) {
												player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
												player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
												player.anim.def = &playerWalk;
											}
											if (player.anim.def == &horseJumpB) animSet(&player.anim, &playerWalkB);
											else if (player.anim.def == &horseJumpp) animSet(&player.anim, &playerWalk);
											if (animState == 1) jumpstack = 3;
											else jumpstack = 2;

										}
										else if (tempBG[e].platformList[i].hitbox.y < tempBG[e].platformList[i - 1].hitbox.y && plato == 0 && //UP SLOPE END
											(ydif < 30 && player.yvel <= 0) && player.hitbox.y + player.hitbox.h > tempBG[e].platformList[i].hitbox.y + math) {
											shift = ((player.hitbox.y + player.hitbox.h) - (tempBG[e].platformList[i].hitbox.y + math));
											player.yvel = 0;
											if (player.anim.def == &horseTwirlB) {
												player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
												player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
												player.anim.def = &playerWalkB;
											}
											else if (player.anim.def == &horseTwirl) {
												player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
												player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
												player.anim.def = &playerWalk;
											}
											if (player.anim.def == &horseJumpB) animSet(&player.anim, &playerWalkB);
											else if (player.anim.def == &horseJumpp) animSet(&player.anim, &playerWalk);
											if (animState == 1) jumpstack = 3;
											else jumpstack = 2;
										}
										else if (tempBG[e].platformList[i].hitbox.y == tempBG[e].platformList[i - 1].hitbox.y && //EQUAL PLATFORMS
											(ydif < 30 && player.yvel <= 0) && playerH > tileH && ++plato3 == 1) {
											player.yvel = 0;
											player.y -= ydif;
											if (player.anim.def == &horseTwirlB) {
												player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
												player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
												player.anim.def = &playerWalkB;
											}
											else if (player.anim.def == &horseTwirl) {
												player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
												player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
												player.anim.def = &playerWalk;
											}
											if (player.anim.def == &horseJumpB) animSet(&player.anim, &playerWalkB);
											else if (player.anim.def == &horseJumpp) animSet(&player.anim, &playerWalk);
											if (animState == 1)  jumpstack = 3;
											else jumpstack = 2;
										}

										float tileXDif2 = player.hitbox.x - tempBG[e].platformList[i].hitbox.x;
										float math2 = abs((dif * (tileXDif2)) / tempBG[e].platformList[i].hitbox.w);
										if ((tempBG[e].platformList[i].id < 4) && //DOWN SLOPE
											tempBG[e].platformList[i].hitbox.y < tempBG[e].platformList[i + 1].hitbox.y && plato2 == 0) {
											shift = ((player.hitbox.y + player.hitbox.h) - (tempBG[e].platformList[i].hitbox.y + math2));
											player.yvel = 0;
											if (player.anim.def == &horseTwirlB) {
												player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
												player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
												player.anim.def = &playerWalkB;
											}
											else if (player.anim.def == &horseTwirl) {
												player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
												player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
												player.anim.def = &playerWalk;
											}
											if (player.anim.def == &horseJumpB) animSet(&player.anim, &playerWalkB);
											else if (player.anim.def == &horseJumpp) animSet(&player.anim, &playerWalk);
											if (animState == 1)  jumpstack = 3;
											else jumpstack = 2;
										}

									}
									//FIRST PLATFORM BLOCK
									if (tempBG[e].platformList[i].id == 1 && player.yvel < 0 && playerH > tileH && ((ydif < 30 && player.yvel <= 0) || ydif < 30)) {
										player.yvel = 0;
										player.y -= ydif;
										if (player.anim.def == &horseTwirlB) {
											player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
											player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
											player.anim.def = &playerWalkB;
										}
										else if (player.anim.def == &horseTwirl) {
											player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
											player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
											player.anim.def = &playerWalk;
										}
										if (player.anim.def == &horseJumpB) animSet(&player.anim, &playerWalkB);
										else if (player.anim.def == &horseJumpp) animSet(&player.anim, &playerWalk);
										if (animState == 1) jumpstack = 3;
										else jumpstack = 2;
									}

								}
							}
							player.y -= shift;
						}
						plato++;
						plato2++;
						plato3++;

						//collision with stars & objects
						for (int e = 0; e < 2; e++) {
							for (int a = 0; a < tempBG[e].starMap.size(); a++) {
								for (int i = 0; i < tempBG[e].starMap[a].starList.size(); i++) {
									float k = tempBG[e].starMap[a].starList[i].x;
									float g = tempBG[e].xmov + k;
									if (((player.flying || tempBG[e].starMap[a].starList[i].finishing) && (k > cam.x - 150 && k < cam.x + 800)) ||
										(!player.flying && (g > cam.x - 200 && g < cam.x + 800))) { //OPTIMIZATION
										if (AABBIntersect(&player.hitbox, &tempBG[e].starMap[a].starList[i].hitbox)) {
											if (!player.fever){
												FMOD_System_PlaySound(fmod, coins[rand() % 10], NULL, false, &coinChan);
												FMOD_Channel_SetVolumeRamp(coinChan, false);
												FMOD_Channel_SetVolume(coinChan, 0.1f);
											}
											if (!player.flying && !player.magnet)
												sparkleTrail.push_back({ tempBG[e].xmov + tempBG[e].starMap[a].starList[i].x - 30,
												tempBG[e].starMap[a].starList[i].y - 30, 80 + rand() % 20, 80 + rand() % 20 });
											else sparkleTrail.push_back({ tempBG[e].starMap[a].starList[i].x - 30, tempBG[e].starMap[a].starList[i].y - 30,
												80 + rand() % 20, 80 + rand() % 20 });
											tempBG[e].starMap[a].starList.erase(tempBG[e].starMap[a].starList.begin() + i);
											tempBG[e].starMap[a].count--;
											if (!player.fever) score += 10 + rand() % 10;
											else score += rand() % 50;
											if (tempBG[e].starMap[a].count == 0) { //BONUS!!
												bonus = tempBG[e].starMap.size() * 5 + rand() % 20;
												bonusEvent = true;
												totalBonus += bonus;
												powerbar += 10;				//FIX THIS!!!!!
											}
											if (!player.fever) powerbar += 7;
										}
									}
								}
							}
							std::vector<int> temp;
							for (int a = 0; a < tempBG[e].objectList.size(); a++) {
								float k = tempBG[e].xmov + tempBG[e].objectList[a].x;
								if (k > cam.x - 200 && k < cam.x + 800) { //OPTIMIZATION
									if (AABBIntersect(&player.hitbox, &tempBG[e].objectList[a].hitbox) && player.flying == false) {
										float py = player.hitbox.y + player.hitbox.h;
										float oy = tempBG[e].objectList[a].hitbox.y;
										float ptop = player.hitbox.y;
										float mtop = tempBG[e].objectList[a].hitbox.y + tempBG[e].objectList[a].hitbox.h;
										switch (tempBG[e].objectList[a].id) {
										case -1: //FLOWER
											if (!tempBG[e].objectList[a].flowerOpened) {
												FMOD_System_PlaySound(fmod, flowersound, NULL, false, &fx);
												tempBG[e].objectList[a].flowerOpened = true;
												tempBG[e].objectList[a].anim.curFrame = 1;
												score += (15.0 + rand() % 10);
											}
											break;
										case -2: //MUSHROOM
											if (py >= oy && abs(py - oy) <= 40 && !player.dead) {
												FMOD_System_PlaySound(fmod, pop, NULL, false, &fx);
												//	FMOD_Channel_SetVolumeRamp(fx, false);
												//	FMOD_Channel_SetVolume(fx, 0.2f);
												temp.push_back(a);
												player.y -= abs(py - oy);
												player.yvel = 1250 * physicsDeltaTime;
												if (animState == 1) jumpstack = 2;
												else jumpstack = 1;
												if (player.anim.def == &horseTwirlB) {
													player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
													player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
													player.anim.def = &horseJumpB;
												}
												else if (player.anim.def == &horseTwirl) {
													player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
													player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
													player.anim.def = &horseJumpp;
												}
												else if (player.anim.def == &playerWalk) player.anim.def = &horseJumpp;
												else if (player.anim.def == &playerWalkB) player.anim.def = &horseJumpB;
												player.anim.curFrame = 0;
												player.anim.isPlaying = true;
											}
											else if (ptop <= mtop && mtop - ptop <= 40 && !player.dead) {
												FMOD_System_PlaySound(fmod, pop, NULL, false, &fx);
												//	FMOD_Channel_SetVolumeRamp(fx, false);
												//	FMOD_Channel_SetVolume(fx, 0.2f);
												temp.push_back(a);
												player.y += abs(ptop - mtop);
												player.yvel = -550 * physicsDeltaTime;
												if (player.anim.def == &horseTwirlB) {
													player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
													player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
													player.anim.def = &horseJumpB;
												}
												else if (player.anim.def == &horseTwirl) {
													player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
													player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
													player.anim.def = &horseJumpp;
												}
												else if (player.anim.def == &playerWalk) player.anim.def = &horseJumpp;
												else if (player.anim.def == &playerWalkB) player.anim.def = &horseJumpB;
												player.anim.curFrame = 5;
												player.anim.isPlaying = false;
												jumpstack = 0;
											}
											else if (invincibleCounter == 0) { //DIE
												if (shield > 0 && invincibleCounter == 0) {
													player.yvel = 700 * physicsDeltaTime;
													if (animState == 1) jumpstack = 2;
													else jumpstack = 1;
													if (player.anim.def == &horseTwirlB) {
														player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
														player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
														player.anim.def = &horseJumpB;
													}
													else if (player.anim.def == &horseTwirl) {
														player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
														player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
														player.anim.def = &horseJumpp;
													}
													else if (player.anim.def == &playerWalk) player.anim.def = &horseJumpp;
													else if (player.anim.def == &playerWalkB) player.anim.def = &horseJumpB;
													player.anim.curFrame = 0;
													player.anim.isPlaying = true;
													invincibleCounter = 1.5;
													shield--;
												}
												else if (invincibleCounter == 0) {
													player.dead = true;
													player.anim.def = &horseExplosion;
													player.anim.curFrame = 0;
													player.anim.isPlaying = true;
													fadeOutEvent = 1;
													fader.anim.def = &fadeOut;
													fader.anim.curFrame = 0;
													screenShakeH = .4;
													player.h += 50;
													player.w += 40;
												}
											}
											break;
										case -3: //SPIKES
											if (shield > 0 && invincibleCounter == 0) {
												FMOD_System_PlaySound(fmod, pop, NULL, false, &fx);
												player.yvel = 1390 * physicsDeltaTime;
												if (animState == 1) jumpstack = 2;
												else jumpstack = 1;
												if (player.anim.def == &horseTwirlB) {
													player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
													player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
													player.anim.def = &horseJumpB;
												}
												else if (player.anim.def == &horseTwirl) {
													player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
													player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
													player.anim.def = &horseJumpp;
												}
												else if (player.anim.def == &playerWalk) player.anim.def = &horseJumpp;
												else if (player.anim.def == &playerWalkB) player.anim.def = &horseJumpB;
												player.anim.curFrame = 0;
												player.anim.isPlaying = true;
												invincibleCounter = 1.5;
												shield--;
											}
											else if (invincibleCounter == 0 && !player.flying) {
												player.dead = true;
												player.anim.def = &horseExplosion;
												player.anim.curFrame = 0;
												player.anim.isPlaying = true;
												fadeOutEvent = 1;
												fader.anim.def = &fadeOut;
												fader.anim.curFrame = 0;
												screenShakeH = .4;
												player.h += 50;
												player.w += 40;
											}
											break;
										case -4: //Evil Cloud
											if (shield > 0 && invincibleCounter == 0) {
												FMOD_System_PlaySound(fmod, pop, NULL, false, &fx);
												player.yvel = 1390 * physicsDeltaTime;
												if (animState == 1) jumpstack = 2;
												else jumpstack = 1;
												if (player.anim.def == &horseTwirlB) {
													player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
													player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
													player.anim.def = &horseJumpB;
												}
												else if (player.anim.def == &horseTwirl) {
													player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
													player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
													player.anim.def = &horseJumpp;
												}
												else if (player.anim.def == &playerWalk) player.anim.def = &horseJumpp;
												else if (player.anim.def == &playerWalkB) player.anim.def = &horseJumpB;
												player.anim.curFrame = 0;
												player.anim.isPlaying = true;
												invincibleCounter = 1.5;
												shield--;
											}
											else if (invincibleCounter == 0 && !player.flying && !player.dead) {
												player.dead = true;
												player.anim.def = &horseExplosion;
												player.anim.curFrame = 0;
												player.anim.isPlaying = true;
												fadeOutEvent = 1;
												fader.anim.def = &fadeOut;
												fader.anim.curFrame = 0;
												screenShakeH = .4;
												player.h += 50;
												player.w += 40;
											}
											break;
										case -5: //Spring
											player.yvel = 2400 * physicsDeltaTime;
											animSet(&tempBG[e].objectList[a].anim, &springExtend);
											player.anim.curFrame = 0;
											player.anim.isPlaying = true;
											if (animState == 1) jumpstack = 2;
											else jumpstack = 1;
											break;
										case -6: //Fireballs
											if (shield > 0 && invincibleCounter == 0) {
												FMOD_System_PlaySound(fmod, pop, NULL, false, &fx);
												player.yvel = 1390 * physicsDeltaTime;
												if (animState == 1) jumpstack = 2;
												else jumpstack = 1;
												if (player.anim.def == &horseTwirlB) {
													player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
													player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
													player.anim.def = &horseJumpB;
												}
												else if (player.anim.def == &horseTwirl) {
													player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
													player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
													player.anim.def = &horseJumpp;
												}
												else if (player.anim.def == &playerWalk) player.anim.def = &horseJumpp;
												else if (player.anim.def == &playerWalkB) player.anim.def = &horseJumpB;
												player.anim.curFrame = 0;
												player.anim.isPlaying = true;
												invincibleCounter = 1.5;
												shield--;
											}
											else if (invincibleCounter == 0 && !player.flying && !player.dead) {
												player.dead = true;
												player.anim.def = &horseExplosion;
												player.anim.curFrame = 0;
												player.anim.isPlaying = true;
												fadeOutEvent = 1;
												fader.anim.def = &fadeOut;
												fader.anim.curFrame = 0;
												screenShakeH = .4;
												player.h += 50;
												player.w += 40;
											}
											break;
										default:
											break;
										}
									}
								}
							}

							if (temp.size() > 0 && onceper == 0) { //for multiple mushrooms
								int random = rand() % temp.size();
								int ind = temp[random];
								diamondList.push_back({ dia, tempBG[e].xmov + tempBG[e].objectList[ind].x + 30, tempBG[e].objectList[ind].y, 30, 30, {}, e, 0, 0 });
								diamondList.back().a = tempBG[e].xmov + tempBG[e].objectList[ind].x + 30;
								diamondList.back().b = tempBG[e].objectList[ind].y;
								diamondList.back().py = player.y;
								puffBuffer.push_back({ 2, tempBG[e].xmov + tempBG[e].objectList[ind].x - cam.x, tempBG[e].objectList[ind].y, 100, 100, puffData });
								tempBG[e].objectList.erase(tempBG[e].objectList.begin() + ind);
								onceper = 1;
							}
						}

						//collision with diamonds
						for (int i = 0; i < diamondList.size(); i++) {
							if (AABBIntersect(&player.hitbox, &diamondList[i].hitbox) && diamondList[i].initialCounter >= 50) {
								FMOD_System_PlaySound(fmod, diamondsound, NULL, false, &fx);
								diamondList.erase(diamondList.begin() + i);
								score += 80 + rand() % 40;
							}
						}
					}

					//COLLISION WITH POWERUP
					if (AABBIntersect(&player.hitbox, &powerup.hitbox)) {
						switch (powerup.id) {
						case 1: // FLY
							if (!player.flying && !player.dead) {
								FMOD_System_PlaySound(fmod, powerupsound, NULL, false, &fx);
								if (player.anim.def == &horseTwirlB || player.anim.def == &horseTwirl) {
									player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
									player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
								}
								player.a = player.y;
								if (animState == 1) animSet(&player.anim, &horseJumpp);
								else animSet(&player.anim, &horseJumpB);
								player.anim.curFrame = 3;
								player.anim.isPlaying = false;
								player.flying = true;
								player.flyCounter = 5;
								powerup = { 0, -200, -200, 0, 0, };
							}
							break;
						case 2: //Magnet
							FMOD_System_PlaySound(fmod, powerupsound, NULL, false, &fx);
							player.magnet = true;
							player.magCounter = 7;
							powerup = { 0, -200, -200, 0, 0, };
							break;
						case 3: //SHIELD
							FMOD_System_PlaySound(fmod, powerupsound, NULL, false, &fx);
							shield++;
							shieldTimer = 20;
							powerup = { 0, -200, -200, 0, 0, };
							break;
						default:
							break;
						}
					}
					//Decrement tc1
					if (player.fever && !pause) tc1 -= physicsDeltaTime;
				} while (lastPhysicsFrameMs + physicsDeltaMs < currentFrameMs);//*/
				addFlower = addFlower > 0 ? --addFlower : 0;
			}
			break;
		}
		case GS_End:{
			ifstream is("highscores.txt");
			istream_iterator<int> start(is), end;
			std::vector<int> scores2(start, end);
			std::sort(scores2.begin(), scores2.end());
			if (scores2.size() < 6) {
				for (int i = 0; i < 6 - scores2.size(); i++) {
					scores2.insert(scores2.begin(), 0);
				}
			}

			glDrawSprite(endscreen, 0, 0, 800, 550);
			//FADE IN
			if (endFade == 0 && fader.anim.curFrame != 7) {
				fader.anim.isPlaying = true;
				animDraw(&faderp->anim, fade, 0, 0, 800, 550);
				animTick(&faderp->anim, deltaTime);
			}
			else {
				endDisStep = distance / 20;
				if (endDis < distance) { //DRAW DISTANCE
					endDis += endDisStep;
					if (endDis > distance) endDis = distance;
				}
				std::stack<Number> numStack;
				int dis = endDis;
				int tens;
				int tenCount = 1;
				for (tens = 1; dis / tens != 0; tens *= 10, tenCount++);
				tens /= 10;
				tenCount--;
				while (tens != 0) {
					numStack.push(numbers[dis / tens]);
					dis %= tens;
					tens /= 10;
					tenCount--;
				}
				int numsize = numStack.size();
				int numwidth = 0;
				for (int i = 0; i < numsize; i++) {
					Number temp = numStack.top();
					if (i % 3 == 0 && i != 0) {
						numwidth += 15;
						glDrawSprite(comma, 200 - numwidth, 73, 10, 40);
					}
					numwidth += temp.w * .8;
					glDrawSprite(temp.img, 200 - numwidth, 73, temp.w * .8, 40);
					numStack.pop();
				}
				if (endDis == distance) { //DRAW SCORE
					endDisStep = score / 20.0;
					if (endScore < score) {
						endScore += endDisStep;
						if (endScore > score) endScore = score;
					}
					if (score == 0) glDrawSprite(numbers[0].img, 200 - 21, 195, 18, 36);
					std::stack<Number> numStack;
					int dis = endScore;
					int tens;
					int tenCount = 1;
					for (tens = 1; dis / tens != 0; tens *= 10, tenCount++);
					tens /= 10;
					tenCount--;
					while (tens != 0) {
						numStack.push(numbers[dis / tens]);
						dis %= tens;
						tens /= 10;
						tenCount--;
					}
					int numsize = numStack.size();
					int numwidth = 0;
					for (int i = 0; i < numsize; i++) {
						Number temp = numStack.top();
						if (i % 3 == 0 && i != 0) {
							numwidth += 15;
							glDrawSprite(comma, 200 - numwidth, 195, 10, 40);
						}
						numwidth += temp.w * .8;
						glDrawSprite(temp.img, 200 - numwidth, 195, temp.w * .8, 40);
						numStack.pop();
					}
				}
				if (endScore == score) { //DRAW BONUS
					endDisStep = totalBonus / 20.0;
					if (endBonus < totalBonus) {
						endBonus += endDisStep;
						if (endBonus > bonus) endBonus = totalBonus;
					}
					if (totalBonus == 0) glDrawSprite(numbers[0].img, 200 - 21, 310, 18, 36);
					std::stack<Number> numStack;
					int dis = endBonus;
					int tens;
					int tenCount = 1;
					for (tens = 1; dis / tens != 0; tens *= 10, tenCount++);
					tens /= 10;
					tenCount--;
					while (tens != 0) {
						numStack.push(numbers[dis / tens]);
						dis %= tens;
						tens /= 10;
						tenCount--;
					}
					int numsize = numStack.size();
					int numwidth = 0;
					for (int i = 0; i < numsize; i++) {
						Number temp = numStack.top();
						if (i % 3 == 0 && i != 0) {
							numwidth += 15;
							glDrawSprite(comma, 200 - numwidth, 310, 10, 40);
						}
						numwidth += temp.w * .8;
						glDrawSprite(temp.img, 200 - numwidth, 310, temp.w * .8, 40);
						numStack.pop();
					}
				}
				float finalScore = endScore + endDis * 1.3 + endBonus * 10;
				if ((endBonus == totalBonus && totalBonus != 0) || endScore == score) {	//DRAW FINAL SCORE
					endDisStep = finalScore / 30.0;
					if (endFinal < finalScore) {
						endFinal += endDisStep;
						if (endFinal > finalScore) endFinal = finalScore;
					}
					std::stack<Number> numStack;
					int dis = endFinal;
					int tens;
					int tenCount = 1;
					for (tens = 1; dis / tens != 0; tens *= 10, tenCount++);
					tens /= 10;
					tenCount--;
					while (tens != 0) {
						numStack.push(numbers[dis / tens]);
						dis %= tens;
						tens /= 10;
						tenCount--;
					}
					int numsize = numStack.size();
					int numwidth = 0;
					for (int i = 0; i < numsize; i++) {
						Number temp = numStack.top();
						if (i % 3 == 0 && i != 0) {
							numwidth += 15;
							glDrawSprite(comma, 400 - numwidth, 333, 10, 40);
						}
						numwidth += temp.w  * .75;
						glDrawSprite(temp.img, 400 - numwidth, 333, temp.w * .75, 40);
						numStack.pop();
					}
				}
				if (endFinal == finalScore) { //DRAW HIGH SCORES
					if (!oneApplause) {
						FMOD_System_PlaySound(fmod, applause, NULL, false, &fx);
						oneApplause = true;
					}
					int index = -1;
					for (int i = 0; i < scores2.size(); i++) {
						if (scores2[i] < finalScore) {
							index = i;
						}
					}
					for (int i = 0; i < index && ++copyOnce == 1; i++) {
						scores2[i] = scores2[i + 1];
					}
					if (index >= 0) {
						scores2[index] = finalScore;
						switch (index) {
						case 0:
							glDrawSprite(highscore, 564, 339, 199, 36);
							break;
						case 1:
							glDrawSprite(highscore, 564, 290, 199, 36);
							break;
						case 2:
							glDrawSprite(highscore, 564, 241, 199, 36);
							break;
						case 3:
							glDrawSprite(highscore, 564, 193, 199, 36);
							break;
						case 4:
							glDrawSprite(highscore, 564, 145, 199, 36);
							break;
						case 5:
							glDrawSprite(highscore, 564, 97, 199, 36);
							break;
						default:
							break;
						}
					}

					for (int a = hspos; a < scores2.size(); a++) {
						std::stack<Number> numStack;
						int dis = scores2[a];
						if (dis == 0) glDrawSprite(numbers[0].img, 755 - 21, 100 + (5 - a) * 48, 18, 36);
						int tens;
						int tenCount = 1;
						for (tens = 1; dis / tens != 0; tens *= 10, tenCount++);
						tens /= 10;
						tenCount--;
						while (tens != 0) {
							numStack.push(numbers[dis / tens]);
							dis %= tens;
							tens /= 10;
							tenCount--;
						}
						int numsize = numStack.size();
						int numwidth = 0;
						for (int i = 0; i < numsize; i++) {
							Number temp = numStack.top();
							if (i % 3 == 0 && i != 0) {
								numwidth += 15;
								glDrawSprite(comma, 755 - numwidth, 100 + (5 - a) * 48, 10, 40);
							}
							numwidth += temp.w * .8;
							glDrawSprite(temp.img, 755 - numwidth, 100 + (5 - a) * 48, temp.w * .8, 40);
							numStack.pop();
						}
					}
					hscount -= deltaTime;
					if (hscount <= 0 && hspos > 0) {
						hscount = .4;
						hspos--;
					}
					ofstream scoreFile("./highscores.txt");
					std::copy(scores2.begin(), scores2.end(), ostream_iterator<int>(scoreFile, " "));
					scoreFile << endl;
				}
			}
			//FADE OUT
			if (endFade > 0) {
				if (fader.anim.curFrame != 7) {
					fader.anim.isPlaying = true;
					animDraw(&faderp->anim, fade, 0, 0, 800, 550);
					animTick(&faderp->anim, deltaTime);
				}
				else {
					if (endFade == 2) {
						tempBG = { bg1, bg2 };
						if (!startAnim) tempBG[0].xmov = -900;
						tempBG[1].xmov = 2000;
						lvl = 0;
						jumpstack = 2;
						lvl1counter = 5;
						lvl2cd = 2;
						score = 0;
						distance = 0;
						inCount = 2;
						bonus = 0;
						swapped = false;
						pause = false;
						addFlower = 2;
						if (player.anim.def == &horseTwirlB) {
							player.x -= 10; player.y += 30; player.w -= 20; player.h -= 65;
							player.hitbox = { player.x + 74 - cam.x, player.y + 50.0 - cam.y, 74, 70 };
							animSet(&player.anim, &playerWalkB);
						}
						player.anim.def = &playerWalkB;
						player.dead = false;
						cam.x = 0;
						fader.anim.def = &fadeIn;
						fader.anim.curFrame = 0;
						gameState = GS_Play;
						backdrop[0].x = 0;
						backdrop[1].x = 0;
						endDis = 0;
						endDisStep = 0;
						endScore = 0;
						endBonus = 0;
						endFinal = 0;
						hscount = .4;
						hspos = 5;
						copyOnce = 0;
						powerbar = 0;
						animState = 0;
						shield = 0;
						invincibleStep = 2;
						invincibleCounter = 0;
						visible = true;
						player.fever = false;
						powerup = { 0, -200, -200, 0, 0 };
						powerupcounter = 20 + rand() % 20;
						puspeed = 1;
						bonusEvent = false;
						totalBonus = 0;
						tempNum.clear();
						speedupscreen = { speeed, 800, 550, 800, 0, 0 };
						speedupevent = false;
						speedupscreen.dx = 800;
						lookBack.clear();
						dieonce = false;
						oneApplause = false;
						shieldTimer = 0;
						FMOD_Channel_Stop(lvl0chan);
						FMOD_Channel_Stop(lvl1chan);
						FMOD_Channel_Stop(lvl2chan);
						FMOD_Channel_Stop(bgChan);
						FMOD_Channel_SetPaused(lvl0chan, 0);
						FMOD_Channel_SetPaused(lvl1chan, 1);
						FMOD_Channel_SetPaused(lvl2chan, 1);
						FMOD_Channel_SetPaused(bgChan, 1);
						nick1 = true;
						nick2 = true;
						what = 0;
						initxmov = 0;
						FMOD_System_PlaySound(fmod, lv0m, NULL, false, &lvl0chan);
						player = {
							{ &playerWalkB, 0, 0, true },
							40, 300, player_width, player_height,
							0, {}, 0, 0, 0, 0, 0, 0, 0, 0, 0
						};
					}
					else if (endFade == 1) {
						selectorPos = 1;
						gameState = GS_TitleScreen;
						fadeOnce = true;
						playTitleMusic = true;
					}
					animDraw(&faderp->anim, fade, 0, 0, 800, 550);
					endFade = 0;
					fader.anim.def = &fadeIn;
					fader.anim.curFrame = 0;
				}
			}
			break;
		}
		default:
			break;
		}

		/*		if ( < 1000 / 60)
				{
				//Sleep the remaining frame time
				SDL_Delay((1000 / FRAMES_PER_SECOND) - fps.get_ticks());
				}*/
		int one = currentFrameMs - lastFrameMs;
		int k = SDL_GetTicks();
		if (one < 1000 / 60){
			SDL_Delay(1000 / 60 - one);
		}
		//DRAW CURRENT STATE
		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();

	return 0;
};