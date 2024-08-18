#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_FILEPATH_RECORDED   128
#define MAX_FILEPATH_SIZE       1024
#define TEXT_BUFFER_SIZE       1024

int screenWidth     =   1280;
int screenHeight    =   720;

typedef enum channelType {
    CHANNEL_R,
    CHANNEL_G,
    CHANNEL_B,
    CHANNEL_A,
} channelType;

Image image = { 0 };
Image resizedImage = { 0 };
float scale = 0.0;
Texture texture = { 0 };
Shader shader = { 0 };
char* textBuffer;

bool waitingToScale = false;
bool waitingToSync = false;
bool waitingforImageData = true;

Vector2 mousePosition = { 0 };
Vector2 lastMousePosition = { 0 };
Vector2 mouseMovement = { 0 };

channelType selectedChannel = CHANNEL_A;
Color selectedColor = BLACK;

int paintValue = 0;
int paintValueBuffer = 0;

Color* imageData = NULL;


int filePathCounter = 0;
char *filePaths[MAX_FILEPATH_RECORDED] = { 0 };

void drawFrame ();
void drawCircleUntoData (double radius, channelType channel, unsigned char value);
Color *getImageData(Image* source);

#define DRAW_TEXT(info, x, y, size, color, ...) \
    snprintf(textBuffer, TEXT_BUFFER_SIZE, info, __VA_ARGS__); \
    DrawText(textBuffer, x, y, size, color)

inline bool lengthNegligible(Vector2 vec) {
    return (vec.x < EPSILON && vec.x > -EPSILON) && (vec.y < EPSILON && vec.y > -EPSILON);
}

inline float min(float a, float b) {
    return (a < b) ? a : b;
}

inline float max(float a, float b) {
    return (a < b) ? b : a;
}

inline int clamp (int source, int min, int max) {
    int intermediate = source < min ? min : source;
    return intermediate > max ? max : intermediate;
}

static inline void scaleImage() {
    scale = max(
        (float)image.width / (float)GetScreenWidth(),
        (float)image.height / (float)GetScreenHeight()
    );
    printf("w: %d, h: %d\n", GetScreenWidth(), GetScreenHeight());
    printf("stretch: %f\n", scale);
    waitingToScale = false;
}

static inline void syncTexture() {
    if (texture.width != 0) UnloadTexture(texture);
    texture = LoadTextureFromImage(image);
    SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);
    waitingToSync = false;
}

int main (int argc, char** argv) {
    InitWindow(screenWidth, screenHeight, "Image Tool");

    SetWindowState(FLAG_WINDOW_RESIZABLE);

    // Allocate space for the required file paths
    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++)
    {
        filePaths[i] = (char *)RL_CALLOC(MAX_FILEPATH_SIZE, 1);
    }
    textBuffer = RL_CALLOC(2048, 1);
    
    SetTargetFPS(60);
    shader = LoadShader(0, "Shaders/default.frag");
    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            waitingToScale = true;
        }
        if (waitingToScale && IsImageReady(image)) scaleImage();
        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (image.data != NULL) {
                UnloadImage(image);
                UnloadTexture(texture);
            }
            image = LoadImage(droppedFiles.paths[0]);
            //imageData = getImageData(&image);
            waitingToScale = true;
            waitingToSync = true;
            waitingforImageData = true;

            UnloadDroppedFiles(droppedFiles);
        }
        if (waitingforImageData && IsImageReady(image)) {
            imageData = getImageData(&image);
            waitingforImageData = false;
            printf("helo\n");
        }
        //printf("Scale:%f\n", stretch);
        if (image.data != NULL) {
            KeyboardKey k = GetKeyPressed();
            switch (k) {
                case KEY_R: {
                    selectedChannel = CHANNEL_R;
                } break;
                case KEY_G: {
                    selectedChannel = CHANNEL_G;
                } break;
                case KEY_B: {
                    selectedChannel = CHANNEL_B;
                } break;
                case KEY_A: {
                    selectedChannel = CHANNEL_A;
                } break;
                case KEY_ENTER: {
                    paintValue = paintValueBuffer;
                    paintValueBuffer = 0;
                    printf("Value: %d\n", paintValueBuffer);
                } break;
                case KEY_ESCAPE:
                case KEY_BACKSPACE: {
                    paintValueBuffer = 0;
                    printf("Input Buffer: 0\n");
                } break;
                default: {}
            }
            if (k >= KEY_ZERO && k <= KEY_NINE) {
                paintValueBuffer *= 10;
                paintValueBuffer += (k - KEY_ZERO);
                printf("Input Buffer: %d\n", paintValueBuffer);
            }
            mousePosition = Vector2Scale(GetMousePosition(), scale);
            mouseMovement = Vector2Subtract(mousePosition, lastMousePosition);
            selectedColor = GetImageColor(image, mousePosition.x, mousePosition.y);
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !lengthNegligible(mouseMovement)) {
                drawCircleUntoData(40.0, selectedChannel, paintValue);
                waitingToSync = true;
            }
            if (waitingToSync && IsImageReady(image)) syncTexture();
            lastMousePosition = mousePosition;
        }
        drawFrame();
    }

    for (int i = 0; i < MAX_FILEPATH_RECORDED; i++) {
        RL_FREE(filePaths[i]);
    }
    if (image.data != NULL) {
        UnloadImage(image);
        UnloadTexture(texture);
    }
    UnloadShader(shader);
    CloseWindow();

    return 0;
}

void drawFrame () {
    BeginDrawing();
    ClearBackground(GetColor(0x181818ff));
    if (image.data == NULL) {
        DrawText("Drop an image hear to work on it", screenWidth / 10, screenHeight / 10, 20, PURPLE);
    }
    else {
        BeginBlendMode(BLEND_CUSTOM);
        rlSetBlendFactors(RL_ZERO, RL_ONE, RL_MAX);
        BeginShaderMode(shader);
        DrawTextureEx(texture, (Vector2){0,0}, 0.0, 1.0 / scale, WHITE);
        EndShaderMode();
        EndBlendMode();
        if (IsKeyPressed(KEY_S) & IsKeyDown(KEY_LEFT_SHIFT)) {
            Image screenShot = LoadImageFromScreen();
            ExportImage(screenShot, "screenshot.png");
            UnloadImage(screenShot);
        }
        if (IsKeyPressed(KEY_S) & IsKeyDown(KEY_LEFT_CONTROL)) {
            ExportImage(image, "image.png");
        }
        Vector2 pos = GetMousePosition();
        DRAW_TEXT("COLOR: %d, %d, %d, %d", 64, 36, 20, GREEN, selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
        DRAW_TEXT("mouse x: %f, y: %f", 64, 72, 20, GREEN, pos.x, pos.y);
        switch (selectedChannel) {
            case CHANNEL_R: {
                snprintf(textBuffer, TEXT_BUFFER_SIZE, "Channel: RED");
            } break;
            case CHANNEL_G: {
                snprintf(textBuffer, TEXT_BUFFER_SIZE, "Channel: GREEN");
            } break;
            case CHANNEL_B: {
                snprintf(textBuffer, TEXT_BUFFER_SIZE, "Channel: BLUE");
            } break;
            case CHANNEL_A: {
                snprintf(textBuffer, TEXT_BUFFER_SIZE, "Channel: ALPHA");
            } break;
        }
        DrawText(textBuffer, 64, 108, 20, GREEN);
        DRAW_TEXT("Drawing With: %d", 64, 144, 20, GREEN, paintValue);
        DRAW_TEXT("%d", 64, 180, 20, GREEN, paintValueBuffer);
    }
    EndDrawing();
}

void drawCircleUntoData (double radius, channelType channel, unsigned char value) {
    int width;
    int index;
    int globalX;
    int globalY;
    int r = (int)round(radius);
    for (int y = -r; y <= r; y++) {
        width = (int)round(cos(((float)y / radius) * (PI / 2)) * radius);
        for (int x = -width; x <= width; x++) {
            globalX = clamp(x + mousePosition.x, 0, image.width - 1);
            globalY = clamp(y + mousePosition.y, 0, image.height - 1);
            index = globalX + globalY*image.width;
            switch (channel) {
                case CHANNEL_R: {
                    imageData[index].r = value;
                } break;
                case CHANNEL_G: {
                    imageData[index].g = value;
                    printf("x: %d, y: %d\n", globalX, globalY);
                } break;
                case CHANNEL_B: {
                    imageData[index].b = value;
                } break;
                case CHANNEL_A: {
                    imageData[index].a = value;
                } break;
            }
        }
    }
}

Color* getImageData(Image* source) {
    if (source->format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {
        printf("Converting image to 32bit RGBA format\n");
        printf("%d\n", source->format);
        Color* newData = LoadImageColors(*source);
        RL_FREE(source->data);
        source->data = newData;
    }
    return (Color*)(source->data);
}