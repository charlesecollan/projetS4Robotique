/*
 * Copyright (c) 2014,
 * National Instruments.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "DIIRQ.h"

#if !defined(LoopDuration)
#define LoopDuration          60   /* How long to monitor the signal, in seconds */
#endif


#if !defined(LoopSteps)
#define LoopSteps             3    /* How long to step between printing, in seconds */
#endif


/*
 * Resources for the new thread.
 */
typedef struct
{
    NiFpga_IrqContext irqContext;      /* IRQ context reserved by Irq_ReserveContext() */
    NiFpga_Bool irqThreadRdy;           /* IRQ thread ready flag */
    uint8_t irqNumber;                 /* IRQ number value */
} ThreadResource;


/**
 * Overview:
 * Demonstrates how to use the DI IRQ. Once the DI IRQ occurs, print
 * the IRQ number, trigger times and main loop count number in the console.
 * The main thread runs for 60 s.
 *
 * Instructions:
 * 1. Connect a signal such as square wave to DIO0(pin11) on connector A.
 * 2. Run this program and observe the console.
 *
 * Output:
 * The IRQ2, triggered times and main loop count number.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the NI myRIO-1900.
 * Change the preprocessor symbol if you want to use this example with the NI myRIO-1950.
 */
void *DI_Irq_Thread(void* resource)
{
    ThreadResource* threadResource = (ThreadResource*) resource;
    while (1)
    {
        uint32_t irqAssert = 0;
        static uint32_t irqCount = 0;

        /*
         * Stop the calling thread, wait until a selected IRQ is asserted.
         */
        Irq_Wait(threadResource->irqContext, threadResource->irqNumber,
                 &irqAssert, (NiFpga_Bool*) &(threadResource->irqThreadRdy));

        /*
         * If an IRQ was asserted.
         */
        if (irqAssert & (1 << threadResource->irqNumber))
        {
            printf("IRQ%d,%d\n", threadResource->irqNumber, ++irqCount);

            /*
             * Acknowledge the IRQ(s) when the assertion is done.
             */
            Irq_Acknowledge(irqAssert);
        }

        /*
         * Check the indicator to see if the new thread is stopped.
         */
        if (!(threadResource->irqThreadRdy))
        {
            printf("The IRQ thread ends.\n");
            break;
        }
    }

    /*
     * Exit the new thread.
     */
    pthread_exit(NULL);

    return NULL;
}


int main(int argc, char **argv)
{
    int32_t status;

    MyRio_IrqDi irqDI0;
    ThreadResource irqThread0;

    pthread_t thread;

    time_t currentTime;
    time_t finalTime;
    time_t printTime;

    /*
     * Configure the DI IRQ number, incremental times, and trigger type.
     */
    const uint8_t IrqNumber = 2;
    const uint32_t Count = 2;
    const Irq_Dio_Type TriggerType = Irq_Dio_Edge;

    printf("Digital Input IRQ:\n");

    /*
     * Specify the settings that correspond to the IRQ channel
     * that needs to be accessed.
     */
    irqDI0.dioChannel = Irq_Dio_A0;
    irqDI0.dioIrqNumber = IRQDIO_A_0NO;
    irqDI0.dioCount = IRQDIO_A_0CNT;
    irqDI0.dioIrqRisingEdge = IRQDIO_A_70RISE;
    irqDI0.dioIrqFallingEdge = IRQDIO_A_70FALL;
    irqDI0.dioIrqEnable = IRQDIO_A_70ENA;

    /*
     * Initiate the IRQ number resource of the new thread.
     */
    irqThread0.irqNumber = IrqNumber;

    /*
     * Open the myRIO NiFpga Session.
     * You must use this function before using all the other functions. 
     * After you finish using this function, the NI myRIO target is ready to be used.
     */
    status = MyRio_Open();
    if (MyRio_IsNotSuccess(status))
    {
        return status;
    }

    /*
     * Configure the DI0 IRQ and return a status message to indicate if the configuration is successful, 
     * the error code is defined in IRQConfigure.h.
     */
    status = Irq_RegisterDiIrq(&irqDI0, &(irqThread0.irqContext),
                                       IrqNumber, Count,
                                       TriggerType);

    /*
     * Terminate the process if it is not successful.
     */
    if (status != NiMyrio_Status_Success)
    {
        printf("CONFIGURE ERROR: %d\n", status);
        printf("Configuration of DI IRQ failed.");
        return status;
    }

    /*
     * Set the indicator to allow the new thread.
     */
    irqThread0.irqThreadRdy = NiFpga_True;

    /*
     * Create new threads to catch the specified IRQ numbers.
     * Different IRQs should have different corresponding threads.
     */
    status = pthread_create(&thread, NULL, DI_Irq_Thread, &irqThread0);
    if (status != NiMyrio_Status_Success)
    {
        printf("CONFIGURE ERROR: %d\n", status);
        printf("Failed to create a new thread!");
        return status;
    }

    /*
     * Normally, the main function runs a long running or infinite loop.
     * Read the console output for 60 seconds so that you can recognize the
     * explanation and loop times.
     */
    time(&currentTime);
    finalTime = currentTime + LoopDuration;
    printTime = currentTime;
    while (currentTime < finalTime)
    {
        static uint32_t loopCount = 0;
        time(&currentTime);

        /* Don't print every loop iteration. */
        if (currentTime > printTime)
        {
            printf("main loop,%d\n", ++loopCount);

            printTime += LoopSteps;
        }
    }

    /*
     * Set the indicator to end the new thread.
     */
    irqThread0.irqThreadRdy = NiFpga_False;

    /*
     * Wait the end of the IRQ thread.
     */
    pthread_join(thread, NULL);

    /*
     * Distable DI0, so you can configure this I/O next time.
     * Every IrqConfigure() function should have its corresponding clear function,
     * and their parameters should also match.
     */
    status = Irq_UnregisterDiIrq(&irqDI0, irqThread0.irqContext,
                                            IrqNumber);
    if (status != NiMyrio_Status_Success)
    {
        printf("CONFIGURE ERROR: %d\n", status);
        printf("Clear configuration of DI IRQ failed.");
        return status;
    }

    /*
     * Close the myRIO NiFpga Session.
     * You must use this function after using all the other functions.
     */
    status = MyRio_Close();

    /*
     * Returns 0 if successful.
     */
    return status;
}
