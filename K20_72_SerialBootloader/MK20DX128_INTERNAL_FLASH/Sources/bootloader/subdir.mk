################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/bootloader/bootloader.c" \

C_SRCS += \
../Sources/bootloader/bootloader.c \

OBJS += \
./Sources/bootloader/bootloader_c.obj \

OBJS_QUOTED += \
"./Sources/bootloader/bootloader_c.obj" \

C_DEPS += \
./Sources/bootloader/bootloader_c.d \

C_DEPS_QUOTED += \
"./Sources/bootloader/bootloader_c.d" \

OBJS_OS_FORMAT += \
./Sources/bootloader/bootloader_c.obj \


# Each subdirectory must supply rules for building sources it contributes
Sources/bootloader/bootloader_c.obj: ../Sources/bootloader/bootloader.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"Sources/bootloader/bootloader.args" -o "Sources/bootloader/bootloader_c.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

Sources/bootloader/%.d: ../Sources/bootloader/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '


