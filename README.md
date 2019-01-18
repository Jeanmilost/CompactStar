<b><u>CompactStar engine</u></b>

The CompactStar engine is a tiny game engine I'm using for my own purposes. With the existence of very performant and low cost, sometimes even free, 3d engine like Unity, Unreal or Panda, I know that the reasons that motivated me to write my own 3d game engine may appear somewhat strange, especially when the result is really less efficient than these engines.

However I had several reasons to do that:
1. I wanted a tiny code that I could use anywhere without difficulties. So I can use this engine to create Windows application projects, iPhone games with xCode on Mac OS X, and for the anecdote, I can even use the SDK with a small c compiler on my cell phone (the Mobile C Compiler, available here: https://itunes.apple.com/us/app/mobile-c-c-c-compiler/id467393915?mt=8), which is very useful for me.
2. I'm an enthousiast game writter, but I'm working alone. And as I'm not a graphist, use a high tech engine to create a game which will just contain very basic graphics seemed to me the same thing than get out the Ferrari to go to supermarket.
3. I wanted to learn something, and to confront myself to the difficulty to create a such engine.

It's the reason why this engine exists.

![Screenshot](Common/Images/Screenshots/CompactStar%20Engine%202D%20Editor.png?raw=true "Screenshot")

<b>Here are the actual supported features</b>
- Simple shapes creation (surface, box, sphere, cylinder, disk, ring and spiral)
- Quake I (.mdl), DirectX (.x, partially) and WaveFront (.obj, partially) loader
- Skybox
- Landscape generation
- Transparency
- Bump mapping
- Full-scene antialiasing and post-processing effects
- Collision detection (partially, ground and mouse collision)
- Cross-platform SDK (Windows and Mac OS X, not tested on Linux) written in C
- Objective-C Metal renderer for OSX/iOS (experimental)
- A small 2D level editor I'm using to create a game (can be compiled with Embarcadero RAD Studio, available here: https://www.embarcadero.com/products/cbuilder/starter/free-download)

I decided to share this code freely, don't hesitate to use it if you think it useful for your purposes. Any collaboration on this project is welcome.
