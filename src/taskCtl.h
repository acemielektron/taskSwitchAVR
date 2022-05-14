#include <stdint.h>

struct task{
    struct task* nextTask;
    volatile uint8_t *sp;
    bool ready;    
};

typedef void (*tpTaskFunc)(void);

void initSwitcher();
int8_t startNewTask(tpTaskFunc pTaskFunc, uint8_t *stack, int16_t stackSize);