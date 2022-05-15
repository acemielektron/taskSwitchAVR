#include "taskCtl.h"
#include <string.h>
#include <avr/io.h>

#define MAX_TASK 5
#define MIN_STACK_SIZE 48

struct task tasks[MAX_TASK + 1]; //first task is main task
struct task *pCurrentTask;
void *freeStack = (void *)RAMEND; // pointer to freeStack
void *minStack = (void *)RAMSTART + MIN_STACK_SIZE; // pointer to minimum stack address

void initSwitcher(int16_t dataSize, int16_t mainStackSize)
{     
  for (int8_t i = 0; i < MAX_TASK; i++)
  {
    tasks[i].nextTask = &tasks[i+1];
    tasks[i].sp = 0;
    tasks[i].ready = false;
  }  
  minStack += dataSize; // update minimum allowable stack address  
  freeStack -= mainStackSize; // update freeStack address
  pCurrentTask = &tasks[0]; // first task is main task
  pCurrentTask ->ready = true; // main task is always ready
  tasks[MAX_TASK].nextTask = pCurrentTask; // last task stores pointer to main task  
  setTimer0msInt(); //start interrupt (preemptive task switching)
}

int8_t startNewTask(tpTaskFunc pTaskFunc, int16_t stackSize)
{
  int8_t taskNum = 1; // start after main task

  //check if requested stackSize is valid and we have enough stack space
  if (stackSize >= MIN_STACK_SIZE || (freeStack - stackSize) < minStack)
  {
    while (taskNum < MAX_TASK)
    {		
      if (tasks[taskNum].sp == 0) //task slot empty
      {
        //PUSH: This instruction stores the contents of register Rr on the STACK. 
        //The Stack Pointer is post-decremented by 1 after the PUSH
        //POP: This instruction loads register Rd with a byte from the STACK. 
        //The Stack Pointer is pre-incremented by 1 before the POP.    
        tasks[taskNum].sp = (uint8_t *)freeStack; //set task stack pointer to top of stack        
        freeStack -= stackSize; // update freeStack address
        tasks[taskNum].sp[0] = (uint16_t)pTaskFunc & 0xFF; //push low(address)
        tasks[taskNum].sp--;
        tasks[taskNum].sp[0] = (uint16_t)pTaskFunc >> 8; //push high(address)       
        tasks[taskNum].sp--;        
        for (int8_t i = 0; i < 33; i++)
        {
          tasks[taskNum].sp[0] = 0; // push cleaned register
          tasks[taskNum].sp--; // decrement stack pointer
        }               
        tasks[taskNum].ready = true;
        return taskNum; // task successfully allocated and started
      }
      taskNum++;
    }
    taskNum = -1; // no task slot available
  }
  else taskNum = 0; // task not allocated (invalid or insufficient stack size)
  return taskNum;
}