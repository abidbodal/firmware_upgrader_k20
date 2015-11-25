################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/flash/flash_kinetis.c" \

C_SRCS += \
../Sources/flash/flash_kinetis.c \

OBJS += \
./Sources/flash/flash_kinetis_c.obj \

OBJS_QUOTED += \
"./Sources/flash/flash_kinetis_c.obj" \

C_DEPS += \
./Sources/flash/flash_kinetis_c.d \

C_DEPS_QUOTED += \
"./Sources/flash/flash_kinetis_c.d" \

OBJS_OS_FORMAT += \
./Sources/flash/flash_kinetis_c.obj \


# Each subdirectory must supply rules for building sources it contributes
Sources/flash/flash_kinetis_c.obj: ../Sources/flash/flash_kinetis.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"Sources/flash/flash_kinetis.args" -o "Sources/flash/flash_kinetis_c.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

Sources/flash/%.d: ../Sources/flash/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '


