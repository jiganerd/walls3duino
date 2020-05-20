//
//  Graphics.cpp
//  walls3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include <assert.h>
#include <algorithm>
#include "Graphics.hpp"

Graphics::SDLException::SDLException(std::string msg):
    error(SDL_GetError()),
    msg(msg)
{
}

std::string Graphics::SDLException::GetMsg() const
{
    return "SDLException: " + msg + ": " + error;
}

Graphics::Graphics() :
    pPixelBuf{new uint8_t[ScreenWidth * ScreenHeight / 8]},
    pSimScreenPixelBuf{new uint32_t[SimScreenWidth * SimScreenHeight]}
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw SDLException("Error initializating SDL");
    
    SDL_CreateWindowAndRenderer(SimScreenWidth, SimScreenHeight, SDL_WINDOW_SHOWN, &pWindow, &pRenderer);
    if (pWindow == NULL)
        throw SDLException("Window could not be created");

    pScreenTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SimScreenWidth, SimScreenHeight);
    if (pScreenTexture == NULL)
        throw SDLException("Could not create screen texture");
    
    SDL_RaiseWindow(pWindow);
}

Graphics::~Graphics()
{
    SDL_DestroyTexture(pScreenTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}

void Graphics::BeginFrame()
{
    memset(pPixelBuf.get(), 0, ScreenWidth * ScreenHeight / 8);
}

void Graphics::EndFrame()
{
    // adapt vertical 1-bit buffer to SDL buffer
    for (uint32_t j = 0; j < ScreenHeightChunks; j++)
    {
        for (uint32_t x = 0; x < ScreenWidth; x++)
        {
            uint8_t chunk = pPixelBuf[j * ScreenWidth + x];
            
            for (uint8_t bit = 0; bit < 8; bit++)
            {
                uint32_t pixel = (((chunk >> bit) & 0x01) ? 0x00FFFFFF : 0x00000000);
                uint8_t y = j * 8 + bit;
                
                // replicate this pixel according to the screen scaling
                for (uint32_t scaleY = 0; scaleY < SimScreenScale; scaleY++)
                {
                    memset(&pSimScreenPixelBuf[(y*SimScreenScale + scaleY) * SimScreenWidth + x*SimScreenScale], pixel, SimScreenScale*sizeof(uint32_t));
                }
            }
        }
    }
    
    if (SDL_UpdateTexture(pScreenTexture, NULL, pSimScreenPixelBuf.get(), SimScreenWidth * sizeof(uint32_t)) < 0)
        throw SDLException("Could not update screen texture");
    
    if (SDL_RenderCopy(pRenderer, pScreenTexture, NULL, NULL) < 0)
        throw SDLException("Could not render screen copy");
    
    SDL_RenderPresent(pRenderer);
}

uint8_t* Graphics::GetScreenBuffer()
{
    return pPixelBuf.get();
}