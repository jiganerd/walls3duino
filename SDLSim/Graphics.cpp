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
    pColumnBuf{new uint8_t[ScreenHeight / 8]},
    pSimScreenPixelBuf{new uint32_t[SimScreenWidth * SimScreenHeight]},
    currColumn{0}
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

void Graphics::EndFrame()
{
    // adapt vertical 1-bit buffer to SDL buffer
    for (uint32_t pageNum = 0; pageNum < ScreenHeightPages; pageNum++)
        for (uint32_t x = 0; x < ScreenWidth; x++)
            WritePageToSimScreenPixelBuf(pPixelBuf[pageNum * ScreenWidth + x], x, pageNum);
    
    FlushSimScreenPixelBuf();
}

void Graphics::EndColumn()
{
    for (uint32_t pageNum = 0; pageNum < ScreenHeightPages; pageNum++)
        WritePageToSimScreenPixelBuf(pColumnBuf[pageNum], currColumn, pageNum);
    
    if (++currColumn == ScreenWidth)
        currColumn = 0;
    
    // simulate "a little bit" the fact that the hardware shows rendering column to column
    // as the screen is drawn
    // we don't do this for every column because this is actually quite a slow operation
    if (currColumn % 4 == 0)
        FlushSimScreenPixelBuf();
}

uint8_t* Graphics::GetScreenBuffer()
{
    return pPixelBuf.get();
}

uint8_t* Graphics::GetColumnBuffer()
{
    return pColumnBuf.get();
}

void Graphics::WritePageToSimScreenPixelBuf(uint8_t pageData, uint32_t x, uint32_t pageNum)
{
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        uint32_t pixel = (((pageData >> bit) & 0x01) ? 0x00FFFFFF : 0x00000000);
        uint8_t y = pageNum * 8 + bit;
        
        // replicate this pixel according to the screen scaling
        for (uint32_t scaleY = 0; scaleY < SimScreenScale; scaleY++)
        {
            memset(&pSimScreenPixelBuf[(y * SimScreenScale + scaleY) * SimScreenWidth + x * SimScreenScale], pixel, SimScreenScale * sizeof(uint32_t));
        }
    }
}

void Graphics::FlushSimScreenPixelBuf()
{
    if (SDL_UpdateTexture(pScreenTexture, NULL, pSimScreenPixelBuf.get(), SimScreenWidth * sizeof(uint32_t)) < 0)
        throw SDLException("Could not update screen texture");
    
    if (SDL_RenderCopy(pRenderer, pScreenTexture, NULL, NULL) < 0)
        throw SDLException("Could not render screen copy");
    
    SDL_RenderPresent(pRenderer);
}