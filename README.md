# CompactStar engine

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
- Artificial intelligence (early stage, see the Bot demo)
- Sound and music
- Cross-platform SDK written in C
- Objective-C Metal renderer for OSX/iOS (experimental)

<b>Here are some screenshots of several projects I realized with this engine (all are available as demo)</b>

![Screenshot](Common/Images/Screenshots/Demos.png?raw=true "Screenshot")

<b>A small prototype of 2D level editor is also provided with the engine</b>

![Screenshot](Common/Images/Screenshots/CompactStar%20Engine%202D%20Editor.png?raw=true "Screenshot")

## Supported compilers and OS

The SDK may be compiled with any C or C++ compiler (Tested with [Embarcadero RAD Studio](https://www.embarcadero.com/), [CodeBlocks](http://www.codeblocks.org/) and [xCode](https://apps.apple.com/us/app/xcode/id497799835?mt=12)). The provided demos and tools may be compiled with [Embarcadero RAD Studio](https://www.embarcadero.com/products/cbuilder/starter/free-download) or with xCode.

This game engine is also available for the [Mobile C Compiler](https://itunes.apple.com/us/app/mobile-c-c-c-compiler/id467393915?mt=8). It may be found on the following GIT: https://github.com/dztall/ucc/tree/master/3D%20engine/CompactStar%20Engine

## Third-party
This project depends on several third-party libraries, which are:
- Glew (http://glew.sourceforge.net/)
- OpenAL (https://openal.org/)
- sxml (https://github.com/capmar/sxml)

These third-party libraries depend on which OS is targetted, but are cross-platform, and are normally available on any OS. Please refer to the original website for the documentation, updates, and licenses.

Several [Blender](https://www.blender.org/) plugins were also used to export models in Quake I (.mdl), Quake II (.md2) and Quake III (.md3) formats. You may find them on the following websites:
- Quake I (.mdl): http://quakeforge.net/files.php
- Quake II (.md2): https://www.rockraidersunited.com/topic/8175-md2-blender-importexport-add-on-early-release/
- Quake III (.md3): https://github.com/neumond/blender-md3

## Assets
Several free assets were used in the demo projects. For convenience, they were left with the demo projects, as a part of them.

All the assets used in the demos were free assets downloaded from [CadNav](https://www.cadnav.com/) and [Free3D](https://free3d.com/), and are subject to the user licenses applied on their respective websites.

<b>Please don't use these assets outside the legal framework defined for them, and if you want to use them, they should be downloaded from their original website</b>, which are:
- Old farm tractor: https://www.cadnav.com/3d-models/model-44958.html
- Trees: https://free3d.com/3d-model/trees-9-53338.html

## License

I decided to share this code freely (under MIT license), don't hesitate to use it if you think it useful for your purposes. Any collaboration on this project is welcome.
