/*
 * kernel.h
 *
 *  Created on: Oct 30, 2023
 *      Author: Jacki
 */

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H_
#define RUN_FIRST_THREAD 0x3
#define YIELD 0x2
#define NUMBER_OF_STACKS 9

#define SHPR2 *(uint32_t*)0xE000ED1C //for setting SVC priority, bits 31-24
#define SHPR3 *(uint32_t*)0xE000ED20 // PendSV is bits 23-16
#define _ICSR *(uint32_t*)0xE000ED04 //This lets us trigger PendSV


typedef struct thread{
uint32_t* sp; //stack pointer
void (*thread_function)(void*); //function pointer
uint32_t timeslice; // max run time [ms]
uint32_t runtime; // time left to run [ms]
}thread;
typedef struct input_arguments{
	uint32_t add;
	uint32_t div;
}input_arguments;

extern uint32_t global_variable;
extern thread threadArray[NUMBER_OF_STACKS];
extern uint32_t activeThread;
extern uint32_t numberOfThreads;

void osKernelInitialize(void);
uint32_t* Create_Stack(void);
void SVC_Handler_Main( unsigned int *svc_args );
_Bool osCreateThread(void* fcn, void* args);
_Bool osCreateThreadWithDeadline(void* fcn, void* args, uint32_t timeout);
void osKernelStart(void);
void Run_First_Thread(void* fcn);
void osYield(void);
void osSched();

#endif /* INC_KERNEL_H_ */
