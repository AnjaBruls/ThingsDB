################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/util/argparse.c \
../src/util/cfgparser.c \
../src/util/cryptx.c \
../src/util/ex.c \
../src/util/fx.c \
../src/util/imap.c \
../src/util/lock.c \
../src/util/logger.c \
../src/util/qpx.c \
../src/util/queue.c \
../src/util/smap.c \
../src/util/strx.c \
../src/util/vec.c \
../src/util/write.c 

OBJS += \
./src/util/argparse.o \
./src/util/cfgparser.o \
./src/util/cryptx.o \
./src/util/ex.o \
./src/util/fx.o \
./src/util/imap.o \
./src/util/lock.o \
./src/util/logger.o \
./src/util/qpx.o \
./src/util/queue.o \
./src/util/smap.o \
./src/util/strx.o \
./src/util/vec.o \
./src/util/write.o 

C_DEPS += \
./src/util/argparse.d \
./src/util/cfgparser.d \
./src/util/cryptx.d \
./src/util/ex.d \
./src/util/fx.d \
./src/util/imap.d \
./src/util/lock.d \
./src/util/logger.d \
./src/util/qpx.d \
./src/util/queue.d \
./src/util/smap.d \
./src/util/strx.d \
./src/util/vec.d \
./src/util/write.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/%.o: ../src/util/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DDEBUG -I"/home/joente/workspace/rqldb/inc" -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


