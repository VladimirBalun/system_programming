#ifndef __UTILS_H__
#define __UTILS_H__

#define TRUE 1
#define FALSE 0

#define SAFE_DELETE(pointer) \
    free((pointer)); \
    (pointer) = NULL;

#endif // __UTILS_H__