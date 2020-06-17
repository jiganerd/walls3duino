//
//  Graphics.hpp
//  walls3duino
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Graphics_hpp
#define Graphics_hpp

#include <string>
#include <cmath>
#include <cstdint>
#include <memory>
#include "SDLHeader.hpp"
#include "Vec2.hpp"

// this class simulates an SSD1306 128x64 black/white OLED display, and somewhat the
// Adafruit SSD1306 display driver, in one of two modes:
// 1) horizontal addressing mode, where the display is addressed left to right and then
//    top to bottom, in 8-bit pages which each represent a chunk of a vertical column, or
// 2) vertical addressing mode, where the display is addressed top to bottom and then
//    left to right, using the same 8-bit vertical pages
// see the SSD1306 datasheet for details
//
// this class uses a full screen pixel buffer for horizontal addressing mode, and only
// a single column pixel buffer for vertical addressing mode
class Graphics
{
public:
    class Exception
    {
    public:
        virtual std::string GetMsg() const = 0;
    };
    
    Graphics();
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    void EndFrame();
    void EndColumn();
    uint8_t* GetScreenBuffer();
    uint8_t* GetColumnBuffer();
    ~Graphics();
    
private:
    class SDLException : public Exception
    {
    public:
        SDLException(std::string msg);
        std::string GetMsg() const override;
    private:
        std::string error;
        std::string msg;
    };
    
    void WritePageToSimScreenPixelBuf(uint8_t pageData, uint32_t x, uint32_t pageNum);
    void FlushSimScreenPixelBuf();

    SDL_Window* pWindow;
    SDL_Renderer* pRenderer;
    SDL_Texture* pScreenTexture;
    std::unique_ptr<uint32_t[]> pSimScreenPixelBuf;
    
    // full-screen pixel buffer arranged as in SSD1306 horizontal addressing mode
    std::unique_ptr<uint8_t[]> pPixelBuf;
    
    // single-column pixel buffer arranged as in SSD1306 vertical addressing mode
    std::unique_ptr<uint8_t[]> pColumnBuf;
    uint32_t currColumn; // used for column-drawing mode only
    
public:
    static constexpr uint32_t ScreenWidth {128u};
    static constexpr uint32_t ScreenHeight {64u};
    static constexpr uint32_t ScreenHeightPages {ScreenHeight/8};
    
    static constexpr uint32_t SimScreenScale {4u};
    static constexpr uint32_t SimScreenWidth {ScreenWidth * SimScreenScale};
    static constexpr uint32_t SimScreenHeight {ScreenHeight * SimScreenScale};
};

#endif /* Graphics_hpp */
