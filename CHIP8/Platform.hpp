//
// Created by sulaiman on 2024-05-25.
//
#include <bits/stdc++.h>
#include <SDL2/sdl.h>

class Platform{
public:
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth,int textureHeight){
        //Initialize SDL library with video subsystem
        SDL_Init(SDL_INIT_VIDEO);

        //Title of window
        //X Position of window
        //Y position of window
        //width of window
        //height of window
        //Flags (Ignored)
        window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

        //Window where rendering is displayed
        //Index of rendering driver to initialize (-1 = the first one supporting the requested flags)
        //Flags - accelerated rendering

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        //Rendering context
        //Pixel Format
        //access pattern allowed for a texture - Changes frequently, lockable
        //width of texture in pixels
        //height of texture in pixels
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);

    }

    //Destructor
    ~Platform() {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Update(void const* buffer, int pitch){
        //texture to update
        //Area to update - the entire texture
        //Raw pixel data in format of a texture
        //Number of bytes in a row of pixel data
        SDL_UpdateTexture(texture, nullptr, buffer, pitch);

        //Clear current rendering target with drawing colour
        SDL_RenderClear(renderer);

        //Rendering context
        //Source texture
        //Use the entire texture
        //Use the entire rendering target
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);

        //Update the screen with any rendering performed since previous call
        SDL_RenderPresent(renderer);
    }

    bool ProcessInput(uint8_t* keys){
        bool quit = false;
        SDL_Event event;

        while (SDL_PollEvent(&event)){
            switch(event.type)
            {
                case SDL_QUIT:{
                    quit = true;
                } break;
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                        {
                            quit = true;
                        }break;

                        case SDLK_x:
                        {
                            keys[0] = 1;
                        } break;

                        case SDLK_1:
                        {
                            keys[1] = 1;
                        } break;

                        case SDLK_2:
                        {
                            keys[2] = 1;
                        } break;

                        case SDLK_3:
                        {
                            keys[3] = 1;
                        } break;

                        case SDLK_q:
                        {
                            keys[4] = 1;
                        } break;

                        case SDLK_w:
                        {
                            keys[5] = 1;
                        } break;

                        case SDLK_e:
                        {
                            keys[6] = 1;
                        } break;

                        case SDLK_a:
                        {
                            keys[7] = 1;
                        } break;

                        case SDLK_s:
                        {
                            keys[8] = 1;
                        } break;

                        case SDLK_d:
                        {
                            keys[9] = 1;
                        } break;

                        case SDLK_z:
                        {
                            keys[0xA] = 1;
                        } break;

                        case SDLK_c:
                        {
                            keys[0xB] = 1;
                        } break;

                        case SDLK_4:
                        {
                            keys[0xC] = 1;
                        } break;

                        case SDLK_r:
                        {
                            keys[0xD] = 1;
                        } break;

                        case SDLK_f:
                        {
                            keys[0xE] = 1;
                        } break;

                        case SDLK_v:
                        {
                            keys[0xF] = 1;
                        } break;
                    }
                }break;

                case SDL_KEYUP:
                {
                    switch (event.key.keysym.sym){
                        case SDLK_x:
                        {
                            keys[0] = 0;
                        } break;

                        case SDLK_1:
                        {
                            keys[1] = 0;
                        } break;

                        case SDLK_2:
                        {
                            keys[2] = 0;
                        } break;

                        case SDLK_3:
                        {
                            keys[3] = 0;
                        } break;

                        case SDLK_q:
                        {
                            keys[4] = 0;
                        } break;

                        case SDLK_w:
                        {
                            keys[5] = 0;
                        } break;

                        case SDLK_e:
                        {
                            keys[6] = 0;
                        } break;

                        case SDLK_a:
                        {
                            keys[7] = 0;
                        } break;

                        case SDLK_s:
                        {
                            keys[8] = 0;
                        } break;

                        case SDLK_d:
                        {
                            keys[9] = 0;
                        } break;

                        case SDLK_z:
                        {
                            keys[0xA] = 0;
                        } break;

                        case SDLK_c:
                        {
                            keys[0xB] = 0;
                        } break;

                        case SDLK_4:
                        {
                            keys[0xC] = 0;
                        } break;

                        case SDLK_r:
                        {
                            keys[0xD] = 0;
                        } break;

                        case SDLK_f:
                        {
                            keys[0xE] = 0;
                        } break;

                        case SDLK_v:
                        {
                            keys[0xF] = 0;
                        } break;
                    }
                }break;
            }
        }
        return quit;
    }

private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};

};