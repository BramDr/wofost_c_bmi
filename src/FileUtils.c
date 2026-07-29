#include "file_utils.h"

int dir_exists(const char *path) {
#ifdef _WIN32
  struct _stat info;
  if (_stat(path, &info) != 0)
    return 0;
  return (info.st_mode & _S_IFDIR) != 0;
#else
  struct stat info;
  if (stat(path, &info) != 0)
    return 0;
  return S_ISDIR(info.st_mode);
#endif
}

int mkdir_if_not_exists(const char *path) {
  if (dir_exists(path)) {
    return 0; // already exists, treat as success
  }

  if (MKDIR(path) == 0) {
    return 0; // created successfully
  }

  // failed
  perror("mkdir failed");
  return -1;
}