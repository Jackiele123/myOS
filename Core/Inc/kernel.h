/*
 * kernel.h
 *
 *  Created on: Oct 30, 2023
 *      Author: Jacki
 */

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H_

typedef struct k_thread{
uint32_t* sp; //stack pointer
void (*thread_function)(void*); //function pointer
}thread;

void osKernelInitialize(void);
uint32_t* Create_Stack(void);
void SVC_Handler_Main( unsigned int *svc_args );
_Bool osCreateThread(void* fcn);
void osKernelStart(void);
void Run_First_Thread(void* fcn);

#define RUN_FIRST_THREAD 0x3

#endif /* INC_KERNEL_H_ */
