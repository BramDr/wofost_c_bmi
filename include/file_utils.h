#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

extern int dir_exists(const char *path);
extern int mkdir_if_not_exists(const char *path);

#endif // FILE_UTILS_H