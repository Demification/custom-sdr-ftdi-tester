#include "Utilits.hpp"

#include <libgen.h> 
#include <unistd.h>
#include <linux/limits.h>

std::string getApplicationDirPath() {
    char exepath[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", exepath, PATH_MAX);
    return std::string(dirname(exepath));
}
