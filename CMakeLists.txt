cmake_minimum_required(VERSION 4.0)
project(Kraken_Cipher)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Kraken_Cipher
        crypto_helpers.cpp
        crypto_helpers.h
        main.cpp)

find_package(cryptopp-modern CONFIG REQUIRED)
target_link_libraries(Kraken_Cipher PRIVATE cryptopp::cryptopp)