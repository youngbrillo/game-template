#pragma once
#include <raylib.h>
#include <raymath.h>
#include <string>

namespace lib
{
    Shader LoadBasicLightingShader();

    struct LightEmitter
    {

        void init();

        void free();

        void apply(Shader& p_shader);
    };
    
    struct LightSource
    {
        Shader shader = {0};
        Vector4 ambient = ColorNormalize(LIGHTGRAY);

        void init();
        void free();
    };

}