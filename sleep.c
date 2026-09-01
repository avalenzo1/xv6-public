#include "types.h"
#include "user.h"

// Uncommited work

int main(int argc, char* argv[]) {
    if (argc < 2) {
        // Insufficient commands
        printf(2, "sleep <amt>\n");
    }

    uint amt = atoi(argv[1]);

    sleep(amt);

    exit();
}