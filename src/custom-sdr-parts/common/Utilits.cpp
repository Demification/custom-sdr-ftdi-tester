#include "Utilits.hpp"
#include "memory.h"
#include <libgen.h> 
#include <unistd.h>
#include <linux/limits.h>

std::string getApplicationDirPath() {
    char exepath[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", exepath, PATH_MAX);
    memset(&exepath[count], 0, PATH_MAX-count);
    return std::string(dirname(exepath));
}
