#version 450 core

layout(location = 0) out vec4 FragColor;
uniform sampler2DMS u_screenTexture;
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

void main() {
    vec3 color = getTexColor(texCoord);
    if(postProcess) {
        color = vec3(0.0f);
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {

                int kernelIdx = ((-y+1)*3)+(x+1);
                float uvOffX = x * (1.0f / width);
                float uvOffY = y * (1.0f / height);

                vec2 kernelUV = vec2(texCoord.x + uvOffX, texCoord.y + uvOffY);
                color += getTexColor(kernelUV) * kernel[kernelIdx];
            }
        }
    }
    FragColor = vec4(color, 1);
}