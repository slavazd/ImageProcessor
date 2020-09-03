cmake_minimum_required(VERSION 3.18)

project(imageprocessor)
add_executable(imageprocessor imageprocessor.cpp)	
find_package(ImageMagick COMPONENTS Magick++)

if(NOT ImageMagick_Magick++_FOUND)
	message(SEND_ERROR "Failed to find ImageMagick")
	return()
else()
	include_directories(${ImageMagick_Magick++_INCLUDE_DIR})
	target_link_libraries(imageprocessor ${ImageMagick_Magick++_LIBRARY})
endif()

