cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(mcpCharacterization)

find_package(ROOT 6.23 CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_library(Cli SHARED src/cliParams.cpp 
                       src/cliParser.cpp 
)

add_library(Main SHARED src/TAsciiFileReader.cpp
                        src/TGainLogFileReader.cpp
                        src/TGainSpeAnalyser.cpp
                        src/TGainSpeReader.cpp
                        src/TLeCroyMeasureFileReader.cpp
                        src/TLeCroyWaveformFileReader.cpp
                        src/TMeasureStorage.cpp
                        src/TSingleGainLogLine.cpp
                        src/TSingleLeCroyMeasureLine.cpp
                        src/TSingleLeCroyWaveformLine.cpp
)

target_link_libraries(Main PUBLIC ROOT::Core
                                  ROOT::RIO  
                                  ROOT::Gpad
                                  ROOT::Tree
                                  ROOT::HistPainter
                                  ROOT::Graf3d
                                  ROOT::Physics
                                  ROOT::GenVector
)

link_directories(${CMAKE_BINARY_DIR})
add_executable(mcpCharacterization src/mcpCharacterization.cpp)
target_link_libraries(mcpCharacterization PUBLIC Cli
                                                 Main
                                                 ROOT::Core
                                                 ROOT::Gpad
)
        
# Copy scripts directory from source to build dir       
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/scripts $<TARGET_FILE_DIR:${PROJECT_NAME}>/scripts)
                       
# Make symbollic link of inputs and outputs directory from source to build dir       
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                       ${CMAKE_SOURCE_DIR}/inputs $<TARGET_FILE_DIR:${PROJECT_NAME}>/inputs)
                       
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                       ${CMAKE_SOURCE_DIR}/outputs $<TARGET_FILE_DIR:${PROJECT_NAME}>/outputs)                       
