################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/audio_capture.c \
../src/beatvisualizer.c \
../src/fft_processing.c \
../src/shared_data.c 

C_DEPS += \
./src/audio_capture.d \
./src/beatvisualizer.d \
./src/fft_processing.d \
./src/shared_data.d 

OBJS += \
./src/audio_capture.o \
./src/beatvisualizer.o \
./src/fft_processing.o \
./src/shared_data.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	aarch64-linux-gnu-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/audio_capture.d ./src/audio_capture.o ./src/beatvisualizer.d ./src/beatvisualizer.o ./src/fft_processing.d ./src/fft_processing.o ./src/shared_data.d ./src/shared_data.o

.PHONY: clean-src

