#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#include <glew.h>
#define SMOOTHSTEP(x) ((x) * (x) * (3 - 2 * (x)))
#include <stdio.h>
#include <stdlib.h>
#include "structandfunctions.h"


// Load a file into an OpenGL texture, and return that texture.
GLuint glTexImageTGAFile(const char* filename, int* outWidth, int* outHeight){
	const int BPP = 4;

	// open the file 
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		fprintf(stderr, "File: %s -- Could not open for reading.\n", filename);
		return 0;
	}

	// skip first two bytes of data we don't need
	fseek(file, 2, SEEK_CUR);

	// read in the image type.  For our purposes the image type should
	// be either a 2 or a 3.
	unsigned char imageTypeCode;
	fread(&imageTypeCode, 1, 1, file);
	if (imageTypeCode != 2 && imageTypeCode != 3) {
		fclose(file);
		fprintf(stderr, "File: %s -- Unsupported TGA type: %d\n", filename, imageTypeCode);
		return 0;
	}

	// skip 9 bytes of data we don't need
	fseek(file, 9, SEEK_CUR);

	// read image dimensions
	int imageWidth = 0;
	int imageHeight = 0;
	int bitCount = 0;
	fread(&imageWidth, sizeof(short), 1, file);
	fread(&imageHeight, sizeof(short), 1, file);
	fread(&bitCount, sizeof(unsigned char), 1, file);
	fseek(file, 1, SEEK_CUR);

	// allocate memory for image data and read it in
	unsigned char* bytes = (unsigned char*)calloc(imageWidth * imageHeight * BPP, 1);

	// read in data 
	if (bitCount == 32) {
		int it;
		for (it = 0; it != imageWidth * imageHeight; ++it) {
			bytes[it * BPP + 0] = fgetc(file);
			bytes[it * BPP + 1] = fgetc(file);
			bytes[it * BPP + 2] = fgetc(file);
			bytes[it * BPP + 3] = fgetc(file);
		}
	}
	else {
		int it;
		for (it = 0; it != imageWidth * imageHeight; ++it) {
			bytes[it * BPP + 0] = fgetc(file);
			bytes[it * BPP + 1] = fgetc(file);
			bytes[it * BPP + 2] = fgetc(file);
			bytes[it * BPP + 3] = 255;
		}
	}

	fclose(file);

	// load into OpenGL
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, bytes);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	free(bytes);

	if (outWidth) {
		*outWidth = imageWidth;
	}
	if (outHeight) {
		*outHeight = imageHeight;
	}
	return tex;
}

// Draw the sprite
void glDrawSprite(GLuint tex, int x, int y, int w, int h){
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
	{
		glColor3ub(255, 255, 255);
		glTexCoord2f(0, 1);
		glVertex2i(x, y);
		glTexCoord2f(1, 1);
		glVertex2i(x + w, y);
		glTexCoord2f(1, 0);
		glVertex2i(x + w, y + h);
		glTexCoord2f(0, 0);
		glVertex2i(x, y + h);
	}
	glEnd();
}

void animTick(AnimData* data, double dt){
	if (!data->isPlaying) return;
	int numFrames = data->def->numFrames;
	data->timeToNextFrame -= dt;
	if (data->timeToNextFrame < 0){
		++data->curFrame;
		if (data->curFrame >= numFrames){
			data->curFrame = numFrames - 1;
			data->timeToNextFrame = 0;
			data->isPlaying = false;
		}
		else{
			AnimFrameDef* curFrame = &data->def->frames[data->curFrame];
			data->timeToNextFrame += curFrame->frameTime;
		}
	}
}

void animSet(AnimData* anim, AnimDef* toPlay){
	anim->def = toPlay;
	anim->curFrame = 0;
	anim->timeToNextFrame = toPlay->frames[0].frameTime;
	anim->isPlaying = true;
}

void animDraw(AnimData* anim, GLuint textures[], int x, int y, int w, int h){
	int curFrameNum = anim->def->frames[anim->curFrame].frameNum;
	GLuint tex = textures[curFrameNum];
	glDrawSprite(tex, x, y, w, h);
}

bool AABBIntersect(const AABB* box1, const AABB* box2)
{
	if (box1->x > box2->x + box2->w) return false;	// box1 to the right	
	if (box1->x + box1->w < box2->x) return false;	// box1 to the left
	if (box1->y > box2->y + box2->h) return false;	// box1 below
	if (box1->y + box1->h < box2->y) return false;	// box1 above
	return true;
}

void updateDiamond(Diamond* d, Player* player, double time){
	if (d->initialCounter < 50) {
		double ax = 700;
		double bx = d->a;
		double v = d->initialCounter / 50.0;
		v = 1 - (1 - v) * (1 - v);
		double ay = d->py;
		double by = d->b;
		d->x = (ax * v) + (bx * (1 - v));
		d->y = (ay * v) + (by * (1 - v));
		d->initialCounter++;
		d->bx = d->x;
		d->by = d->y;
	}
	else if (!player->flying && !player->magnet && !d->finishing){
		d->x -= 200 * time;
		if (d->switcher == 1){
			double a = d->by;
			double b = d->by + 150;
			if (d->counter < 60){
				double v = d->counter / 60.0;
				v = SMOOTHSTEP(v);
				d->y = (a * v) + (b * (1 - v));
				d->counter++;
			}
			else{
				d->switcher = 0;
				d->counter = 0;
			}
		}
		else{
			double a = d->by + 150;
			double b = d->by;
			if (d->counter < 60){
				double v = d->counter / 60.0;
				v = SMOOTHSTEP(v);
				d->y = (a * v) + (b * (1 - v));
				d->counter++;
			}
			else{
				d->switcher = 1;
				d->counter = 0;
			}
		}
	}
	else if (player->flying || player->magnet || d->finishing) {
		if (!d->resetCount) {
			d->counter = 1;
			d->resetCount = true;
			d->bx = d->x;
			d->by = d->y;
		}
		d->finishing = true;
		double ax = player->x + 85.0;
		double bx = d->bx;
		double v = d->counter / 30.0;
		v = v * v;
		double ay = player->y + 60;
		double by = d->by;
		d->x = (ax * v) + (bx * (1 - v));
		d->y = (ay * v) + (by * (1 - v));
		d->counter++;
	}
};

void updateStar(Star* s, Player* p, double time, int mag){
	animTick(&s->anim, time);
	if (s->finishing || mag == 1 && (s->count < 30 && p->flying || p->magnet)) {
		s->finishing = true;
		double ax = p->x + 85;
		double bx = s->bx;
		double v = s->count / 30.0;
		v = 1 - (1 - v) * (1 - v);
		double ay = p->y + 58;
		double by = s->by;
		s->x = (ax * v) + (bx * (1 - v));
		s->y = (ay * v) + (by * (1 - v));
		s->count++;
	}
}

void updateObject(Object* object, double time){  //UPDATE object
	animTick(&object->anim, time);
}

void objectDraw(Object* object, GLuint objectTextures[], int x, int y){			//DRAW object
	animDraw(&object->anim, objectTextures, x, y, object->w, object->h);
	if (!object->anim.isPlaying && object->anim.def->repeat) animSet(&object->anim, object->anim.def);
}

void starDraw(Star* s, GLuint starTextures[], int x, int y){			//DRAW object
	animDraw(&s->anim, starTextures, x, y, s->w, s->h);
	if (!s->anim.isPlaying && s->anim.def->repeat) animSet(&s->anim, s->anim.def);
}

void updatePlayer(Player* player, double time){		//UPDATE PLAYER	
	animTick(&player->anim, time);

	if (player->fever) {
		if (player->feverCounter < 50) {
			double a = 0;
			double b = player->fevery;
			double v = player->feverCounter / 50.0;
			v = 1 - (1 - v) * (1 - v);
			player->y = (a * v) + (b * (1 - v));
			player->feverCounter++;
			player->yvel = 0;
		}
	}

	if (player->flying){
		if (player->initialCounter < 15){
			double a = 0.0;
			double b = player->a;
			double v = player->initialCounter / 18.0;
			player->y = (a * v) + (b * (1 - v));
			player->initialCounter++;
			player->a = player->y;
		}
		else{
			if (player->switcher == 1){
				double a = 0.0;
				double b = 50.0;
				if (player->counter < 15){
					double v = player->counter / 15.0;
					v = SMOOTHSTEP(v);
					player->y = (a * v) + (b * (1 - v));
					player->counter++;
				}
				else{
					double v = player->counter / 15.0;
					v = SMOOTHSTEP(v);
					player->y = (a * v) + (b * (1 - v));
					player->switcher = 0;
					player->counter = 0;
				}
			}
			else{
				double a = 50.0;
				double b = 0.0;
				if (player->counter < 15){
					double v = player->counter / 15.0;
					v = SMOOTHSTEP(v);
					player->y = (a * v) + (b * (1 - v));
					player->counter++;
				}
				else{
					double v = player->counter / 15.0;
					v = SMOOTHSTEP(v);
					player->y = (a * v) + (b * (1 - v));
					player->switcher = 1;
					player->counter = 0;
				}
			}
		}
		player->flyCounter -= time;
		if (player->flyCounter <= 0) {
			player->initialCounter = 0;
			player->yvel = 0;
			player->flying = false;
			player->flyOver = true;
		}
	}
	if (player->magCounter > 0){
		player->magCounter -= time;
	}
	else player->magnet = false;
}

void playerDraw(Player* player, GLuint playerTextures[], double x, double y){		//DRAW PLAYER
	animDraw(&player->anim, playerTextures, x, y, player->w, player->h);
	if (!player->anim.isPlaying && player->anim.def->repeat) animSet(&player->anim, player->anim.def);
}

void updateNumber(Number* n, double t) {
	if (n->count < 30) {
		double a = n->dy + 100;
		double b = n->dy;
		double v = n->count / 30.0;
		v = 1 - (1 - v) * (1 - v);
		n->y = (a * v) + (b * (1 - v));
		n->count++;
	}
	else if (n->count2 < 30) {
		double a = n->dy;
		double b = n->dy + 100;
		double v = n->count2 / 30.0;
		v = v * v;
		n->y = (a * v) + (b * (1 - v));
		n->count2++;
	}
}

void speedRacer(Number* n, double t) {
	if (n->count < 50) {
		double a = 0;
		double b = n->dx;
		double v = n->count / 50.0;
		v = 1 - (1 - v) * (1 - v);
		n->x = (a * v) + (b * (1 - v));
		n->count++;
	}
	else if (n->count2 < 50) {
		double a = -800;
		double b = 0;
		double v = n->count2 / 50.0;
		v = v * v;
		n->x = (a * v) + (b * (1 - v));
		n->count2++;
	}
}