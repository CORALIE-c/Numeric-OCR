cmake_minimum_required(VERSION 3.4)

project(NumericDigitsOCR LANGUAGES CXX)

set(source_files
  src/ocr/base_types.hpp
  src/ocr/BMP_Loader.cpp
  src/ocr/BMP_Loader.hpp
  src/ocr/Color.cpp
  src/ocr/Color.hpp
  src/ocr/Feature_Database.cpp
  src/ocr/Feature_Database.hpp
  src/ocr/Feature_Loader.cpp
  src/ocr/Feature_Loader.hpp
  src/ocr/Feature_Vector.cpp
  src/ocr/Feature_Vector.hpp
  src/ocr/Image.cpp
  src/ocr/Image.hpp
  src/ocr/input.hpp
  src/ocr/Kernel_Image_Operator.cpp
  src/ocr/Kernel_Image_Operator.hpp
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
