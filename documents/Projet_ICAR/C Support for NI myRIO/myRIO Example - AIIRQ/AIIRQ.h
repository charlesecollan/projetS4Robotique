/*
 * AIIRQ.h
 *
 * Copyright (c) 2014,
 * National Instruments.
 * All rights reserved.
 */

#ifndef AIIRQ_h_
#define AIIRQ_h_

#include "AIO.h"
#include "IRQConfigure.h"

#if NiFpga_Cpp
extern "C" {
#endif


/**
 * Flags that indicate whether the AI IRQ pin is triggered by the rising or falling edge.
 */
typedef enum
{
    Irq_Ai_RisingEdge,            /**< IRQ is triggered by the rising edge */
    Irq_Ai_FallingEdge            /**< IRQ is triggered by the falling edge */
} Irq_Ai_Type;


/**
 * Flags that indicate and configure AI IRQ.
 */
typedef enum
{
    Irq_Ai_A0_Enable = 0x01,       /**< The AI0 IRQ enable option */
    Irq_Ai_A1_Enable = 0x04,       /**< The AI1 IRQ enable option */
    Irq_Ai_A0_Type = 0x02,         /**< The AI0 IRQ triggered type option */
    Irq_Ai_A1_Type = 0x08          /**< The AI1 IRQ triggered type option */
} Irq_Ai_Configure;


/**
 * Registers and settings for a particular AI IRQ.
 */
typedef struct
{
    uint32_t aiIrqNumber;         /**< AI IRQ number register */
    uint32_t aiHysteresis;        /**< AI IRQ hysteresis register */
    uint32_t aiThreshold;         /**< AI IRQ threshold register */
    uint32_t aiIrqConfigure;      /**< AI IRQ enable and trigger type
                                      configuration register */
    MyRio_Aio aiScaling;          /**< Structure consists of registers
                                      and settings for a particular 
                                      analog I/O */
    Irq_Channel aiChannel;        /**< AI IRQ supported I/O */
} MyRio_IrqAi;


/**
 * Configure the AI IRQ number, threshold and hysteresis and trigger type.
 */
int32_t Irq_RegisterAiIrq(MyRio_IrqAi* irqChannel,
                          NiFpga_IrqContext* irqContext,
                          uint8_t irqNumber, 
                          double threshold,
                          double hysteresis, 
                          Irq_Ai_Type type);


/**
 * Disable the IRQ configuration on the specified IRQ channel and number.
 */
int32_t Irq_UnregisterAiIrq(MyRio_IrqAi* irqChannel,
                             NiFpga_IrqContext irqContext,
                             uint8_t irqNumber);

#if NiFpga_Cpp
}
#endif

#endif /* AIIRQ_h_ */
