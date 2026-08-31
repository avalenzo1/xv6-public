#include "types.h"
#include "user.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        // Insufficient commands
        printf(2, "sleep <amt>\n");
    }

    int amt = atoi(argv[1]);

    sleep(amt);

    exit();
}