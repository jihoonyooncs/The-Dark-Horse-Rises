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

extern struct AnimFrameDef{
	int frameNum;
	float frameTime;
};

extern struct AnimDef{
	bool repeat;
	AnimFrameDef frames[20];	
	int numFrames;
};

extern struct AnimData{
	AnimDef* def;
	int curFrame;
	float timeToNextFrame;
	bool isPlaying;
};
extern struct AABB {
	float x, y, w, h;
};
extern struct Player{	//player struct
	AnimData anim;
	float x, y, w, h;
	float yvel;
	AABB hitbox;
	bool flying;
	float flyCounter; //for fly duration
	int change;		//for parabolic movement
	float counter;	//for parabolic movement
	float switcher;	//for parabolic movement
	float a;		//for parabolic movement
	float initialCounter;
	float first;
	bool magnet;
	float magCounter;
	bool dead;
	bool flyOver;
	bool fever;
	int feverCounter;
	float feverHang;
	float fevery;
};

extern struct Number{
	GLuint img;
	int w;
	int h;
	float x, y;
	float dy;
	int count;
	int count2;
	float dx;
};

extern struct Tree{
	float x, y;
};
extern struct Star{
	AnimData anim;
	float x, y, w, h;
	AABB hitbox;
	float count;
	float bx;
	float by;
	bool finishing;
	bool set;
};
extern struct StarVec{
	std::vector<std::vector<int>> map;
	int count;
	std::vector<Star> starList;
};
extern struct Cloud{
	float x, y;
	GLuint img;
	AABB hitbox;
};
extern struct Diamond{
	GLuint img;
	float x, y, w, h;
	AABB hitbox;
	int pos;
	int counter;	//parabolic movement
	int switcher;	//parabolic movement
	float bx;		//magnet
	float by;		//magnet/parabolic
	bool finishing;
	int initialCounter;
	float a; //temp x storage
	float b; //temp y storage
	float py;
	bool resetCount;
};
extern struct Object{
	int id;
	float x, y;
	float w, h;
	AnimData anim;
	AABB hitbox;
	int createDiamond;	//for mush
	bool flowerOpened; //for flower
	bool soundPlayed;
	bool soundPlayed2;
};

extern struct Camera{  //camera struct
	float x, y;
	int shape;
};
extern struct Tile{
	float leftpad, rightpad, toppad;
	AABB hitbox;
};
extern struct BGTile{
	float x, y, w, h;
	float xoffset, yoffset;
	AABB hitbox;
};
extern struct Platform {
	int id;
	float x, y;
	AABB hitbox;
};
extern struct Background{
	float xmov;
	int maxLength;
	std::vector<std::vector<int>> tileMap;
	std::vector<StarVec> starMap;
	std::vector<std::vector<std::vector<int>>> platforms;
	std::vector<Platform> platformList;
	std::vector<Tree> treeList;
	std::vector<Object> objectList;
};

extern struct Backdrop{
	GLuint img;
	float x;
	int width;
	int layer;
};
extern struct Fader{
	AnimData anim;
};

extern struct Sparkle {
	float x, y, w, h;
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

	GLfloat fade_factor;
} g_resources;

GLuint glTexImageTGAFile(const char* filename, int* outWidth, int* outHeight);
void glDrawSprite(GLuint tex, int x, int y, int w, int h);
void animTick(AnimData* data, float dt);
void animSet(AnimData* anim, AnimDef* toPlay);
void animDraw(AnimData* anim, GLuint textures[], int x, int y, int w, int h);
bool AABBIntersect(const AABB* box1, const AABB* box2);
void updateDiamond(Diamond* d, Player* player, float time);
void updateStar(Star* s, Player* p, float time, int mag);
void starDraw(Star* s, GLuint starTextures[], int x, int y);
void updateObject(Object* object, float time);
void objectDraw(Object* object, GLuint objectTextures[], int x, int y);
void updatePlayer(Player* player, float time);
void playerDraw(Player* player, GLuint playerTextures[], float x, float y);
void updateNumber(Number* n,  float t);
void speedRacer(Number* n, float t);