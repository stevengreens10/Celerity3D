#version 450 core

layout(location = 0) out vec4 FragColor;
uniform sampler2D u_screenTexture;
uniform int width;
uniform int height;
in vec2 texCoord;

float kernel[] = {
    1, 1, 1,
    1, -8, 1,
    1, 1, 1
};

void main() {
    vec3 color = vec3(texture(u_screenTexture, texCoord));
    if(texCoord.x > 0.5f) {
        color = vec3(0.0f);
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {

                int kernelIdx = ((-y+1)*3)+(x+1);
                float uvOffX = x * (1.0f / width);
                float uvOffY = y * (1.0f / height);

                vec2 kernelUV = vec2(texCoord.x + uvOffX, texCoord.y + uvOffY);
                color += vec3(texture(u_screenTexture, kernelUV) * kernel[kernelIdx]);
            }
        }
    }
    FragColor = vec4(color, 1);
}