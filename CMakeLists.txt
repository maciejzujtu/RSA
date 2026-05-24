cmake_minimum_required(VERSION 3.16)
project(rsa C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_path(GMP_INCLUDE_DIR   NAMES gmp.h)
find_library(GMP_LIBRARY    NAMES gmp)

if(NOT GMP_INCLUDE_DIR OR NOT GMP_LIBRARY)
    message(FATAL_ERROR "GMP Library not found.")
endif()


add_library(rsa_core STATIC
    src/algorithms/mod_exp.c
    src/algorithms/extended_gcd.c
    src/algorithms/miller_rabin.c
    src/rsa/errors.c
    src/rsa/keygen.c
    src/rsa/rsa_core.c
    src/rsa/pkcs1v15.c
)

target_include_directories(rsa_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${GMP_INCLUDE_DIR}
)

target_link_libraries(rsa_core PUBLIC ${GMP_LIBRARY})

add_executable(rsa src/main.c)
target_link_libraries(rsa PRIVATE rsa_core)