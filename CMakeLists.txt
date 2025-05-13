# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(ControleDeEnergiaPorSetor C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(${CMAKE_SOURCE_DIR}/lib)

# Add executable. Default name is the project name, version 0.1

add_executable(ControleDeEnergiaPorSetor ControleDeEnergiaPorSetor.c lib/leds.c lib/ssd1306.c )# Biblioteca para o display OLED

pico_set_program_name(ControleDeEnergiaPorSetor "ControleDeEnergiaPorSetor")
pico_set_program_version(ControleDeEnergiaPorSetor "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(ControleDeEnergiaPorSetor 0)
pico_enable_stdio_usb(ControleDeEnergiaPorSetor 1)

# Add the standard library to the build
target_link_libraries(ControleDeEnergiaPorSetor
        hardware_pio
        hardware_pwm
        hardware_i2c
        hardware_clocks
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(ControleDeEnergiaPorSetor PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${PICO_SDK_PATH}/lib/lwip/src/include
        ${PICO_SDK_PATH}/lib/lwip/src/include/arch
        ${PICO_SDK_PATH}/lib/lwip/src/include/lwip
)

target_sources(ControleDeEnergiaPorSetor PRIVATE
    ${PICO_SDK_PATH}/lib/lwip/src/apps/http/httpd.c
    ${PICO_SDK_PATH}/lib/lwip/src/apps/http/fs.c
)


# Generate PIO header
pico_generate_pio_header(ControleDeEnergiaPorSetor ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)


pico_add_extra_outputs(ControleDeEnergiaPorSetor)

