# 1. Requirements
cmake_minimum_required(VERSION 3.10)
project(usd_diary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2.Configuration
add_compile_options(-Wno-deprecated)  # This is just to avoid some spammy warnings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # This line is optional. It's just for certain IDEs to use

# 3. Boilerplate
set(USD_LIBRARY_DIRECTORY $ENV{USD_INSTALL_ROOT}/lib)
set(USD_INCLUDE_DIRECTORY $ENV{USD_INSTALL_ROOT}/include)

find_library(USD_BOOST_PYTHON boost_python HINTS ${USD_LIBRARY_DIRECTORY})
# USD_BOOST_PYTHON boost_python was actually boost_python3.11 in my libs dir, I created a simlink however.
find_library(USD_KIND usd_kind HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_SDF usd_sdf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_TF usd_tf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USD usd_usd HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USDGEOM usd_usdGeom HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_VT usd_vt HINTS ${USD_LIBRARY_DIRECTORY})

find_package(PythonLibs REQUIRED)

# 4. Include/Link Everything
add_executable(scope_reparent_run 
	"scope_reparent/src/main.cpp" 
	"scope_reparent/src/cli_processor.cpp")

target_include_directories(scope_reparent_run
PUBLIC
	${PYTHON_INCLUDE_PATH}
	${USD_INCLUDE_DIRECTORY}
)

target_link_libraries(
	scope_reparent_run
	${PYTHON_LIBRARY}
	${USD_BOOST_PYTHON}
    ${USD_KIND}
	${USD_SDF}
	${USD_TF}
	${USD_USD}
	${USD_USDGEOM}
	${USD_VT}
)
