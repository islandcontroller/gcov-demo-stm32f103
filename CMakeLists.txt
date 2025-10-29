cmake_minimum_required(VERSION 3.20)

# Project setup
project(template-stm32f1xx # <-------------------------------------------------- set your project name here
	LANGUAGES C ASM
)

# Filenames configuration
set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(CMAKE_HEXFILE_SUFFIX ".hex")
set(CMAKE_LISTING_SUFFIX ".lst")
set(CMAKE_MAPFILE_SUFFIX ".map")

# Language configuration
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Override default flags
set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_ASM_FLAGS_DEBUG "")
set(CMAKE_ASM_FLAGS_RELEASE "")

# Output target
add_executable(${PROJECT_NAME})

# Source files (exclude build outputs and file templates)
file(GLOB_RECURSE TARGET_SOURCES *.c *.S)
list(FILTER TARGET_SOURCES EXCLUDE REGEX "build\/.*")
list(FILTER TARGET_SOURCES EXCLUDE REGEX "Controller\/.*\/Template\/.*")
target_sources(${PROJECT_NAME} PRIVATE ${TARGET_SOURCES})

# Include paths
target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_SOURCE_DIR}
	Controller
	Controller/STM32F1xx
	Controller/STM32F1xx/Core
	Controller/STM32F1xx/Peripheral/inc
	# <--------------------------------------------------------------------- add your additional includes here
)

# Common compiler/linker settings
set(MACHINE_OPTIONS
	-march=armv7-m
	-mcpu=cortex-m3
	-mthumb
)

# Compiler configuration
target_compile_definitions(${PROJECT_NAME} PRIVATE
	-DSTM32F103xB # <------------------------------------------------------- change to match your MCU type
)
target_compile_options(${PROJECT_NAME} PRIVATE
	${MACHINE_OPTIONS}
		
	-fdata-sections
	-ffunction-sections
	-flto
	
	-Wall
	-Wextra

	$<$<CONFIG:Debug>:-O1 -g> # <------------------------------------------- set optimization and config flags here (Debug/Release)
	$<$<CONFIG:Release>:-O2>
)

# Linker configuration
set(LINKER_FILE Controller/STM32F1xx/linker_script_stm32f103x8.ld) # <---------- change to match your MCU type
cmake_path(REMOVE_FILENAME LINKER_FILE OUTPUT_VARIABLE LINKER_BASEDIR)
cmake_path(GET LINKER_FILE FILENAME LINKER_FILE)
target_link_directories(${PROJECT_NAME} PRIVATE	${LINKER_BASEDIR})
target_link_options(${PROJECT_NAME} PRIVATE
	${MACHINE_OPTIONS}
	
	-T${LINKER_FILE}

	-specs=nano.specs
	-specs=nosys.specs
	-nostartfiles

	-Wl,--gc-sections
	-Wl,--print-memory-usage
	-Wl,-Map=${PROJECT_NAME}${CMAKE_MAPFILE_SUFFIX},--cref
)
target_link_libraries(${PROJECT_NAME}
	PUBLIC c # libc
	PUBLIC m # libm (math)
	# <--------------------------------------------------------------------- adjust these to your needs
)

# Post-Build: register generated mapfile
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND true
	BYPRODUCTS ${PROJECT_NAME}${CMAKE_MAPFILE_SUFFIX}
)

# Post-Build: print section sizes
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_SIZE_UTIL} ${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}
)

# Post-Build: generate listings
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_OBJDUMP} -d -S ${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX} > ${PROJECT_NAME}${CMAKE_LISTING_SUFFIX}
	BYPRODUCTS ${PROJECT_NAME}${CMAKE_LISTING_SUFFIX}
)

# Post-Build: generate HEX file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX} ${PROJECT_NAME}${CMAKE_HEXFILE_SUFFIX}
)