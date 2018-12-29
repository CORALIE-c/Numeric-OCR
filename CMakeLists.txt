cmake_minimum_required(VERSION 3.4)

project(NumericDigitsOCR LANGUAGES CXX)

set(source_files
  src/rodu4140/base_types.hpp
  src/rodu4140/BMP_Loader.cpp
  src/rodu4140/BMP_Loader.hpp
  src/rodu4140/Color.cpp
  src/rodu4140/Color.hpp
  src/rodu4140/Feature_Database.cpp
  src/rodu4140/Feature_Database.hpp
  src/rodu4140/Feature_Loader.cpp
  src/rodu4140/Feature_Loader.hpp
  src/rodu4140/Feature_Vector.cpp
  src/rodu4140/Feature_Vector.hpp
  src/rodu4140/Image.cpp
  src/rodu4140/Image.hpp
  src/rodu4140/input.hpp
  src/rodu4140/Kernel_Image_Operator.cpp
  src/rodu4140/Kernel_Image_Operator.hpp
  src/main.cpp
)

add_executable(${PROJECT_NAME}
  ${source_files}
)
add_executable("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

set_target_properties(${PROJECT_NAME}
  PROPERTIES 
    CXX_STANDARD 11 
    CXX_STANDARD_REQUIRED True
    CXX_EXTENSIONS False
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNUC" OR 
    CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(${PROJECT_NAME}
    PRIVATE "-Wall" "-Werror"
  )
endif ()

target_include_directories(${PROJECT_NAME}
  PRIVATE "external/rapidjson/include"
)

if (MSVC)
  target_include_directories(${PROJECT_NAME}
    PRIVATE "external/dirent/include"
  )
endif ()
