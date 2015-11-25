################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/rs232/rs232.c" \

C_SRCS += \
../Sources/rs232/rs232.c \

OBJS += \
./Sources/rs232/rs232_c.obj \

OBJS_QUOTED += \
"./Sources/rs232/rs232_c.obj" \

C_DEPS += \
./Sources/rs232/rs232_c.d \

C_DEPS_QUOTED += \
"./Sources/rs232/rs232_c.d" \

OBJS_OS_FORMAT += \
./Sources/rs232/rs232_c.obj \


# Each subdirectory must supply rules for building sources it contributes
Sources/rs232/rs232_c.obj: ../Sources/rs232/rs232.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: ARM Compiler'
	"$(ARM_ToolsDirEnv)/mwccarm" -gccinc @@"Sources/rs232/rs232.args" -o "Sources/rs232/rs232_c.obj" -c "$<" -MD -gccdep
	@echo 'Finished building: $<'
	@echo ' '

Sources/rs232/%.d: ../Sources/rs232/%.c
	@echo 'Regenerating dependency file: $@'
	
	@echo ' '


