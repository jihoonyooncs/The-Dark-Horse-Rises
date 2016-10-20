#The Dark Horse Rises
![](http://i.giphy.com/3o7TKLXP7kE2kKC1SU.gif)

###Synopsis
The Dark Horse Rises was a game made over a span of a little over a month that I made using C++, SDL, and OpenGL in Visual Studio. It was a project I was very passionate about and still is my favorite project I have worked on so far. It got me passionate about programming and about game development. I wanted to make a game that was simple yet fun while feeling like a full game instead of a demo. Here I will go through the steps I took to make the game as well as explain parts of the code and the challenges and obstacles that I ran into while coding. 

###Building a Game Engine
Building a game from scratch was initially more challenging than I thought it would be. I wasn't sure how to make hard boundaries using coordinates, add a moving background with parallax, or even move the character around with the arrow keys in a smooth way. I started with the basics. First, make a platform where the player can walk on. Next, add multiple tiles so he can walk accross the map. Next, figure out a way to implement gravity. Then after that we need collision, collision detection for jumping, moving, staying still, etc. After that animation becomes important. You need different animations for every move, and it must react along with the position of the character with every input. The character should be able to traverse any terrain that the tile-based map can throw at it, whether it's collision from the bottom, side, or top. Hitboxes need to be implemented for different scenarios and have to match with the animation frame. Finally, different kinds of physics have to be implemented: jumping off of things, falling, getting hit, double jumping, triple jumping, gliding, etc. These were the basics of the game engine, but even they brought many unforseen and frustrating challenges.

###The Game and its Implementation
I will try to concisely share the key functions of the game and ![title.GIF]({{site.baseurl}}/title.GIF)


###Challenges
One thing that this game taught me about programming is that nothing really goes according to plan. I would say oh, that seems easy to implement, and it would end up taking me 3 days of coding and debugging. 

###Notes and future changes
Because of the way I created the executable file for this project at the time, you unfortunately need Visual Studio 2013 to run the game. This is something I will be coming back to and changing in the future.