/*
 * ClumsyTools - PS3 Homebrew Application
 * 
 * A simple starter template for PS3 homebrew development
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <sysutil/video.h>
#include <rsx/gcm_sys.h>
#include <rsx/rsx.h>
#include <sys/process.h>
#include <io/pad.h>

#include "rsxutil.h"

#define MAX_BUFFERS 2

buffer *buffers[MAX_BUFFERS];
int currentBuffer = 0;

gcmContextData *context;
videoResolution res;
int display_width;
int display_height;

/*
 * Initialize screen and graphics
 */
static void init_screen()
{
    void *host_addr = memalign(1024*1024, HOST_SIZE);
    assert(host_addr != NULL);

    context = initScreen(host_addr, HOST_SIZE);
    getResolution(&res);
    
    display_width = res.width;
    display_height = res.height;

    for (int i = 0; i < MAX_BUFFERS; i++)
        buffers[i] = makeBuffer(display_width, display_height, i);

    flip(context, MAX_BUFFERS - 1);
}

/*
 * Wait for button press
 */
static void waitForButton()
{
    padInfo padinfo;
    padData paddata;
    
    ioPadInit(7);
    
    while(1)
    {
        ioPadGetInfo(&padinfo);
        
        for(int i = 0; i < MAX_PADS; i++)
        {
            if(padinfo.status[i])
            {
                ioPadGetData(i, &paddata);
                
                // Check if any button is pressed
                if(paddata.BTN_CROSS || paddata.BTN_CIRCLE || 
                   paddata.BTN_START || paddata.BTN_SELECT)
                {
                    ioPadEnd();
                    return;
                }
            }
        }
        
        usleep(10000); // 10ms
    }
}

/*
 * Draw a simple colored rectangle
 */
static void drawRect(int x, int y, int width, int height, uint32_t color)
{
    for(int yy = y; yy < y + height; yy++)
    {
        for(int xx = x; xx < x + width; xx++)
        {
            if(xx >= 0 && xx < display_width && yy >= 0 && yy < display_height)
            {
                buffers[currentBuffer]->ptr[yy * display_width + xx] = color;
            }
        }
    }
}

/*
 * Clear screen with color
 */
static void clearScreen(uint32_t color)
{
    for(int y = 0; y < display_height; y++)
    {
        for(int x = 0; x < display_width; x++)
        {
            buffers[currentBuffer]->ptr[y * display_width + x] = color;
        }
    }
}

/*
 * Main application entry point
 */
int main(int argc, const char* argv[])
{
    // Initialize screen
    init_screen();
    
    // Colors (ARGB format)
    uint32_t COLOR_BLACK = 0xFF000000;
    uint32_t COLOR_WHITE = 0xFFFFFFFF;
    uint32_t COLOR_RED   = 0xFFFF0000;
    uint32_t COLOR_GREEN = 0xFF00FF00;
    uint32_t COLOR_BLUE  = 0xFF0000FF;
    
    // Main loop
    int frame = 0;
    while(1)
    {
        // Clear to black
        clearScreen(COLOR_BLACK);
        
        // Draw some colored rectangles
        drawRect(100, 100, 200, 100, COLOR_RED);
        drawRect(350, 100, 200, 100, COLOR_GREEN);
        drawRect(600, 100, 200, 100, COLOR_BLUE);
        
        // Draw a moving rectangle
        int x = 100 + (frame % 600);
        drawRect(x, 300, 50, 50, COLOR_WHITE);
        
        // Flip buffer
        flip(context, buffers[currentBuffer]->id);
        currentBuffer = !currentBuffer;
        
        frame++;
        
        // Check for exit (this is simplified, add proper pad handling)
        usleep(16666); // ~60 FPS
        
        // For now, exit after 10 seconds (600 frames)
        if(frame > 600)
            break;
    }
    
    // Cleanup
    gcmSetWaitFlip(context);
    
    for(int i = 0; i < MAX_BUFFERS; i++)
        rsxFree(buffers[i]->ptr);
    
    rsxFinish(context, 1);
    
    return 0;
}

