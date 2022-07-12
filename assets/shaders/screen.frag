#version 450 core

layout(location = 0) out vec4 FragColor;
//uniform sampler2DMS u_screenTexture;
uniform sampler2D u_screenTexture;
uniform int width;
uniform int height;
uniform bool postProcess;
uniform int samples;
in vec2 texCoord;

float kernel[] = {
    1, 1, 1,
    1, -8, 1,
    1, 1, 1
};

vec3 getTexColor(vec2 uv) {
    vec3 avgColor = vec3(0.0f);
    ivec2 texCoordMS = ivec2(uv.x * width, uv.y * height);
    for(int i = 0; i < samples; i++) {
        avgColor += vec3(texelFetch(u_screenTexture, texCoordMS, i));
    }
    return avgColor / float(samples);
}

float character(int n, vec2 p)
{
    p = floor(p*vec2(4.0, -4.0) + 2.5);
    if (clamp(p.x, 0.0, 4.0) == p.x)
    {
        if (clamp(p.y, 0.0, 4.0) == p.y)
        {
            int a = int(round(p.x) + 5.0 * round(p.y));
            if (((n >> a) & 1) == 1) return 1.0;
        }
    }
    return 0.0;
}

vec3 doPostProcess() {
    vec2 pix = gl_FragCoord.xy;
    vec2 uv = floor(pix/8.0f) * 8.0f / vec2(width, height);
    vec3 col = getTexColor(uv);

    float gray = 0.3 * col.r + 0.59 * col.g + 0.11 * col.b;

    int n =  4096;                // .
    if (gray > 0.2) n = 65600;    // :
    if (gray > 0.3) n = 332772;   // *
    if (gray > 0.4) n = 15255086; // o
    if (gray > 0.5) n = 23385164; // &
    if (gray > 0.6) n = 15252014; // 8
    if (gray > 0.7) n = 13199452; // @
    if (gray > 0.8) n = 11512810; // #

    vec2 p = mod(pix/4.0, 2.0) - vec2(1.0);

    col = col*vec3(character(n, p));

    return col;
}

void main() {
//    vec3 color = getTexColor(texCoord);
    vec3 color = vec3(texture(u_screenTexture, texCoord));
    if(postProcess) {
        color = doPostProcess();
        /*color = vec3(0.0f);
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {

                int kernelIdx = ((-y+1)*3)+(x+1);
                float uvOffX = x * (1.0f / width);
                float uvOffY = y * (1.0f / height);

                vec2 kernelUV = vec2(texCoord.x + uvOffX, texCoord.y + uvOffY);
                color += getTexColor(kernelUV) * kernel[kernelIdx];
            }
        }*/
    }
    FragColor = vec4(color, 1);
}