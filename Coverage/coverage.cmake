# List of files to instrument for coverage
set(INSTRUMENTED_SOURCES
	main.c
	Controller/STM32F1xx/Peripheral/src/stm32f1xx_hal.c
	Controller/STM32F1xx/Peripheral/src/stm32f1xx_hal_gpio.c
)

# Add build options in order to activate coverage instrumentation for selected files
set_source_files_properties(
	${INSTRUMENTED_SOURCES}
	PROPERTIES COMPILE_FLAGS --coverage
)

# General coverage configuration
target_compile_options(${PROJECT_NAME} PRIVATE
	-fprofile-info-section
)
target_link_options(${PROJECT_NAME} PRIVATE
	--coverage
	-T${CMAKE_SOURCE_DIR}/Coverage/gcov_info.ld
)

# Register ".gcno" files as byproducts
list(TRANSFORM INSTRUMENTED_SOURCES PREPEND "CMakeFiles/${PROJECT_NAME}.dir/") # dirty...
list(TRANSFORM INSTRUMENTED_SOURCES APPEND ".gcno")
list(JOIN INSTRUMENTED_SOURCES " " INSTRUMENTED_SOURCES)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND true
	BYPRODUCTS ${INSTRUMENTED_SOURCES}
)