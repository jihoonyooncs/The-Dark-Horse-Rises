#pragma once
#include <glew.h>
#include <vector>

extern int tw;
extern int th;
extern int player_width;
extern int player_height;

enum GameState {
	GS_TitleScreen,
	GS_LoadingScreen,
	GS_Play,
	GS_End,
	GS_Tutorial,
};

struct AnimFrameDef{
	int frameNum;
	double frameTime;
};

struct AnimDef{
	bool repeat;
	AnimFrameDef frames[20];
	int numFrames;
};

struct AnimData{
	AnimDef* def;
	int curFrame;
	double timeToNextFrame;
	bool isPlaying;
};
struct AABB {
	double x, y, w, h;
};
struct Player{	//player struct
	AnimData anim;
	double x, y, w, h;
	double yvel;
	AABB hitbox;
	bool flying;
	double flyCounter; //for fly duration
	int change;		//for parabolic movement
	double counter;	//for parabolic movement
	double switcher;	//for parabolic movement
	double a;		//for parabolic movement
	double initialCounter;
	double first;
	bool magnet;
	double magCounter;
	bool dead;
	bool flyOver;
	bool fever;
	int feverCounter;
	double feverHang;
	double fevery;
};

struct Number{
	GLuint img;
	int w;
	int h;
	double x, y;
	double dy;
	int count;
	int count2;
	double dx;
};

struct Tree{
	double x, y;
};
struct Star{
	AnimData anim;
	double x, y, w, h;
	AABB hitbox;
	double count;
	double bx;
	double by;
	bool finishing;
	bool set;
};
struct StarVec{
	std::vector<std::vector<int>> map;
	int count;
	std::vector<Star> starList;
};
struct Cloud{
	double x, y;
	GLuint img;
	AABB hitbox;
};
struct Diamond{
	GLuint img;
	double x, y, w, h;
	AABB hitbox;
	int pos;
	int counter;	//parabolic movement
	int switcher;	//parabolic movement
	double bx;		//magnet
	double by;		//magnet/parabolic
	bool finishing;
	int initialCounter;
	double a; //temp x storage
	double b; //temp y storage
	double py;
	bool resetCount;
};
struct Object{
	int id;
	double x, y;
	double w, h;
	AnimData anim;
	AABB hitbox;
	int createDiamond;	//for mush
	bool flowerOpened; //for flower
	bool soundPlayed;
	bool soundPlayed2;
};

struct Camera{  //camera struct
	double x, y;
	int shape;
};
struct Tile{
	double leftpad, rightpad, toppad;
	AABB hitbox;
};
struct BGTile{
	double x, y, w, h;
	double xoffset, yoffset;
	AABB hitbox;
};
struct Platform {
	int id;
	double x, y;
	AABB hitbox;
};
struct Background{
	double xmov;
	int maxLength;
	std::vector<std::vector<int>> tileMap;
	std::vector<StarVec> starMap;
	std::vector<std::vector<std::vector<int>>> platforms;
	std::vector<Platform> platformList;
	std::vector<Tree> treeList;
	std::vector<Object> objectList;
};

struct Backdrop{
	GLuint img;
	double x;
	int width;
	int layer;
};
struct Fader{
	AnimData anim;
};

struct Sparkle {
	double x, y, w, h;
};

static struct {
	/* ... fields for buffer and texture objects */
	GLuint vertex_shader, fragment_shader, program;

	struct {
		GLint fade_factor;
		GLint textures[2];
	} uniforms;

	struct {
		GLint position;
	} attributes;

	GLdouble fade_factor;
} g_resources;

GLuint glTexImageTGAFile(const char* filename, int* outWidth, int* outHeight);
void glDrawSprite(GLuint tex, int x, int y, int w, int h);
void animTick(AnimData* data, double dt);
void animSet(AnimData* anim, AnimDef* toPlay);
void animDraw(AnimData* anim, GLuint textures[], int x, int y, int w, int h);
bool AABBIntersect(const AABB* box1, const AABB* box2);
void updateDiamond(Diamond* d, Player* player, double time);
void updateStar(Star* s, Player* p, double time, int mag);
void starDraw(Star* s, GLuint starTextures[], int x, int y);
void updateObject(Object* object, double time);
void objectDraw(Object* object, GLuint objectTextures[], int x, int y);
void updatePlayer(Player* player, double time);
void playerDraw(Player* player, GLuint playerTextures[], double x, double y);
void updateNumber(Number* n, double t);
void speedRacer(Number* n, double t);