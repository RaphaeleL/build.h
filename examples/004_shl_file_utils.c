#define SHL_IMPLEMENTATION
#define SHL_STRIP_PREFIX
#include "../build.h"

int main() {
    String file = {0};

    if (shl_read_file("./README.md", &file)) {
        info("File successful read! (Cap: %d, Len: %d:)\n", file.cap, file.len);
        for (size_t i = 0; i < file.len; ++i) {
            hint("%s\n", file.data[i]);
        }
    }


    return 0;
}
