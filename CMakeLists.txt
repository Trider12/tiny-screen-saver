cmake_minimum_required(VERSION 3.29)

project(tss)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

STRING (REGEX REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG}) # disable RTC, CMake turns it on by default

option(USE_CRINKLER "Use crinckler.exe instead of link.exe as a linker" ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(USE_CRINKLER OFF) # Visual Studio can't debug crinkler binaries
endif()

if(USE_CRINKLER)
    # must set the linker before add_executable()
    set(CMAKE_LINKER_TYPE crinkler)
    set(CMAKE_C_USING_LINKER_crinkler ${CMAKE_CURRENT_SOURCE_DIR}/crinkler23/Win32/Crinkler.exe)
    set(CMAKE_C_USING_LINKER_MODE TOOL)
endif()

add_executable(tss WIN32 main.c) # /SUBSYSTEM:WINDOWS with WIN32, /SUBSYSTEM:CONSOLE without it

target_compile_options(tss PRIVATE
    $<IF:$<CONFIG:DEBUG>, /MDd, /MD /O1>
    /W4 /GS- /analyze-
    /wd4324 # disables the "structure was padded due to alignment specifier" warning
)

if(USE_CRINKLER)
    message("Using crinkler.exe")

    target_compile_definitions(tss PRIVATE USE_CRINKLER)

    target_link_options(tss PRIVATE
        /ENTRY:main
        /NODEFAULTLIB
        /MANIFEST:NO
        /HASHSIZE:1000
        /ORDERTRIES:5000
        /TINYHEADER
        /TINYIMPORT
        /PROGRESSGUI
    )
else()
    message("Using link.exe")

    target_sources(tss PRIVATE dialog.rc) # crinkler doesn't support .rc files...

    target_link_options(tss PRIVATE
        /MACHINE:x86
        /ENTRY:main
        /NODEFAULTLIB
        /NOLOGO
        /MANIFEST:NO
        /ALIGN:32
        /MERGE:.rdata=.
        /MERGE:.pdata=.
        /MERGE:.text=.
        /SECTION:.,ER
    )
endif()

target_link_libraries(tss PRIVATE
    kernel32.lib
    user32.lib
    gdi32.lib
    shlwapi.lib
    shell32.lib
    advapi32.lib
)

add_custom_command(TARGET tss POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tss> $<TARGET_FILE_DIR:tss>/tss.scr
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:tss> $ENV{SystemRoot}/System32/tss.scr ||
            ${CMAKE_COMMAND} -E echo "Need admin rights to be able to copy to $ENV{SystemRoot}/System32/"
)