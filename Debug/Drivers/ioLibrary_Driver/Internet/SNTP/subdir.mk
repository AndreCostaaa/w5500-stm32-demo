################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver/Internet/SNTP/sntp.c 

OBJS += \
./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.o 

C_DEPS += \
./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver/Internet/SNTP/%.o Drivers/ioLibrary_Driver/Internet/SNTP/%.su Drivers/ioLibrary_Driver/Internet/SNTP/%.cyclo: ../Drivers/ioLibrary_Driver/Internet/SNTP/%.c Drivers/ioLibrary_Driver/Internet/SNTP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I/home/andre/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/STM32L4xx_HAL_Driver/Inc -I/home/andre/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I/home/andre/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include -I/home/andre/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-SNTP

clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-SNTP:
	-$(RM) ./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.cyclo ./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.d ./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.o ./Drivers/ioLibrary_Driver/Internet/SNTP/sntp.su

.PHONY: clean-Drivers-2f-ioLibrary_Driver-2f-Internet-2f-SNTP

