<b><u>CompactStar engine</u></b>

The CompactStar engine is a tiny game engine I'm using for my own purposes. With the existence of very performant and low cost, sometimes even free, 3D engines like Unity, Unreal or Panda, I know that the reasons that motivated me to write my own 3D game engine may appear somewhat strange, especially when the result is less efficient than these engines.

However I had several reasons to do that:
1. I wanted to learn something, and to confront myself to the difficulty to create a such engine.
2. I wanted to have a game engine as tiny as possible, with just the functionalities I needed.
3. I wanted to remain as independent as possible when creating several of my projects.

It's the reason why this engine exists.

<b>Here are the actual supported features</b>
- Simple shapes creation (surface, box, sphere, cylinder, disk, ring and spiral)
- Quake I (.mdl), DirectX (.x, partially), and WaveFront (.obj, partially) models
- Animations for Quake I (.mdl) and DirectX (.x) models
- Skybox
- Landscape generation
- Transparency
- Bump mapping
- Full-scene antialiasing and post-processing effects
- Collision detection (partially, ground and mouse collision)
- Particles system (early stage, see the Weather demo and Spaceship game demo)
- Physics (early stage, see the Wild Soccer demo)
- Sound and music
- Cross-platform SDK written in C
- Objective-C Metal renderer for OSX/iOS (experimental)

<b>Here are some screenshots of several projects I realized with this engine (all are available as demo)</b>

![Screenshot](Common/Images/Screenshots/Demos.png?raw=true "Screenshot")

<b>A small prototype of 2D level editor is also provided with the engine</b>

![Screenshot](Common/Images/Screenshots/CompactStar%20Engine%202D%20Editor.png?raw=true "Screenshot")

<b>Supported compilers and OS</b>

The SDK may be compiled with any C or C++ compiler (Tested with Embarcadero RAD Studio, CodeBlocks and xCode). The provided demos and tools may be compiled with Embarcadero RAD Studio (a free version is available here: https://www.embarcadero.com/products/cbuilder/starter/free-download) or with xCode.

This game engine is also available for the Mobile C Compiler (https://itunes.apple.com/us/app/mobile-c-c-c-compiler/id467393915?mt=8). It may be found on the following GIT: https://github.com/dztall/ucc/tree/master/3D%20engine/CompactStar%20Engine

The third-party libraries depend on which OS is targetted, but are cross-platform, and are normally available on any OS.

<b>License</b>

I decided to share this code freely, don't hesitate to use it if you think it useful for your purposes. Any collaboration on this project is welcome.
