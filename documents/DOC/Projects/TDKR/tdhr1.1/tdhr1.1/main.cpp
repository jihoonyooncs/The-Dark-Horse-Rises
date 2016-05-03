#include <SDL.h>
#include <glew.h>
#include <iostream>

class GameEngine{
private:
	//The window we will show our squares in
	SDL_Window* win = NULL;
	//The renderer we will render our rects to
	SDL_Renderer* rend = NULL;
	//This holds the events so we can exit the app
	SDL_Event e;

	//test for pseudo-infinite game loop
	bool isRunning = true;

	//The amount of times positions are updated per second (FPS)
	//Lower this to more easily see our red square interpolating
	const int updatesPerSecond = 60;
	//The velocity in pixels per second of our rects to show them animating
	//increase this to really see the difference between none interpolated animation
	const float speed = 400.0;

	//This is the actual amount of time in milliseconds an update takes
	const int singleFrameTimeInMS = 1000 / updatesPerSecond;
	//The max amount of times we can update position before we draw
	const int maxFrameSkip = 5;
	//The number of frame skips that actually happened
	int loops = 0;

	//The time of the next frame. Time of now + singleFrameTimeInMS = nextFrameTime
	Uint32 nextFrameTime = 0;
	//The time of the frame we just showed
	Uint32 prevFrameTime = 0;

	//An array that holds the position of our red rectangle.
	float prevPos[2] = { 0.0, 50.0 };
	//our red rectangle
	SDL_Rect redRectangle = { int(prevPos[0]), int(prevPos[1]), 50, 50 };
	//our yellow rectangle
	SDL_Rect yellowRectangle = { 0, 0, 50, 50 };
	//Time of now - prevFrameTime / 1000 = a way to allow us to specify
	//the velocity of our objects in pixels per second
	float deltaTime = 0;
	//The percentage of time we currently are between frames
	float interpolation = 0;
	//The previous interpolation so we can check against it 
	int prevIntpol = -1;

	//A timer to output number of times we have drawn in one second
	Uint32 timerNow = 0;
	//The number of times we have drawn
	Uint32 drawCount = 0;
public:
	GameEngine();
	~GameEngine();

	//This is the main pseudo-infinite game loop
	void mainLoop();
	//This is where we check for inputs
	void inputs();
	//This is where we update positions
	//This loop separates the updates from the drawing
	//so no matter how slow we draw our positions will be correct
	void updatePositions();
	//This is where we update positions based upon the current time
	//between frames
	void interpolate();
	//This is where we feed everything to the renderer
	void draw();
};

//Default constructor that holds the standard SDL2 initializations
GameEngine::GameEngine()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "could not initialize SDL2 \n" << SDL_GetError() << "\n";
		isRunning = false;
	}
	win = SDL_CreateWindow("Animation Interpolation",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		600,
		SDL_WINDOW_SHOWN);
	if (win == NULL)
	{
		std::cout << "Could not create Window\n" << SDL_GetError() << "\n";
		isRunning = false;
	}
	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (rend == NULL)
	{
		std::cout << "Could not create Renderer\n" << SDL_GetError() << "\n";
		isRunning = false;
	}
	nextFrameTime = SDL_GetTicks();
	//Let's get ready for some main loop action!
	this->mainLoop();
}

//The Default destructor with the standard SDL2 quitting instructions
GameEngine::~GameEngine()
{
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

//Our main pseudo-infinite game loop
void GameEngine::mainLoop()
{
	//To show what is happening in the terminal
	std::cout << "Frames Drawn\tDelta\t\tInterpolation\n";
	//Start the loop while isRunning is true
	while (isRunning)
	{
		//this is a timer that shows the number of times our interpolated 
		//objects have been drawn every second
		if (SDL_GetTicks() > timerNow + 1000)
		{
			std::cout << drawCount << "\t\t" << deltaTime << "\t\t" << interpolation << "\n";
			drawCount = 0;
			timerNow = SDL_GetTicks();
		}
		//This function holds our input handling
		//We can also put this above the updatePositions() call
		//But that slows down the inputs.
		this->inputs();

		//loops is the number of time we have skipped frames
		loops = 0;
		// a way to allow us to specify the velocity of our 
		//objects in pixels per second. This keeps the animation distance
		//constant when the frame rate changes
		deltaTime = float(SDL_GetTicks() - prevFrameTime) / 1000.0f;

		//This loop determines when to update the position
		//This is how we separate the drawing and the position updating
		while (SDL_GetTicks() > nextFrameTime && loops < maxFrameSkip)
		{
			//Finally update the position of our objects
			this->updatePositions();

			//nextFrameTime is the time in MS we need to pass to update
			nextFrameTime += singleFrameTimeInMS;
			//This is the time right now
			prevFrameTime = SDL_GetTicks();
			//If we are stuck here because the frame rate slows we need to
			//break out if loops is > maxFrameSkip
			loops++;
		}
		//This is basically the percentage between frames we currently are
		interpolation = float(SDL_GetTicks() + singleFrameTimeInMS - nextFrameTime)
			/ float(singleFrameTimeInMS);

		//convert that percentage to an integer to make it easy to test
		int ip = int(interpolation * 10);


		//Finally we can draw if the following tests have passed
		//Check against previous interpolation so we don't render
		//the same thing more than once
		//Play around with these to find an ideal interpolation

		//draws on 0% and 50%
		if ((ip == 5 || ip == 0) && ip != prevIntpol)

			//draws on 0%, 30%, 60%, 90%
			// if ( (ip == 0 || ip == 3 || ip == 6 || ip == 9) && 
			//   ip != prevIntpol )

			//draw as fast as our hardware can
			// if (true)
		{
			//finally do the interpolation calculation
			this->interpolate();
			//make the interpolation current
			prevIntpol = ip;
			//Aaaaaand draw!
			this->draw();
		}

	}
}

//Our input handling function
void GameEngine::inputs()
{
	while (SDL_PollEvent(&e))
	{
		//If an exit keypress combo has been pressed 
		//or the window X has been clicked
		if (e.type == SDL_QUIT)
		{
			//This will exit the pseudo-infinite game loop
			//Causing the destructor to do its magic
			isRunning = false;
		}
		else if (e.type == SDL_WINDOWEVENT)
		{
			if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
			{
				SDL_Event f;
				f.type = SDL_WINDOWEVENT;
				f.window.event = SDL_WINDOWEVENT_FOCUS_GAINED;
				SDL_WaitEvent(&f);
			}
		}
	}
}

//Do the main updating of object positions
void GameEngine::updatePositions()
{
	//Update our red rectangle based on its previous position
	//current velocity and shim it with deltaTime
	redRectangle.x = prevPos[0] + speed * deltaTime;
	redRectangle.y = prevPos[1] + speed * deltaTime;

	//Do a basic animation on our yellow rectangle
	yellowRectangle.x += speed * deltaTime;
	yellowRectangle.y += speed * deltaTime;

	//These two if statements just make the squares
	//go back to the beginning
	if (redRectangle.y > 550)
	{
		redRectangle.x = 0;
		redRectangle.y = 50;

	}
	if (yellowRectangle.y > 500)
	{
		yellowRectangle.x = 0;
		yellowRectangle.y = 0;
	}

	//Now we update the prevPos of our red rectangle
	//to test against on the next frame
	prevPos[0] = redRectangle.x;
	prevPos[1] = redRectangle.y;
}

//The interpolating function
void GameEngine::interpolate()
{
	//Basically prevPos will never change during the frame
	//But we need to update it's position based upon the
	//percentage of time we currently are between frames
	//The actual position of the red square never changes
	//We just show it in different positions between frames
	redRectangle.x = prevPos[0] + (speed*deltaTime)*interpolation;
	redRectangle.y = prevPos[1] + (speed*deltaTime)*interpolation;
}

//Finally the drawing function
void GameEngine::draw()
{
	//Set our background to black
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
	//clear the screen
	SDL_RenderClear(rend);

	//Set our color to red
	SDL_SetRenderDrawColor(rend, 255, 0, 0, 0);
	//paint the red recatngle
	SDL_RenderFillRect(rend, &redRectangle);
	//set the color to yellow
	SDL_SetRenderDrawColor(rend, 255, 255, 0, 0);
	//paint the yellow rectangle
	SDL_RenderFillRect(rend, &yellowRectangle);

	//Now show the renderer
	SDL_RenderPresent(rend);

	//Remember we our counting how many times
	//we have drawn our objects
	drawCount++;
}

//Don't you love simple main functions
int main()
{
	//Create our GameEngine object
	GameEngine test;
	return 0;
}