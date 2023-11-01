/*
 * kernel.c
 *
 *  Created on: Oct 30, 2023
 *      Author: Jacki
 */
#include "main.h"
#include "kernel.h"
#include<stdio.h>
#include <stdbool.h>

extern void runFirstThread(void);
#define RUN_FIRST_THREAD 0x3
#define STACK_SIZE 0x400
#define MAX_STACK_SIZE 0x4000
uint32_t* MSP_INIT_VAL;
uint32_t* newStackptr;
uint32_t* currStackptr;

struct k_thread threads;

thread threadArray[NUMBER_OF_STACKS] = {0};

uint32_t activeThread = 0;
uint32_t numberOfThreads = 0;

void osKernelInitialize(void)
{
	//set the priority of PendSV to almost the weakest
	SHPR3 |= 0xFE << 16; //shift the constant 0xFE 16 bits to set PendSV priority
	SHPR2 |= 0xFDU << 24; //Set the priority of SVC higher than PendSV

	MSP_INIT_VAL = *(uint32_t**)0x0;
	newStackptr = MSP_INIT_VAL - STACK_SIZE;
	threads.sp = MSP_INIT_VAL;
}

uint32_t* Create_Stack(void)
{
	uint32_t* stackptr = (uint32_t*)((uint32_t*)newStackptr - STACK_SIZE);
	if( ((uint32_t*)MSP_INIT_VAL - (uint32_t*)stackptr) > MAX_STACK_SIZE )
		return NULL;
	newStackptr = stackptr;
	return (stackptr - STACK_SIZE);

}
void Run_First_Thread(void* fcn)
{
	currStackptr = Create_Stack();
	// Initializing Stack Context
	*(--currStackptr) = 1<<24;
	*(--currStackptr) = (uint32_t)fcn;
	for (int i = 0; i<14; i++)
		*(--currStackptr) = 0xA;
}

_Bool osCreateThread(void* fcn)
{
	uint32_t* threadStackptr = Create_Stack();
	if (threadStackptr == NULL)
		return false;

	*(--threadStackptr) = 1<<24;
		*(--threadStackptr) = (uint32_t)fcn;
		for (int i = 0; i<14; i++)
			*(--threadStackptr) = 0xA;

	threads.sp = threadStackptr;
	threads.thread_function = fcn;

	threadArray[numberOfThreads] = threads;
	numberOfThreads++;
	return true;
}

void osKernelStart(void)
{
	__asm("SVC #0x3");
}

void SVC_Handler_Main( unsigned int *svc_args )
{
	unsigned int svc_number;
	/*
	* Stack contains:
	* r0, r1, r2, r3, r12, r14, the return address and xPSR
	* First argument (r0) is svc_args[0]
	*/
	svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
	switch( svc_number )
	{
		case 0: //17 is sort of arbitrarily chosen
			printf("Success!\r\n");
			break;
		case 1:
			printf("ERROR DETECTED\r\n");
			break;
		case YIELD:
			_ICSR |= 1<<28;
			__asm("isb");
			break;
		case RUN_FIRST_THREAD:
			__set_PSP((uint32_t)threadArray[0].sp);
			runFirstThread();
			break;
		default: /* unknown SVC */
			break;
	 }
}
void osSched(){
	threadArray[activeThread].sp = (uint32_t*)(__get_PSP() - 8*4);
	activeThread = (activeThread+1)% numberOfThreads;
	__set_PSP((uint32_t)threadArray[activeThread].sp);
	return;
}
void osYield(void)
{
	__asm("SVC #0x2");
}
