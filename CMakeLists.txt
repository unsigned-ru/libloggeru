###################
## Project Setup ##
###################
cmake_minimum_required(VERSION 3.23)
set(PROJECT_NAME "libloggeru")
project(${PROJECT_NAME} LANGUAGES CXX VERSION 1.0.0)

#stop cmake from prepending
set(CMAKE_SHARED_LIBRARY_PREFIX "") 


#build type option
set(LIBRARY_BUILD_TYPE "SHARED" CACHE STRING "STATIC/SHARED")


# Make lowercase variants of CMAKE variables as required
# note: most of everything in this project is snake_case, following STLXX coding standards
# therefore we want some of these to be snake_case too.
string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_TOLOWER) #eg. windows
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_TOLOWER) #eg. debug/release

#################
## VCPKG SETUP ##
#################
if (WIN32)	
	# If we are on a Windows based system, we should use VCPKG!
	option(VCPKG_PATH "Path to root folder of VCPKG")

	#check if a custom vpckg path was provided
	if (${VCPKG_PATH} STREQUAL OFF)
		set(VCPKG_PATH "C:/Program Files (x86)/vcpkg") #if not, set a default path
	endif()

	#include toolchain file
	include("${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")

	#Apply correct include directory according to 
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		#64 bit
		set(ARCHITECTURE "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		# 32 bit
		set(ARCHITECTURE "x86")
	endif()

	set(CMAKE_INSTALL_PREFIX ${VCPKG_PATH}/installed/${ARCHITECTURE}-windows/)
endif()



###########################
## Create Library Target ##
###########################
add_library(${PROJECT_NAME} ${LIBRARY_BUILD_TYPE})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
message("Library build type: ${LIBRARY_BUILD_TYPE}")

###################
## Project Files ##
###################
target_sources(${PROJECT_NAME}
    PRIVATE 
	"src/loggeru.cpp"
    PUBLIC FILE_SET HEADERS 
	BASE_DIRS "include"
    FILES 
	"include/loggeru.h"
	"include/loggeru/console_colors.h"
)

# Add project include dirs
target_include_directories(${PROJECT_NAME} PRIVATE 
	"src"
	"include"
	$<$<NOT:$<STREQUAL:${VCPKG_PATH}, OFF>>:${VCPKG_PATH}/installed/${ARCHITECTURE}-windows/include>
)

##########
## Libs ##
##########

#add FMT
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)


####################
## Compiler Flags ##
####################
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /EHsc /std:c++20>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror -std=c++20>
)

##################
## Installation ##
##################

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${PROJECT_NAME}-config-version.cmake"
	VERSION ${${PROJECT_NAME}_VERSION}
	COMPATIBILITY AnyNewerVersion
)

if (WIN32)
	if (NOT EXISTS ${VCPKG_PATH})
		message(FATAL_ERROR "Failed to find VCPKG location to install to, CMake will exit." )
	endif()

	SET(${PROJECT_NAME}_LIB_DIR_PREFIX "")
	if(${CMAKE_BUILD_TYPE_TOLOWER} STREQUAL "debug")
		SET(${PROJECT_NAME}_LIB_DIR_PREFIX "debug/")
	endif()
	
	install(TARGETS ${PROJECT_NAME} 
		EXPORT ${PROJECT_NAME}-targets
			LIBRARY DESTINATION ${${PROJECT_NAME}_LIB_DIR_PREFIX}
			ARCHIVE DESTINATION ${${PROJECT_NAME}_LIB_DIR_PREFIX}lib
			RUNTIME DESTINATION ${${PROJECT_NAME}_LIB_DIR_PREFIX}bin
			FILE_SET HEADERS DESTINATION include
	)

	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-vcpkg.pc.in" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" @ONLY)
	install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION "lib/pkgconfig")

	install(EXPORT ${PROJECT_NAME}-targets
		FILE ${PROJECT_NAME}-targets.cmake
		NAMESPACE libloggeru::
		DESTINATION "share/${PROJECT_NAME}")

	install(FILES "cmake/${PROJECT_NAME}-config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
		DESTINATION "share/${PROJECT_NAME}"
	)

else()
	include(GNUInstallDirs)

	install(TARGETS ${PROJECT_NAME} 
		EXPORT ${PROJECT_NAME}-targets
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
			FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)

	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}.pc.in" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" @ONLY)
	install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

	install(EXPORT ${PROJECT_NAME}-targets
		FILE ${PROJECT_NAME}-targets.cmake
		NAMESPACE ${PROJECT_NAME}::
		DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

	install(FILES "cmake/${PROJECT_NAME}-config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
		DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
	)
endif()
