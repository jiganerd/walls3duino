//
//  Graphics.hpp
//  walls3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Graphics_hpp
#define Graphics_hpp

#include <string>
#include <cmath>
#include <cstdint>
#include <SDL2/SDL.h>
#include "Vec2.hpp"

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
    void BeginFrame();
    void EndFrame();
    uint8_t* GetScreenBuffer();
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

    SDL_Window* pWindow;
    SDL_Renderer* pRenderer;
    SDL_Texture* pScreenTexture;
    std::unique_ptr<uint8_t[]> pPixelBuf;
    std::unique_ptr<uint32_t[]> pSimScreenPixelBuf;
    
public:
    static constexpr uint32_t ScreenWidth {128u};
    static constexpr uint32_t ScreenHeight {64u};
    static constexpr uint32_t ScreenHeightChunks {ScreenHeight/8};
    
    static constexpr uint32_t SimScreenScale {4u};
    static constexpr uint32_t SimScreenWidth {ScreenWidth * SimScreenScale};
    static constexpr uint32_t SimScreenHeight {ScreenHeight * SimScreenScale};
};

#endif /* Graphics_hpp */
