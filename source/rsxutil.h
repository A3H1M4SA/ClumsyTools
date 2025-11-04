/*
 * RSX utility functions for PS3 graphics
 */

#ifndef __RSXUTIL_H__
#define __RSXUTIL_H__

#include <rsx/gcm_sys.h>
#include <rsx/rsx.h>

#define HOST_SIZE (1024*1024)

typedef struct {
    int height;
    int width;
    uint32_t id;
    uint32_t *ptr;
    uint32_t offset;
} buffer;

gcmContextData *initScreen(void *host_addr, u32 size);
void getResolution(videoResolution *res);
buffer *makeBuffer(int width, int height, int id);
void flip(gcmContextData *context, s32 buffer);
void waitFlip();

#endif /* __RSXUTIL_H__ */

