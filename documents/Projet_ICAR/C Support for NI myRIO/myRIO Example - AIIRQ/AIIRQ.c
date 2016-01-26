/*
 * Configuration for Analog Input Interrupt Request (IRQ)
 *
 * Copyright (c) 2014,
 * National Instruments.
 * All rights reserved.
 */
#include <stdio.h>

/*
 * Include the myRIO header file.
 * The target type must be defined in your project, as a stand-alone #define,
 * or when calling the compiler from the command line.
 */
#include "MyRio.h"
#include "AIIRQ.h"


/*
 * Define some normal macros to ensure the input is within limitation.
 */
#if !defined(THRESHOLD_MAX)
#define THRESHOLD_MAX 5
#endif

#if !defined(THRESHOLD_MIN)
#define THRESHOLD_MIN 0
#endif

#if !defined(HYSTERESIS_MAX)
#define HYSTERESIS_MAX 1
#endif

#if !defined(HYSTERESIS_MIN)
#define HYSTERESIS_MIN 0
#endif


/*
 * Declare the myRIO NiFpga_Session so that it can be used by any function in
 * this file. The variable is actually defined in myRIO.c.
 *
 * This removes the need to pass the myrio_session around to every function and
 * only has to be declared when it is being used.
 */
extern NiFpga_Session myrio_session;


/**
 * Reserve the interrupt from FPGA and configure AI IRQ.
 *
 * @param[in]  irqChannel   A structure containing the registers and settings
                                for a particular analog IRQ I/O to modify.
 * @param[in]  irqContext   IRQ context under which you reserve the IRQ.
 * @param[in]  irqNumber    The IRQ number (IRQNO_MIN - IRQNO_MAX).
 * @param[in]  threshold    The triggering voltage threshold (THRESHOLD_MIN - THRESHOLD_MAX).
 * @param[in]  hysteresis   The hysteresis (HYSTERESIS_MIN - HYSTERESIS_MAX) helps present
 *                              multiple triggers due to noise.
 * @param[in]  type         Trigger type.
 * @return  the configuration status.
 */
int32_t Irq_RegisterAiIrq(MyRio_IrqAi* irqChannel, 
                          NiFpga_IrqContext* irqContext,
                          uint8_t irqNumber, 
                          double threshold,
                          double hysteresis, 
                          Irq_Ai_Type type)
{
    int32_t status;
    uint8_t cnfgValue;
    uint16_t thresholdScaled;
    uint16_t hysteresisScaled;

    /*
     * Reserve an IRQ context. IRQ contexts are single-threaded; only one thread
     * can wait with a particular context at any given time. To minimize jitter
     * when first waiting on IRQs, reserve as many contexts as the application requires.
     * If a context is successfully reserved, you must unreserve it later.
     * Otherwise a memory leak will occur.
     */
    status = NiFpga_ReserveIrqContext(myrio_session, irqContext);

    /*
     * Check if there was an error when you reserve an IRQ.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    MyRio_ReturnStatusIfNotSuccess(status,
            "A required NiFpga_IrqContext was not reserved.")

    /*
     * Limit the IRQ number within a range,
     * if the entered value is out of range, print an error message.
     */
    if (irqNumber > IRQNO_MAX || irqNumber < IRQNO_MIN)
    {
        printf("The specified IRQ Number is out of range.\n");
        return NiMyrio_Status_IrqNumberNotUsable;
    }

    /*
     * Check if the IRQ number or channel value already existed in the resource list,
     * return configuration status, and print an error message.
     */
    status = Irq_CheckReserved(irqChannel->aiChannel, irqNumber);
    if (status == NiMyrio_Status_IrqNumberNotUsable)
    {
        printf("You have already registered an interrupt with the same interrupt number.\n");
        return status;
    }
    else if (status == NiMyrio_Status_IrqChannelNotUsable)
    {
        printf("You have already registered an interrupt with the same channel name.\n");
        return status;
    }

    /*
     * Write the value to the AI IRQ number register.
     */
    status = NiFpga_WriteU8(myrio_session, irqChannel->aiIrqNumber, irqNumber);

    /*
     * Check if there was an error when you wrote to the AI IRQ number register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    MyRio_ReturnStatusIfNotSuccess(status,
            "Could not write to AI IRQ number register!")

    /*
     * Coerce the threshold within THRESHOLD_MIN to THRESHOLD_MAX,
     * and coerce the hysteresis within HYSTERESIS_MIN to HYSTERESIS_MAX.
     */
    if (threshold > THRESHOLD_MAX)
    {
        threshold = THRESHOLD_MAX;
    }
    else if (threshold < THRESHOLD_MIN)
    {
        threshold = THRESHOLD_MIN;
    }
    if (hysteresis > HYSTERESIS_MAX)
    {
        hysteresis = HYSTERESIS_MAX;
    }
    else if (hysteresis < HYSTERESIS_MIN)
    {
        hysteresis = HYSTERESIS_MIN;
    }

    /*
     * Initialize the scaled value in the structure.
     */
    Aio_Scaling(&irqChannel->aiScaling);

    /*
     * Scale the voltage value to the raw value.
     */
    threshold = (threshold - irqChannel->aiScaling.scale_offset)
            / irqChannel->aiScaling.scale_weight + 0.5;
    thresholdScaled = (uint16_t) threshold;

    /*
     * Write the value to the AI threshold register.
     */
    status = NiFpga_WriteU16(myrio_session, irqChannel->aiThreshold, thresholdScaled);

    /*
     * Check if there was an error when you wrote to the AI threshold register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    MyRio_ReturnStatusIfNotSuccess(status,
            "Could not write to AI threshold register!")

    /*
     * Scale the voltage value to the raw value.
     */
    hysteresis = (hysteresis - irqChannel->aiScaling.scale_offset)
            / irqChannel->aiScaling.scale_weight + 0.5;
    hysteresisScaled = (uint16_t) hysteresis;

    /*
     * Write the value to the AI hysteresis register.
     */
    status = NiFpga_WriteU16(myrio_session, irqChannel->aiHysteresis, hysteresisScaled);

    /*
     * Check if there was an error when you wrote to the AI threshold register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    MyRio_ReturnStatusIfNotSuccess(status,
            "Could not write to AI hysteresis register!")

    /*
     * Get the current value of the AI configure register.
     */
    status = NiFpga_ReadU8(myrio_session, irqChannel->aiIrqConfigure, &cnfgValue);

    /*
     * Check if there was an error when you wrote to the AI threshold register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    MyRio_ReturnStatusIfNotSuccess(status,
            "Could not read from the AI configure register!")

    /*
     * Configure the IRQ triggered-type for the particular analog IRQ I/O.
     */
    if (irqChannel->aiChannel == Irq_Ai_A0)
    {
        /*
         * Clear the value of the masked bits in the AI configure register. This is
         * done so that the correct value can be set later on.
         */
        cnfgValue = cnfgValue & (~Irq_Ai_A0_Enable) & (~Irq_Ai_A0_Type);

        /*
         * Configure the value of the settings in the AI configure register. If the
         * value to set is 0, this operation would not work unless the bit was
         * previously cleared. This is done so the triggered type is configured.
         */
        if (type == Irq_Ai_RisingEdge)
        {
            cnfgValue = cnfgValue | Irq_Ai_A0_Enable | Irq_Ai_A0_Type;
        }
        else if (type == Irq_Ai_FallingEdge)
        {
            cnfgValue = cnfgValue | Irq_Ai_A0_Enable;
        }
    }
    else if (irqChannel->aiChannel == Irq_Ai_A1)
    {
        /*
         * Clear the value of the masked bits in the AI configure register. This is
         * done so that the correct value can be set later on.
         */
        cnfgValue = cnfgValue & (~Irq_Ai_A1_Enable) & (~Irq_Ai_A1_Type);

        /*
         * Configure the value of the settings in the AI configure register. If the
         * value to set is 0, this operation would not work unless the bit was
         * previously cleared. This is done so the triggered type is configured.
         */
        if (type == Irq_Ai_RisingEdge)
        {
            cnfgValue = cnfgValue | Irq_Ai_A1_Enable | Irq_Ai_A1_Type;
        }
        else if (type == Irq_Ai_FallingEdge)
        {
            cnfgValue = cnfgValue | Irq_Ai_A1_Enable;
        }
    }

    /*
     * Write the new value of the AI configure register to the device.
     */
    status = NiFpga_WriteU8(myrio_session, irqChannel->aiIrqConfigure, cnfgValue);

    /*
     * Check if there was an error when you wrote to the AI threshold register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    MyRio_ReturnStatusIfNotSuccess(status,
            "Could not write to the AI configure register!")

    /*
     * Add the channel value and IRQ number in the list.
     */
    Irq_AddReserved(irqChannel->aiChannel, irqNumber);

    return NiMyrio_Status_Success;
}


/**
 * Clear the AI IRQ configuration.
 *
 * @param[in]  irqChannel       A structure containing the registers and settings
 *                                 for a particular analog IRQ I/O to modify.
 * @param[in]  irqContext       IRQ context under which to clear IRQ.
 * @param[in]  irqNumber        IRQ number that you need to clear
 * @return the configuration status.
 */
int32_t Irq_UnregisterAiIrq(MyRio_IrqAi* irqChannel, 
                            NiFpga_IrqContext irqContext,
                            uint8_t irqNumber)
{
    int32_t status;
    uint8_t cnfgValue;

    /*
     * Limit the IRQ number within a range,
     * if the entered value is out of range, print an error message.
     */
    if (irqNumber > IRQNO_MAX || irqNumber < IRQNO_MIN)
    {
        printf("The specified IRQ Number is out of range.\n");
        return NiMyrio_Status_IrqNumberNotUsable;
    }

    /*
     * Check if the specified IRQ resource is registered.
     */
    status = Irq_CheckReserved(irqChannel->aiChannel, irqNumber);
    if (status == NiMyrio_Status_Success)
    {
        /*
         * Did not find the resource in the list.
         */
        printf("You didn't register an interrupt with this IRQ number.\n");
        return NiMyrio_Status_Success;
    }

    /*
     * Get the current value of the AI configure register.
     */
    status = NiFpga_ReadU8(myrio_session, irqChannel->aiIrqConfigure, &cnfgValue);

    /*
     * Check if there was an error when you wrote to the AI threshold register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    MyRio_ReturnStatusIfNotSuccess(status,
            "Could not read from the AI configure register!")

    /*
     * Disable the specified channel.
     */
    if (irqChannel->aiChannel == Irq_Ai_A0)
    {
        /*
         * Clear the value of the masked bits in the AI configure register. This is
         * done so AI0 is disabled.
         */
        cnfgValue = cnfgValue & (~Irq_Ai_A0_Enable);
    }
    else if (irqChannel->aiChannel == Irq_Ai_A1)
    {
        /*
         * Clear the value of the masked bits in the AI configure register. This is
         * done so AI1 is disabled.
         */
        cnfgValue = cnfgValue & (~Irq_Ai_A1_Enable);
    }

    /*
     * Write the new value of the AI configure register to the device.
     */
    status = NiFpga_WriteU8(myrio_session, irqChannel->aiIrqConfigure, cnfgValue);

    /*
     * Check if there was an error when you wrote to the AI threshold register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    MyRio_ReturnStatusIfNotSuccess(status,
            "Could not write to the AI configure register!")

    /*
     * Remove the reserved resource in the list.
     */
    status = Irq_RemoveReserved(irqNumber);
    /*
     * Check if there was an error when you wrote to the AI threshold register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    MyRio_ReturnStatusIfNotSuccess(status,
            "Could not release the irq resource!");

    /*
     * Unreserve an IRQ context obtained from Irq_ReserveIrqContext.
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_UnreserveIrqContext(myrio_session, irqContext);

    /*
     * Check if there was an error when you wrote to the AI threshold register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    MyRio_ReturnStatusIfNotSuccess(status,
            "A required NiFpga_IrqContext was not unreserved.")

    return NiMyrio_Status_Success;
}
