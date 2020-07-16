cmake_minimum_required(VERSION 3.9.0)

get_filename_component(RootDir "${CMAKE_CURRENT_LIST_DIR}" PATH)

function(set_wx_widgets_lib_directories)
	IF( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "4" )
		link_directories("${RootDir}/wxwidgets/lib/vc_dll")
	ELSEIF( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "8" )
		link_directories("${RootDir}/wxwidgets/lib/vc_x64_dll")
	ENDIF()
endfunction(set_wx_widgets_lib_directories)

function(set_wx_widgets_include_directories)	
	IF( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "4" )		
		list(APPEND WXWIDGETS_INCLUDE_DIRS_DEBUG
			"${RootDir}/wxwidgets/lib/vc_dll/mswud"
			"${RootDir}/wxwidgets/include"
		)
		
		list(APPEND WXWIDGETS_INCLUDE_DIRS_RELEASE
			"${RootDir}/wxwidgets/lib/vc_dll/mswu"
			"${RootDir}/wxwidgets/include"
		)
	ELSEIF( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "8" )	
		list(APPEND WXWIDGETS_INCLUDE_DIRS_DEBUG
			"${RootDir}/wxwidgets/lib/vc_x64_dll/mswud"
			"${RootDir}/wxwidgets/include"
		)
		
		list(APPEND WXWIDGETS_INCLUDE_DIRS_RELEASE
			"${RootDir}/wxwidgets/lib/vc_x64_dll/mswu"
			"${RootDir}/wxwidgets/include"
		)
	ENDIF()
	
	#include directories
	target_include_directories(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:${WXWIDGETS_INCLUDE_DIRS_DEBUG}>)
	target_include_directories(${PROJECT_NAME} PUBLIC $<$<CONFIG:Release>:${WXWIDGETS_INCLUDE_DIRS_RELEASE}>)
endfunction()

function(set_project_compile_options)
	list(APPEND COMPILEOPT
		/Gy			  	# Enable function level linking
		/Yu			  	# Use precompiled headers
		/Zm800			# Precompiled header memory allocation limit
		/GR				# Enable run type information
        /std:c++17
		/JMC			# Enables Just My Code stepping, which brings better debuging experience reference #32706 for more information
	)
	
	target_compile_options(${PROJECT_NAME} PUBLIC  ${COMPILEOPT})	
	set_project_compile_options_debugrelease()
endfunction(set_project_compile_options)

function(set_project_definitions)
	#Unicode character set
	add_definitions(-DUNICODE -D_UNICODE)

	# Produces a program database (PDB) that contains type information and symbolic debugging information for use with the debugger. 
	# The symbolic debugging information includes the names and types of variables, as well as functions and line numbers. 
	# /Zi does not affect optimizations. However, /Zi does imply /debug; see /DEBUG (Generate Debug Info) for more information. 
	add_definitions(/Zi)
    add_definitions(/MP)

	#64bit define
	IF( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "8" )
		IF( ${WIN32} ) 
			add_definitions(-D_WIN64)
		ENDIF()
		add_definitions(-D__x86_64__)
	ENDIF()
		
endfunction(set_project_definitions)

function(set_project_compile_options)
	list(APPEND COMPILEOPT
		/Gy			  	# Enable function level linking
		/Yu			  	# Use precompiled headers
		/Zm800			# Precompiled header memory allocation limit
		/GR				# Enable run type information
	)
	
	target_compile_options(${PROJECT_NAME} PUBLIC  ${COMPILEOPT})	
	set_project_compile_options_debugrelease()
endfunction(set_project_compile_options)

function(set_project_compile_flags)
		set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/INCREMENTAL /DEBUG:FASTLINK /SUBSYSTEM:WINDOWS /SAFESEH:NO /LARGEADDRESSAWARE /NODEFAULTLIB:MSVCRT")
		set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/INCREMENTAL:NO /DEBUG:FULL /SUBSYSTEM:WINDOWS /SAFESEH:NO /MAP /ignore:4099 /LARGEADDRESSAWARE")
endfunction(set_project_compile_flags)

function(set_project_processor_definitions)
	list (APPEND COMPILER_DEFINES
		_WINDOWS
	)

	list(APPEND COMPILER_DEFINES_DEBUG
		_DEBUG                    	
	)

	list(APPEND COMPILER_DEFINES_RELEASE
		NDEBUG                  
	)
	
	target_compile_definitions(${PROJECT_NAME} PUBLIC  ${COMPILER_DEFINES})
	target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:${COMPILER_DEFINES_DEBUG}>)
	target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:Release>:${COMPILER_DEFINES_RELEASE}>)
endfunction(set_project_processor_definitions)

function(set_precompiled_header)
		set(FILENAME "stdafx.h")
		set(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp")
        set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Yu\"${FILENAME}\" /DUSE_PRECOMPILED_HEADERS")
        set_source_files_properties(${SOURCE} PROPERTIES COMPILE_FLAGS "/Yc")
endfunction(set_precompiled_header)

function(set_project_bin_properties)
	set(RuntimeOutputNameDebug "${PROJECT_NAME}D")
	
	set_target_properties(${PROJECT_NAME} PROPERTIES 
						RUNTIME_OUTPUT_NAME_DEBUG ${RuntimeOutputNameDebug}
						RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/bin")
					  
	set_target_properties(${PROJECT_NAME} PROPERTIES 
						RUNTIME_OUTPUT_NAME_RELEASE ${PROJECT_NAME}
						RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/bin")
endfunction(set_project_bin_properties)

function(set_project_compile_options_debugrelease)
	list(APPEND COMPILEOPT_DEBUG
		/Od           	# Enable basic run-time checks
		/MDd		  	# Multithreaded debug runtime
	)

	list(APPEND COMPILEOPT_RELEASE
		/O2           	# Optimize for maximum speed
		/MD           	# Multithreaded release runtime
		/Ob1		  	# Inline function expansion
		/GF			  	# Enable string pooling
	)
	
	target_compile_options(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:${COMPILEOPT_DEBUG}>)
	target_compile_options(${PROJECT_NAME} PUBLIC $<$<CONFIG:Release>:${COMPILEOPT_RELEASE}>)
endfunction(set_project_compile_options_debugrelease)

function(set_project_exe_mode)
	file(GLOB_RECURSE sources "*.cpp" "*.h" "*.c")
	set(root ${CMAKE_CURRENT_SOURCE_DIR})
	source_group(TREE ${root} FILES ${sources})
	list(REMOVE_ITEM sources "${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles/${CMAKE_VERSION}/CompilerIdCXX/CMakeCXXCompilerId.cpp")
	list(REMOVE_ITEM sources "${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles/${CMAKE_VERSION}/CompilerIdC/CMakeCCompilerId.c")
	source_group("" FILES ${sources})
	add_executable(${PROJECT_NAME} ${sources})
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
	set_project_compile_options()
	set_project_definitions()
	set_project_compile_flags()
	set_project_processor_definitions()
	set_precompiled_header()
	STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endfunction(set_project_exe_mode)

set(CMAKE_EXE_LINKER_FLAGS_DEBUG 	"")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE 	"")

project(mdb2sqlite)

set_wx_widgets_lib_directories()
set_project_exe_mode()
set_wx_widgets_include_directories()
set_project_bin_properties()

file(GLOB WX_DLLS "${RootDir}/wxwidgets/lib/vc_dll/*.dll")
file(COPY ${WX_DLLS} DESTINATION "${RootDir}/bin")