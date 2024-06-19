################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.c 

OBJS += \
./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.o 

C_DEPS += \
./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/ioLibrary_Driver/Ethernet/W5200/%.o Drivers/ioLibrary_Driver/Ethernet/W5200/%.su Drivers/ioLibrary_Driver/Ethernet/W5200/%.cyclo: ../Drivers/ioLibrary_Driver/Ethernet/W5200/%.c Drivers/ioLibrary_Driver/Ethernet/W5200/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I/home/andre/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/STM32L4xx_HAL_Driver/Inc -I/home/andre/dev/viacam/calec/w5500-demo/Drivers/ioLibrary_Driver/Ethernet -I/home/andre/dev/viacam/calec/w5500-demo/Drivers/ioLibrary_Driver/Internet/DHCP -I/home/andre/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I/home/andre/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/CMSIS/Device/ST/STM32L4xx/Include -I/home/andre/STM32Cube/Repository/STM32Cube_FW_L4_V1.18.0/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet-2f-W5200

clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet-2f-W5200:
	-$(RM) ./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.cyclo ./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.d ./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.o ./Drivers/ioLibrary_Driver/Ethernet/W5200/w5200.su

.PHONY: clean-Drivers-2f-ioLibrary_Driver-2f-Ethernet-2f-W5200

