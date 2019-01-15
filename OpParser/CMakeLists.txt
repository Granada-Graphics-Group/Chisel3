project(OpParser)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
MESSAGE( STATUS "CMake Version: " ${CMAKE_VERSION} )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(ANTLR4 antlr-4.7-complete.jar)

option(generateCPPParser "Use antlr4 to generate the cpp parser" OFF)

if(generateCPPParser)
    add_custom_target(GenerateParser
    COMMAND
    java -jar ${PROJECT_SOURCE_DIR}/${ANTLR4} -Werror -Dlanguage=Cpp -listener -visitor -o ${PROJECT_SOURCE_DIR}/ -package OpLanguage ${PROJECT_SOURCE_DIR}/OLexer.g4 ${PROJECT_SOURCE_DIR}/OParser.g4
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    DEPENDS ${PROJECT_SOURCE_DIR}/OLexer.g4 ${PROJECT_SOURCE_DIR}/OParser.g4
    )
endif()
   
set (OPPARSER_HDRS  runtime
                    runtime/src
                    runtime/src/misc
                    runtime/src/atn
                    runtime/src/dfa
                    runtime/src/tree
                    runtime/src/support
    )

set (OPPARSER_SRCS  OLexer.cpp
                    OParser.cpp
                    OParserBaseListener.cpp
                    OParserBaseVisitor.cpp
                    OParserListener.cpp
                    OParserVisitor.cpp
                    OErrorListener.cpp
    )
    
add_library(OpParser ${OPPARSER_SRCS} translator.cpp)
target_include_directories(OpParser PUBLIC ${OPPARSER_HDRS})
target_include_directories(OpParser INTERFACE ${OPPARSER_HDRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(OpParser PUBLIC ANTLR4CPP_STATIC GLM_ENABLE_EXPERIMENTAL GLM_FORCE_CTOR_INIT)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
    target_link_libraries(OpParser antlr4-runtime)
  else()
    target_link_libraries(OpParser antlr4-runtime-gcc5)
  endif()
else()
	target_link_libraries(OpParser antlr4-runtime)
endif()  

#if(generateCPPParser)
#   add_dependencies(OpParser GenerateParser)    
#endif()
