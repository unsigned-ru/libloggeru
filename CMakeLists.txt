cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME "loggeru")
project(${PROJECT_NAME} LANGUAGES CXX VERSION 1.0.0)

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
endif()

###################
## Project Files ##
###################
set(${PROJECT_NAME}_SRC_FILES
	#src
	"src/Loggeru.cpp"
)

# Add source to this project's library.
# THIS IS WHERE THE TARGET GETS CREATED
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRC_FILES})

# Add project include dirs
target_include_directories(${PROJECT_NAME} PRIVATE 
	"src"
	"include"
	${VCPKG_PATH}/installed/${ARCHITECTURE}-windows/include,
)

##########
## Libs ##
##########

#add FMT
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)


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
		"${PROJECT_NAME}ConfigVersion.cmake"
		VERSION ${${PROJECT_NAME}_VERSION}
		COMPATIBILITY AnyNewerVersion
	)

if (WIN32)
	if (NOT EXISTS ${VCPKG_PATH})
		message(FATAL_ERROR "Failed to find VCPKG location to install to, CMake will exit." )
	endif()

	install(TARGETS ${PROJECT_NAME} 
			EXPORT ${PROJECT_NAME}Targets
			FILE_SET HEADERS
			LIBRARY DESTINATION ${VCPKG_PATH}/installed/${ARCHITECTURE}-windows/lib
			ARCHIVE DESTINATION ${VCPKG_PATH}/installed/${ARCHITECTURE}-windows/lib
			RUNTIME DESTINATION ${VCPKG_PATH}/installed/${ARCHITECTURE}-windows/bin
			INCLUDES DESTINATION ${VCPKG_PATH}/installed/${ARCHITECTURE}-windows/include
	)

	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-vcpkg.pc.in" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" @ONLY)
	install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION "${VCPKG_PATH}/installed/${ARCHITECTURE}-windows/lib/pkgconfig")


	install(EXPORT ${PROJECT_NAME}Targets
		FILE ${PROJECT_NAME}Targets.cmake
		NAMESPACE loggeru::loggeru
		DESTINATION "${VCPKG_PATH}/installed/${ARCHITECTURE}-windows/share/${PROJECT_NAME}/")

	install(FILES "cmake/${PROJECT_NAME}Config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
		DESTINATION "${VCPKG_PATH}/installed/${ARCHITECTURE}-windows/share/${PROJECT_NAME}/"
	)

else()
	# Installation
	include(GNUInstallDirs)

	install(TARGETS ${PROJECT_NAME} 
		EXPORT ${PROJECT_NAME}Targets
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)

	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}.pc.in" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" @ONLY)
	install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}")


	install(EXPORT ${PROJECT_NAME}Targets
		FILE ${PROJECT_NAME}Targets.cmake
		NAMESPACE loggeru::loggeru
		DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

	install(FILES "cmake/${PROJECT_NAME}Config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
		DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
	)

endif()

# --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

################
## Unit Tests ##
################
## TODO: add unit tests
#
#set(PROJECT_UNIT_TEST_NAME ${PROJECT_NAME}_UnitTests)
#
#add_executable(${PROJECT_UNIT_TEST_NAME} 
#	"unit-tests/Tests.cpp"
#)
#
## link our library
#target_link_libraries(${PROJECT_UNIT_TEST_NAME} PUBLIC ${PROJECT_NAME})
#
## add our library headers
#target_include_directories(${PROJECT_UNIT_TEST_NAME} PUBLIC 
#	"include"
#)
#
#####################
### Compiler Flags ##
#####################
#target_compile_options(${PROJECT_UNIT_TEST_NAME} PRIVATE
#  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /EHsc /std:c++20>
#  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror -std=c++20>
#)
#
#include(CTest)
#add_test(
#	NAME unittests
#	COMMAND ${PROJECT_UNIT_TEST_NAME}
#	WORKING_DIRECTORY unit-tests
#)