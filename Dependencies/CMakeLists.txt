#to download libraries
include(FetchContent)

FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.3)

FetchContent_MakeAvailable(Catch2)



add_subdirectory(SAIS)
add_subdirectory(LZW)