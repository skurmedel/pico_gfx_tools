set(PYTAGORAS_HEADER_DIR "${PROJECT_SOURCE_DIR}/include/")

add_library(${PYTAGORAS_LIBNAME} STATIC 
    q.cpp
)
target_include_directories(${PYTAGORAS_LIBNAME} PUBLIC ${PYTAGORAS_HEADER_DIR})

target_compile_features(${PYTAGORAS_LIBNAME} PUBLIC cxx_std_17)
set_target_properties(${PYTAGORAS_LIBNAME} PROPERTIES CXX_EXTENSIONS OFF)
