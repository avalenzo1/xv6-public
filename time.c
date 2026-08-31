#include "types.h"
#include "user.h"

int main(int argc, char* argv[])
{
    // Insufficient commands
    if (argc < 2) {
        printf(2, "time <command>...\n");
        exit();
    }

    int cpid = fork();

    if (cpid) {
        // parent
        uint cycles_before = 0;

        asm("rdtscp"
        : "=r" (cycles_before));

        wait();

        uint cycles_after = 0;

        asm("rdtscp"
        : "=r" (cycles_after));

        printf(2, "%s took %d cycles to run.\n", argv[1], cycles_after - cycles_before);
    } else {
        // child

        char** fake_argv = argv + 1;
        
        exec(argv[1], fake_argv);
    }

    exit();
}