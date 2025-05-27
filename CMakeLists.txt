cmake_minimum_required(VERSION 3.20.0)
project(WebServerMainstreaming VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set OpenSSL paths for Windows
if(WIN32)
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64") # This is the default installation path when installing OpenSSL binaries on windows
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
    
    # For Visual Studio, configure both Debug and Release
    if(MSVC)
        set(OPENSSL_SSL_LIBRARY_DEBUG "${OPENSSL_ROOT_DIR}/lib/VC/x64/MDd/libssl.lib")
        set(OPENSSL_CRYPTO_LIBRARY_DEBUG "${OPENSSL_ROOT_DIR}/lib/VC/x64/MDd/libcrypto.lib")
        set(OPENSSL_SSL_LIBRARY_RELEASE "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD/libssl.lib")
        set(OPENSSL_CRYPTO_LIBRARY_RELEASE "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD/libcrypto.lib")
        
        # Default to release if not explicitly debug
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(OPENSSL_SSL_LIBRARY "${OPENSSL_SSL_LIBRARY_DEBUG}")
            set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_CRYPTO_LIBRARY_DEBUG}")
        else()
            set(OPENSSL_SSL_LIBRARY "${OPENSSL_SSL_LIBRARY_RELEASE}")
            set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_CRYPTO_LIBRARY_RELEASE}")
        endif()
    else()
        # Non-MSVC compiler, just use the MD libraries
        set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD/libssl.lib")
        set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD/libcrypto.lib")
    endif()
    
    # Debug output
    message(STATUS "Using OpenSSL libs:")
    message(STATUS "SSL library: ${OPENSSL_SSL_LIBRARY}")
    message(STATUS "Crypto library: ${OPENSSL_CRYPTO_LIBRARY}")
    
    # Check if files exist
    if(NOT EXISTS "${OPENSSL_SSL_LIBRARY}")
        message(FATAL_ERROR "SSL library file not found: ${OPENSSL_SSL_LIBRARY}")
    endif()
    
    if(NOT EXISTS "${OPENSSL_CRYPTO_LIBRARY}")
        message(FATAL_ERROR "Crypto library file not found: ${OPENSSL_CRYPTO_LIBRARY}")
    endif()
    
    # Create imported targets manually instead of using find_package
    add_library(SSL STATIC IMPORTED)
    set_property(TARGET SSL PROPERTY IMPORTED_LOCATION "${OPENSSL_SSL_LIBRARY}")
    
    add_library(Crypto STATIC IMPORTED)
    set_property(TARGET Crypto PROPERTY IMPORTED_LOCATION "${OPENSSL_CRYPTO_LIBRARY}")
    
    # Define the OpenSSL targets that we'd normally get from find_package
    add_library(OpenSSL::SSL ALIAS SSL)
    add_library(OpenSSL::Crypto ALIAS Crypto)
    
    # Skip find_package since we're creating our own targets
else()
    # For non-Windows, use the standard find_package
    find_package(OpenSSL REQUIRED)
endif()

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(
    src
    third_party
    ${OPENSSL_INCLUDE_DIR}
)

# Link against OpenSSL libraries
target_link_libraries(${PROJECT_NAME} 
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Copy DLL files to the output directory (Windows only)
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OPENSSL_ROOT_DIR}/libssl-3-x64.dll"
            "${OPENSSL_ROOT_DIR}/libcrypto-3-x64.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()