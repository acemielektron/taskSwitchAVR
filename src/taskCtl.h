#include <stdint.h>

// structure to hold our task variables
// if we have more ram, add stack min/max bounds and do overflow/underflow checks
struct task{
    struct task* nextTask;
    uint8_t *sp;    
    bool ready;  
};

typedef void (*tpTaskFunc)(void);

void initSwitcher(int16_t dataSize, int16_t mainStackSize);
int8_t startNewTask(tpTaskFunc pTaskFunc, int16_t stackSize);