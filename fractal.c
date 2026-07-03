#include "include/raylib.h"
#include <complex.h>
#include <stdlib.h>
#include <stdio.h>


double complex smallest(double complex a, double complex b) {
    return (cabs(a) < cabs(b)) ? 1 : 0;
}

unsigned int changeBrightness(unsigned int hexColor, float factor) {
    unsigned char r = (hexColor >> 24) & 0xFF;
    unsigned char g = (hexColor >> 16) & 0xFF;
    unsigned char b = (hexColor >> 8) & 0xFF;
    unsigned char a = hexColor & 0xFF;

    r = (unsigned char)(r * factor);
    g = (unsigned char)(g * factor);
    b = (unsigned char)(b * factor);

    return (r << 24) | (g << 16) | (b << 8) | a;
}

// Newton's Fractal for f(z) = z**3 - 1 function 
// (you can use any other complex function and fractals will look different)
unsigned int polynomial(double complex x, double complex y) {
    double complex z0 = x + y*I; 
    double complex step; 
    int iter_count = 0;
    const double complex root1 = 1 + 0*I;
    const double complex root2 = -0.5 + 0.866*I;
    const double complex root3 = -0.5 - 0.866*I;

    do {
        if (cabs(z0) < 0.0000001) {
            z0 = 0.000001 + 0.000001*I;
        }
        step = (z0 * z0 * z0 - 1) / (3 * z0 * z0);
        z0 = z0 - step;
        iter_count++;
    } while (cabs(step) > 0.000001 && iter_count < 150);
    
    double complex diff1 = root1 - z0;
    double complex diff2 = root2 - z0;
    double complex diff3 = root3 - z0;
    Color color; 

    if (smallest(diff1, diff2) && smallest(diff1, diff3)) {
        color = RED;
    } else if (smallest(diff2, diff1) && smallest(diff2, diff3)) {
        color = BLUE;
    } else {
        color = GREEN;
    }

    unsigned int darkColor = ColorToInt(color);

    if (iter_count > 90) {
        darkColor = changeBrightness(darkColor, 0.1);
    } else if (iter_count > 70) {
        darkColor = changeBrightness(darkColor, 0.3);
    } else if (iter_count > 55) {
        darkColor = changeBrightness(darkColor, 0.4);
    } else if (iter_count > 40) {
        darkColor = changeBrightness(darkColor, 0.5);
    } else if (iter_count > 25) {
        darkColor = changeBrightness(darkColor, 0.6);
    } else if (iter_count > 10) {
        darkColor = changeBrightness(darkColor, 0.7);
    }
    else {
        darkColor = changeBrightness(darkColor, 0.95);
    }

    return darkColor;
}

void pxgrid(int width, int height, Color *raw_data, float zoomScale, float offsetX, float offsetY, int step) {  

    for (int x = 0; x < width; x += step) {
        for (int y = 0; y < height; y += step) {
            // compute complex values relative to the coordinate plane
            double complex cx = ((double)x / width) * zoomScale + offsetX;
            double complex cy = ((double)y / width) * zoomScale + offsetY;
            unsigned int hexcolor = polynomial(cx, cy);
            Color pxcolor = GetColor(hexcolor);

            for (int ix = 0; ix < step && (x + ix) < width; ix++) {
                for (int iy = 0; iy < step && (y + iy) < height; iy++) {
                    raw_data[(y + iy) * width + (x + ix)] = pxcolor;
                }
            } 
        }
    }
}

void rerender(
        int width, int height, Color *pixels, Texture2D fractal_texture, float oldZoom, float newZoom, float *offsetX, float *offsetY, int step
) {
    float mousex = ((float)GetMouseX() / width) * oldZoom + *offsetX;
    float mousey = ((float)GetMouseY() / width) * oldZoom + *offsetY;
    
    // calculate new X and Y coordinate after scaling
    *offsetX = mousex - ((float)GetMouseX() / width) * newZoom;
    *offsetY = mousey - ((float)GetMouseY() / width) * newZoom;

    pxgrid(width, height, pixels, newZoom, *offsetX, *offsetY, step);
    UpdateTexture(fractal_texture, pixels);
}

int main(void) {
    int width = 800;
    int height = 600;
    float zoomScale = 4.0;
    float offsetX = -2.0;
    float offsetY = -2.0;  

    Color *pixels = (Color *)malloc(width * height * sizeof(Color));
    if (pixels == NULL) {
        fprintf(stderr, "Something went wrong");
        return 1;
    }

    InitWindow(width, height, "Fractal");
    SetTargetFPS(60);

    pxgrid(width, height, pixels, zoomScale, offsetX, offsetY, 1);
    Image fractalimg = {
        .data = pixels,
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    Texture2D fractal_texture = LoadTextureFromImage(fractalimg);
    UnloadImage(fractalimg);

    int is_dirty = 0;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) break;

        float mouse = GetMouseWheelMove(); 
        Vector2 mouseDelta = GetMouseDelta();
        bool isPanning = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        // when you hold down the mouse wheel the texture renders in 4x4 blocks. 
        // it reduces cpu load and prevents crashes.
        if (mouse != 0) {
            float oldZoom = zoomScale;
            if (mouse == 1.0f)  zoomScale *= 0.9f;
            else  zoomScale /= 0.9f;
            rerender(width, height, pixels, fractal_texture, oldZoom, zoomScale, &offsetX, &offsetY, 4);
            is_dirty = 1;
        }     
        else if (IsKeyPressed(KEY_KP_ADD)) { 
            float oldZoom = zoomScale;
            zoomScale *= 0.9f; 
            rerender(width, height, pixels, fractal_texture, oldZoom, zoomScale, &offsetX, &offsetY, 4);
            is_dirty = 1;
        } 
        else if (IsKeyPressed(KEY_KP_SUBTRACT)) { 
            float oldZoom = zoomScale;
            zoomScale /= 0.9f; 
            rerender(width, height, pixels, fractal_texture, oldZoom, zoomScale, &offsetX, &offsetY, 4);
            is_dirty = 1;
        }
        else if (isPanning && (mouseDelta.x != 0 || mouseDelta.y != 0)) {
            offsetX -= (mouseDelta.x / width) * zoomScale;
            offsetY -= (mouseDelta.y / width) * zoomScale;

            pxgrid(width, height, pixels, zoomScale, offsetX, offsetY, 4);
            UpdateTexture(fractal_texture, pixels);
            is_dirty = 1;
        }
        else if (is_dirty) {
            pxgrid(width, height, pixels, zoomScale, offsetX, offsetY, 1);
            UpdateTexture(fractal_texture, pixels);
            is_dirty = 0;
        }

        BeginDrawing();
        ClearBackground(BLACK);
 
        DrawTexture(fractal_texture, 0, 0, WHITE);
        EndDrawing();
    }

    UnloadTexture(fractal_texture);
    free(pixels);
    CloseWindow();
    return 0;
}

