/*
 * RSX utility functions implementation
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include <sysutil/video.h>
#include <rsx/gcm_sys.h>
#include <rsx/rsx.h>

#include "rsxutil.h"

static int current_buffer = 0;

void waitFlip()
{
    while(gcmGetFlipStatus() != 0)
        usleep(200);
    gcmResetFlipStatus();
}

void flip(gcmContextData *context, s32 buffer)
{
    if(gcmSetFlip(context, buffer) == 0)
    {
        rsxFlushBuffer(context);
        gcmSetWaitFlip(context);
    }
}

buffer *makeBuffer(int width, int height, int id)
{
    buffer *buf = malloc(sizeof(buffer));
    
    buf->width = width;
    buf->height = height;
    buf->id = id;
    
    int depth = 4;
    int size = depth * width * height;
    
    buf->ptr = (uint32_t *)rsxMemalign(64, size);
    
    if(buf->ptr == NULL)
    {
        printf("Error allocating buffer memory\n");
        return NULL;
    }
    
    if(rsxAddressToOffset(buf->ptr, &buf->offset) != 0)
    {
        printf("Error getting buffer offset\n");
        return NULL;
    }
    
    if(gcmSetDisplayBuffer(id, buf->offset, width * depth, width, height) != 0)
    {
        printf("Error setting display buffer\n");
        return NULL;
    }
    
    return buf;
}

void getResolution(videoResolution *res)
{
    videoState state;
    videoGetState(0, 0, &state);
    videoGetResolution(state.displayMode.resolution, res);
}

gcmContextData *initScreen(void *host_addr, u32 size)
{
    gcmContextData *context = NULL;
    videoState state;
    videoConfiguration vconfig;
    videoResolution res;
    
    // Initialize gcm
    rsxInit(&context, 0x10000, size, host_addr);
    
    if(context == NULL)
    {
        printf("Error initializing RSX context\n");
        return NULL;
    }
    
    // Get video state
    if(videoGetState(0, 0, &state) != 0)
    {
        printf("Error getting video state\n");
        return NULL;
    }
    
    // Configure video output
    memset(&vconfig, 0, sizeof(videoConfiguration));
    vconfig.resolution = state.displayMode.resolution;
    vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
    vconfig.pitch = 0;
    vconfig.aspect = state.displayMode.aspect;
    
    if(videoConfigure(0, &vconfig, NULL, 0) != 0)
    {
        printf("Error configuring video\n");
        return NULL;
    }
    
    if(videoGetState(0, 0, &state) != 0)
    {
        printf("Error getting video state (2)\n");
        return NULL;
    }
    
    gcmSetFlipMode(GCM_FLIP_VSYNC);
    
    return context;
}

