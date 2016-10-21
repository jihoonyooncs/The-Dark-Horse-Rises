#The Dark Horse Rises
![](http://i.giphy.com/3o7TKLXP7kE2kKC1SU.gif)

##Synopsis
The Dark Horse Rises was a game made over a span of a little over a month that I made using C++, SDL, and OpenGL in Visual Studio. It was a project I was very passionate about and still is my favorite project I have worked on so far. It got me passionate about programming and about game development. I wanted to make a game that was simple yet fun while feeling like a full game instead of a demo. Here I will go through the steps I took to make the game as well as explain parts of the code and the challenges and obstacles that I ran into while coding. 

##Building a Game Engine
Building a game from scratch was initially more challenging than I thought it would be. I wasn't sure how to make hard boundaries using coordinates, add a moving background with parallax, or even move the character around with the arrow keys in a smooth way. I started with the basics. First, make a platform where the player can walk on. Next, add multiple tiles so he can walk accross the map. Next, figure out a way to implement gravity. Then after that we need collision, collision detection for jumping, moving, staying still, etc. After that animation becomes important. You need different animations for every move, and it must react along with the position of the character with every input. The character should be able to traverse any terrain that the tile-based map can throw at it, whether it's collision from the bottom, side, or top. Hitboxes need to be implemented for different scenarios and have to match with the animation frame. Finally, different kinds of physics have to be implemented: jumping off of things, falling, getting hit, double jumping, triple jumping, gliding, etc. These were the basics of the game engine, but even they brought many unforseen and frustrating challenges.

##The Game, its Implementation, and the Challenges.
One thing that this game taught me about programming is that nothing really goes according to plan. I would say oh, that seems easy to implement, and it would end up taking me 3 days of coding and debugging. Here I'll try to concisely describe some key features and how I implemented them along with the challenges I ran into along the way.

###Animation
The animation is handled by 3 structs (AnimFrameDef, AnimDef, and AnimData) and 3 functions (animTick, animSet, and animDraw). The structs define the necessary variables. AnimFrameDef holds the number of the frame and how long it stays on the screen. AnimDef defines an AnimFrameDef and also how many frames there are total in the animation. AnimData defines a pointer to AnimDef and defines the current frame, time to the next frame, and a boolean to set the animation to run or not. The functions run the animation, with animTick using the data to go to the next frame if conditions are met, animSet being used to set an animation to a certain state and frame, and animDraw to draw the character on the screen.
<img src="struct.JPG" width="200">
<img src="functions.JPG" width="480">
One challenge was playing the right animation when the horse (main character) changes states. In the game, the horse transforms from a normal horse into Pegasus. During this transformation I ran into various bugs in the code with the animation not working properly. 


###Physics/Collision
Physics and collision gave me the most bugs and challenges of the entire project. Even simple things created some strange bugs that took me several days to fix. At the beginning, a problem was the character not going past one block horizontally, because I had set the condition to 'when you touch a block, stop moving' to enforce gravity. To fix this, I moved the checking of whether the character was touching the ground to the physics loop, simply un-applying gravity when the horse touched the ground. Another small, related challenge was that when I did that, the character would be stuck under the platform if it was going fast enough downwards. I fixed this by moving it back up the amount it needed in order to be on the top most tile. Then, the horse could be seen going under the platform for a split second before jumping back up. To fix this, I moved the drawing functions after the physics loop. Other physics related challenges were hitting a tile at an angle while coming down from a jump, jittering (small, rapid up and down movements while standing on a platform), only detecting collision for the top tile and no other tile, colliding into multiple enemies at the same time, and many more. There were many other small, hard to fix bugs that I ran into with physics, and I eventually fixed them through debugging, memory breakpoints, print statements, or creating smaller simulations of parts of the code in another project. 

###Endless platforms
I was initially stumped on how to make endless platforms that constantly kept generating and how I would manage all those maps at once. 

###Stars
One challenge was adding stars in the special stage. The special stage is an all the stars you can get type of level where there are no enemies and you just jump around and get all the stars you can get. However, because the level was so large, placing the stars one by one through coordinates would have taken much too long. After some struggle, I decded to implement a system where I could go into the game stage, click on where I wanted the stars, have temporary stars show up to show me what they would look like, and have all the coordinates be written to a text file and read into the code as a list of an object I called StarVec. These hold stars that can be picked up when touched and add to your gold count. This made it much faster to organize the stars and put in any pattern I wanted. I also added an undo functionality to speed up the process (I wouldn't have to restart when I made a mistake). 

###Platforms 
I used the same method as the stars to later on make platforms. These platforms didn't conform to the tile spaces (each map is a 2d Array), which created some problems, but with this method, I could place the platform anywhere I wanted and at any height relative to each other, so I could create a gentler or steeper slope. Because my mouse wasn't too accurate, I also made it so that when I was building these platforms, each tile would snap to a nearby tile horizontally if it was close enough, and you could make fine adjustments with your mouse. This, like the stars, allowed me to make many interesting platforms above the tile map that led to some interesting map layouts.

###Diamonds
The diamonds pop out of the mushrooms when the character jumps on them. They go to the right side of the screen and then slowly move up and down to the left. At first I thought I could just set upper and lower bounds and move it left, but the movement was incredibly robotic. I fixed this by creating a function that would move the diamonds in a parabolic motion to the left so that the motion would be much more fluid. I also ran other animations through this function to make the movement much more fluid: the powerups, coins going up to the counter when they've been picked up, the "speed up" sign coming down from the right side of the screen and then pausing in mid screen and then going off to the left, and the trajectory of stars and diamonds as they are sucked into the character when he has the magnet powerup.


###Others (Organization of code, optimization, framerates, etc)


##Notes and future changes
Because of the way I created the executable file for this project at the time, you unfortunately need Visual Studio 2013 to run the game. This is something I will be coming back to and changing in the future.

*I'm currently in the process of writing this page