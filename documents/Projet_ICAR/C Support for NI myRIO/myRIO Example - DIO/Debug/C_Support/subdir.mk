################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/MyRio/C\ Support\ for\ NI\ myRIO/source/IRQConfigure.c \
C:/MyRio/C\ Support\ for\ NI\ myRIO/source/MyRio.c \
C:/MyRio/C\ Support\ for\ NI\ myRIO/source/NiFpga.c 

OBJS += \
./C_Support/IRQConfigure.o \
./C_Support/MyRio.o \
./C_Support/NiFpga.o 

C_DEPS += \
./C_Support/IRQConfigure.d \
./C_Support/MyRio.d \
./C_Support/NiFpga.d 


# Each subdirectory must supply rules for building sources it contributes
C_Support/IRQConfigure.o: C:/MyRio/C\ Support\ for\ NI\ myRIO/source/IRQConfigure.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-nilrt-linux-gnueabi-gcc -DMyRio_1900 -I"C:\MyRio\C Support for NI myRIO\source" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

C_Support/MyRio.o: C:/MyRio/C\ Support\ for\ NI\ myRIO/source/MyRio.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-nilrt-linux-gnueabi-gcc -DMyRio_1900 -I"C:\MyRio\C Support for NI myRIO\source" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

C_Support/NiFpga.o: C:/MyRio/C\ Support\ for\ NI\ myRIO/source/NiFpga.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-nilrt-linux-gnueabi-gcc -DMyRio_1900 -I"C:\MyRio\C Support for NI myRIO\source" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


