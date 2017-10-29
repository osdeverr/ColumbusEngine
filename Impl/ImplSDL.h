/************************************************
*                   ImplSDL.h                   *
*************************************************
*          This file is a part of:              *
*               COLUMBUS ENGINE                 *
*************************************************
*             Nikolay(Columbus) Red             *
*                   20.07.2017                  *
*************************************************/

#pragma once

#ifndef IMPLSDL_H
#define IMPLSDL_H

#include <cstdio>
#include <SDL.h>

#include <RenderAPI/APIOpenGL.h>

#include <Math/Vector2.h>
#include <System/Console.h>
#include <System/Timer.h>

namespace C
{

	struct C_SDLWindowConfig
	{
		bool Resizable = false;
		bool Fullscreen = false;
		int Width = 640;
		int Height = 480;
		char* Title = (char*)"Columbus Engine";
	};

	class C_SDLWindow
	{
	private:
		SDL_Window* mWindow;
		SDL_GLContext mGLC;

		bool mClosed = false;

		bool keyFocus = false;
		bool mouseFocus = false;
		bool shown = false;
		bool minimized = false;

		C_Timer mDrawTime;
		unsigned mFPSLimit = 60;
		float mTimeToDraw = 1.0 / (float)mFPSLimit;
		float mRedrawTime = 0.0;
		int mFPS = 0;

		Uint8* keys;

		bool keydown[256];
		bool keyup[256];

		SDL_Event mTmpEvent;

		//Initialize SDL
		void initSDL();
		//Initialize window
		void initWindow(C_SDLWindowConfig aConfig);
		//Initialize OpenGL
		void initOpenGL();
		//Printing API versions in console
		void getVer();
	public:
		//Constructor
		C_SDLWindow(C_SDLWindowConfig aConfig);
		//Clear input
		void SYS_CLEAR_INPUT();
		//Poll window events
		void pollEvent(SDL_Event& aEvent);
		//Clear window
		void clear(float r, float g, float b, float a);
		//Display window
		void display();
		//Return window size
		void getSize(int* aX, int* aY);
		//Return window size
		C_Vector2 getSize();
		//Return mouse position
		void getMousePos(int* aX, int* aY);
		//Return mouse position
		C_Vector2 getMousePos();
		//Return global mouse position
		void getMousePosGlobal(int* aX, int* aY);
		//Return global mouse position
		C_Vector2 getMousePosGlobal();
		//Return window aspect
		float aspect();
		//Return window open
		bool isOpen();
		//Return key-focus in window
		bool isKeyFocus();
		//return window mouse-focus
		bool isMouseFocus();
		//Return window shown
		bool isShown();
		//Return window minimized
		bool isMinimized();
		//Return SDL_Window handle
		SDL_Window* getHandle();
		//Return key-press in window
		bool getKey(int aKey);
		//Return key-down in window
		bool getKeyDown(int aKey);
		//Return key-up in window
		bool getKeyUp(int aKey);
		//Return mouse-button-press in window
		bool getMouseButton(int aButton);
		//Set vertical sync
		void setVerticalSync(bool aV);
		//Set FPS limit
		void setFPSLimit(unsigned aFPSLimit);
		//Return FPS limit
		unsigned getFPSLimit();
		//Return redraw time
		float getRedrawTime();
		//Return FPS;
		int getFPS();
		//Destructor
		~C_SDLWindow();
	};

}

#endif
