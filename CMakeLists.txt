project(Chisel)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
MESSAGE( STATUS "CMake Version: " ${CMAKE_VERSION} )
MESSAGE( STATUS "Compiler: " ${CMAKE_CXX_COMPILER} )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#SET(CMAKE_CXX_EXTENSIONS OFF)  

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Svg)
find_package(OpenGL REQUIRED)

add_subdirectory(QtUI)
add_subdirectory(OpParser)

include_directories(QtUI lib/headers lib/headers/pugixml-1.6/src)
target_include_directories(QtUI PUBLIC ${Chisel_SOURCE_DIR} lib/headers)
target_include_directories(OpParser PUBLIC ${Chisel_SOURCE_DIR} lib/headers)

if(UNIX)
    link_directories(lib/linux)
else()    
    link_directories(lib/windows/debug)
	link_directories(lib/windows/release)    
endif()    

set(Chisel_SRCS  lib/headers/pugixml-1.6/src/pugixml.cpp 
                    sqlitedatabasemanager.cpp databasetable.cpp 
                    layer.cpp registerlayer.cpp palette.cpp
                    log.cpp logger.cpp 
                    light.cpp
                    renderer.cpp glrenderer.cpp
                    rendertechnique.cpp rendertarget.cpp renderpass.cpp
                    computejob.cpp
                    unsynchronizedbufferobject.cpp persistentbufferobject.cpp bufferobject.cpp
                    vertexarray.cpp arraybufferobject.cpp pixelbufferobject.cpp uniformbufferobject.cpp
                    fontmanager.cpp resourcemanager.cpp bufferlockmanager.cpp
                    fbxloader.cpp plyloader.cpp objloader.cpp fileloader.cpp                      
                    scene3d.cpp camera.cpp model3d.cpp material.cpp
                    texturearray.cpp texture.cpp sampler.cpp textureview.cpp
                    mesh.cpp cone.cpp cylinder.cpp circle.cpp disk.cpp sphere.cpp quad.cpp line.cpp
                    shader.cpp GLSLFileLoader.cpp 
                    fbobject.cpp fbxexporter.cpp
                    chisel.cpp main.cpp
   )
   
set(SQLite3SRCS sqlite3/sqlite3.c)

add_executable(Chisel ${Chisel_SRCS} ${SQLite3SRCS} QtUI/resources.qrc)

target_compile_definitions(Chisel PUBLIC GLM_ENABLE_EXPERIMENTAL GLM_FORCE_CTOR_INIT)
target_link_libraries(Chisel ${OPENGL_gl_LIBRARY} QtUI OpParser Qt5::Svg)
if(UNIX)
    target_include_directories(Chisel PRIVATE ${QtUI_BINARY_DIR})
    target_link_libraries(Chisel freetype GLEW m dl pthread debug fbxsdkd optimized fbxsdk)    
else()
    target_include_directories(Chisel PRIVATE ${QtUI_BINARY_DIR}/QtUI_autogen/include)  
    target_compile_definitions(Chisel PUBLIC NOMINMAX FBXSDK_SHARED)    
    target_link_libraries(Chisel glew32 debug libfbxsdkd debug freetype265d optimized libfbxsdk optimized freetype265)
endif()

install(TARGETS Chisel RUNTIME DESTINATION bin)
