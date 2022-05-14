#include "taskCtl.h"
#include "util.h"
#include <string.h>

#define MAX_TASK 5
#define MIN_STACK 50

struct task tasks[MAX_TASK + 1]; //first task is main task
struct task *pCurrentTask;

void initSwitcher()
{  
  for (int8_t i = 0; i < MAX_TASK; i++)
  {
    tasks[i].nextTask = &tasks[i+1];
    tasks[i].sp = 0;
    tasks[i].ready = false;
  }
  pCurrentTask = &tasks[0]; // first task is main task
  pCurrentTask ->ready = true;
  tasks[MAX_TASK].nextTask = pCurrentTask; // last task stores pointer to main task
  setTimer0msInt(); //start interrupt
}

int8_t startNewTask(tpTaskFunc pTaskFunc, uint8_t *stack, int16_t stackSize)
{
  int8_t taskNum = 1; // start after main task

  if (stackSize >= MIN_STACK)
  {
    while (taskNum < MAX_TASK)
    {		
      if (tasks[taskNum].sp == 0) //task slot empty
      {
        //PUSH: This instruction stores the contents of register Rr on the STACK. 
        //The Stack Pointer is post-decremented by 1 after the PUSH
        //POP: This instruction loads register Rd with a byte from the STACK. 
        //The Stack Pointer is pre-incremented by 1 before the POP.
        //stack-top = stack-size - 1;
        //clear space for 32 registers, status register and return address 
        // 32 + 1 + 2 = 35

        // restore context will first pop 33 registers -> sp = sp + 33
        // then pop return address (2) -> sp = sp + 2
        struct task *newTask = &tasks[taskNum];
        newTask->sp = stack + stackSize - 1; //set task stack pointer to top of stack        
        newTask->sp[0] = (uint16_t)pTaskFunc & 0xFF; //push low(address)
        newTask->sp--;
        newTask->sp[0] = (uint16_t)pTaskFunc >> 8; //push high(address)       
        newTask->sp--;        
        for (int8_t i = 0; i < 33; i++)
        {
          newTask->sp[0] = 0; // push cleaned register
          newTask->sp--; // decrement stack pointer
        }               
        newTask->ready = true;
        return taskNum; // task successfully allocated and started
      }
      taskNum++;
    }
    taskNum = -1; // no task slot available
  }
  else taskNum = 0; // task not allocated (insufficient stack size)
  return taskNum;
}