DirectX11_Workshop
===================

The DirectX Workshop is a DirectX 11 graphics framework in a Visual Studio 2013 project that demonstrates a NOT BASIC DirectX application. The basic projects you find in book are bare bones, built with poor architecture, and don't respond well to changes. This project endeavors to provide a framework for prototyping graphics ideas in DirectX 11. Simply, I need a workshop to test things out, and I am making it a goal to ensure the framework is friendly to newcomers.

###Dependencies
===================

1. Visual Studio 2013 on Windows 7. Has great DirectX debugger, C++11, and C++ refactoring. All very convenient.

2. DirectX Toolkit (DXTK). Plenty of handy boilerplate functions that are great for getting something on screen without having to write some sections of boilerplate code.

3. Pempek Assert library. Great for having more robust and customizable asserts. Must have to catch those cases where the code fails as early as possible.

###Requirements
===================

0. Framework - Provide functionality to make it easy to begin working on a graphics project; boilerplate code. 
 * Zero Startup Time. Should be able to get the repository, build, and run.
 * Having a framework to begin with will help focus on course concepts, save time in bug hunting, and show a better example of a usable graphics framework than the more atomic examples in introductory DirectX books.

1. Transparency - Make the code understandable to readers, and well documented. 
 * My time is limited for this, so commits that help document outside of code comments would be handy.
 * Must be "Graphics Newcomer Friendly"

2. Robustness - The program should report when it fails, and in an appropriate manner.
 * If the DirectX device failed to construct, it should hit a breakpoint immediately and report why it failed (if possible). 
 * Do not try to handle the errors (because that squelches them), but detect them so they do not cause issues further down the pipeline.
 * Provide bug free code. Build, Compile, Run, Interact, Close without failures or crashing.

3. Flexibility - The code should be built to be changed.
 * The GraphicsSystem will be constructed to hold the major pipeline of graphics calls. Everything else around this should be modular.

4. Small - Keep it short and simple.
 * The purpose of the framework is to build complex things on top.  This is built for prototyping to that end.
 * Focus on one platform (DirectX 11, Windows 7, Visual Studio 2013)
 * Remaining small and focusing on one platform makes bug free (2) achievable.
 * This is an amalgamation of snippets of functionality. Notions to scale up will not be considered.
 * On that last point, scaling up means high performance, data-driven, and a smart pre/post processing on the collection of objects to draw. These increase the complexity of the code, and violate all other goals.

5. Up To Date - Use the latest greatest features.
 * Demonstrate how to use the newer API's.
 * Use the latest in coding practices and language features.

7. Architecturally Sound - Built for Elegance
 * This should serve as an excellent example for how projects of this size can be built. 
 * Nothing in the framework should exist without a reason.
 * The architecture should be built to the requirements here.

