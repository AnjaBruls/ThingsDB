################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../inc/lib/http_parser.c 

OBJS += \
./inc/lib/http_parser.o 

C_DEPS += \
./inc/lib/http_parser.d 


# Each subdirectory must supply rules for building sources it contributes
inc/lib/%.o: ../inc/lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -std=gnu11 -DNDEBUG -I../inc -O3 -Wall -Wextra -Winline -c -fmessage-length=0 -msse4.1 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

