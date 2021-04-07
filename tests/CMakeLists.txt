include(FetchContent)
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)

# CMake 3.14+
FetchContent_MakeAvailable(catch)

set(PYTAGORAS_TEST_LIB pico_pytagoras_tests)
set(PYTAGORAS_TEST_RUNNER pico_pytagoras_runtest)


add_executable(${PYTAGORAS_TEST_LIB}
    q_tests.cpp
)
target_link_libraries(${PYTAGORAS_TEST_LIB} PUBLIC ${PYTAGORAS_LIBNAME} Catch2WithMain)

target_compile_features(${PYTAGORAS_TEST_LIB} PUBLIC cxx_std_17)
set_target_properties(${PYTAGORAS_TEST_LIB} PROPERTIES CXX_EXTENSIONS OFF)