#
# libflewnit build script
#
# TODO prio 1: find out how to build a shared library and how to let it be found by an executable
# TODO prio 2: maybe add some scripts to download, build and install all depencencies; needs root acces... sudo cmake . ? ;(
#
# TODO prio x: Windows port might come in the future, but is not supported yet; Windows stuff hacked into this script only
# to act as code snippets to make a future port slightly easier;
#

cmake_minimum_required(VERSION 2.8)

project(libflewnit)

#Set user defined stuff----------------------------------------

if(UNIX)

    set(MEDIA_LAYER "GLFW" CACHE STRING  "Media layer; Chose GLFW, XCB or SDL")
    
    option(USE_SDL_LOCAL "if SDL is chosen as media layer, choose either installed version or a locally compiled one;option is necessary to handly folder structures of local builds" OFF)
    set(SDL_DIRECTORY "/usr/local" CACHE PATH "SDL Directory; might be locally compiled, or installed in /usr or /usr/local")
    set(GLFW_DIRECTORY "/usr/local" CACHE PATH "SDL Directory; might be locally compiled, or installed in /usr or /usr/local")
   
    # a bad hack, should be replaced with a FIND_SDL()-module-like stuff, but i'm to stupid for this atm
    #option(SDL_INSTALLED_LOCALLY "if USE_SDL_LOCAL is unselected, then you might have SDL installed in /usr/local instead of /usr" ON)

    option(RELEASE "build in Release mode for more performance but no debugging features" OFF)
    
    set(GLM_DIRECTORY "../thirdParty/ogl-math" CACHE PATH "Path to glm Math library")
    set(ASSIMP_DIRECTORY "/usr/local/include/assimp" CACHE PATH "Path to assimp asset loading library")
    

elseif(WIN32)

     #no other option but SDL for Windows atm
     set(medialayer "GLFW")
     set(SDL_DIRECTORY "../../Dependencies/glfw-2.7" CACHE PATH "GLFW window manager directory")
     
     ##sdl maybe to come...

endif(UNIX)


#set binary output directories for lib and app (internally used by cmake)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

#set source directories for the library resp the application
set(LIBRARY_SOURCE_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
set(APPLICATION_SOURCE_DIRECTORY ${CMAKE_SOURCE_DIR}/app)

#Optimization / Debug
if(UNIX)

    if(RELEASE)
        set(CMAKE_CXX_FLAGS "-O3 -msse4 -mssse3") #Optimized
    else()
        set(CMAKE_CXX_FLAGS "-Wall -O0 -g3") #Debug
        add_definitions(-D_DEBUG)
    endif()
    
    #Set c++0x
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
    
elseif(WIN32)

    if(MSVC)
        #some big hacks concerning build configurations to go...
    endif(MSVC)
    
endif(UNIX)


#need to include FindOPENCL.cmake to
SET( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
message("CMake module path for OpenCL finding: ${CMAKE_MODULE_PATH}\n")
#needed for library files,  headers are hardcoded below ;(
FIND_PACKAGE (OPENCL)
#------------------------------------------------


#check for qt
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})



if(UNIX)

    #check for opengl3 headers aLIBRARY_SOURCE_DIRECTORYnd download them if they don't exist ---------------------------------------------
    function(download_gl_header GL_HEADER)
        if(NOT EXISTS include/GL3/${GL_HEADER})
	    	execute_process(
		    	WORKING_DIRECTORY include/GL3
    			COMMAND wget http://www.opengl.org/registry/api/${GL_HEADER}
	    	)
	    endif()
    endfunction(download_gl_header)

    if(EXISTS /usr/include/GL3)
	    message("OpenGL 3 Headers are  in /usr/include")
	    #they are not necessaryly there, folder might be empty; but let's think about this later
    ELSE()
        # untested code ;(
    	message("No OpenGL 3 Headers found in /usr! Using local ones")
    	IF(NOT EXISTS include/GL3)
		    execute_process(COMMAND mkdir include/GL3)
	    ENDIF()
	
	    set(GL_HEADERS gl3.h glext.h glxext.h)
    	foreach(GL_HEADER ${GL_HEADERS})
		    download_gl_header(${GL_HEADER})
        endforeach(GL_HEADER)
    
    	include_directories(include/GL3)
    endif()


elseif(WIN32)

    if(MSVC)
        #some big hacks concerning acquisition of gl3-headers to go...
    endif(MSVC)
    
endif(UNIX)


if(UNIX)
   
    # Check for nvidia libGL in Ubuntu; --------------------------------------------
    # Is this a hack realy only concerning Ubuntu and not the whole Linux Systems?
    if(EXISTS "/usr/lib/nvidia-current/libGL.so")
    	message("Taking Ubuntu Nvidia libGL")
	    set(OPENGL_LIB /usr/lib/nvidia-current/libGL.so)
    else()
    	message("Taking system libGL")
	    set(OPENGL_LIB GL)
    ENDIF() 

    
elseif(WIN32)
  
endif(UNIX)


if(UNIX)

    if(USE_SDL_LOCAL)
        set(SDL_INCLUDE_DIRECTORY ${SDL_DIRECTORY}/include)
   else(USE_SDL_LOCAL)
       set(SDL_INCLUDE_DIRECTORY ${SDL_DIRECTORY}/include/SDL)
   endif(USE_SDL_LOCAL)
   
elseif(WIN32)
   set(SDL_INCLUDE_DIRECTORY ${SDL_DIRECTORY}/include)
endif(UNIX)


#${SDL_INCLUDE_DIRECTORY}
#     ${QT_INCLUDES}	  
 #to add the "src" directory to the include path might seem hacky, but at least for development purposes this seems appropriate
 include_directories(
     src
     include
     ${GLM_DIRECTORY} 
     ${ASSIMP_DIRECTORY}  	
     include/opencl10
     ${QT_INCLUDES}
 )

link_directories(
	/usr/local/lib
)
 
 
 #choose window manager library-----------------------------------------------------------------------------
if(MEDIA_LAYER STREQUAL "SDL")
    set(WINDOW_MANAGER_LIBRARY_NAME SDL)
elseif(MEDIA_LAYER STREQUAL "XCB")
    set(WINDOW_MANAGER_LIBRARY_NAME XCB)
elseif(MEDIA_LAYER STREQUAL "GLFW")
    set(WINDOW_MANAGER_LIBRARY_NAME GLFW)
else()
    message(FATAL_ERROR "Enter a valid Media Layer Library, either SDL or XCB")
endif()

#tell our source code which media layer to use
#add_definitions(-DFLEWNIT_USE_${WINDOW_MANAGER_LIBRARY_NAME})
set(STRINGTOECHO "# define FLEWNIT_USE_${WINDOW_MANAGER_LIBRARY_NAME}\n")
execute_process(
    WORKING_DIRECTORY src
	COMMAND echo ${STRINGTOECHO} 
	OUTPUT_FILE cmakeGeneratedDefinitions.h 
	)

# another now obsolete medialayer-hack; see above
#find media layer source files
#file(GLOB_RECURSE GLOB_RESULT ${LIBRARY_SOURCE_DIRECTORY}/MediaLayer/${WINDOW_MANAGER_LIBRARY_NAME}/*.cpp)
#add them to the whole library source file list
#set(LIBRARY_SOURCE_FILES ${LIBRARY_SOURCE_FILES} ${GLOB_RESULT})
#----------------------------------------------------------------------------------------------------------


#Add flewnit library sources --------------------------------------------------------------------------------

#glob all .cpp files in the source directory
file(GLOB_RECURSE LIBRARY_SOURCE_FILES ${LIBRARY_SOURCE_DIRECTORY}/*.cpp)


#the following stuff was a hack for a compile-time-dependent media layer setup; has now become obsolete because of a real interface:
#This stuff instead of globbing the whole source folder is necessary in order to handle the media layer sources differently
#set(LIBRARY_RELATIVE_DIRECTORIES
#       Common
#       Simulator
#       UserInterface
#       Buffer
#       Util
#       World
#)
#
#first, add the toplevel UnifiedRenderingEngine-sources, which ar running all our stuff in the library ;)
#set(LIBRARY_SOURCE_FILES ${LIBRARY_SOURCE_DIRECTORY}/URE.cpp)
#
#glob all source files in the given subdirectory
#foreach(LIBRARY_RELATIVE_DIRECTORY ${LIBRARY_RELATIVE_DIRECTORIES})
#    file(GLOB_RECURSE GLOB_RESULT ${LIBRARY_SOURCE_DIRECTORY}/${LIBRARY_RELATIVE_DIRECTORY}/*.cpp)
#    set(LIBRARY_SOURCE_FILES ${LIBRARY_SOURCE_FILES} ${GLOB_RESULT})
    #TODO check if adding all the header will omit the fuckup that changes in headers don't result in rebuild :@
#endforeach(LIBRARY_RELATIVE_DIRECTORY)
#----------------------------------------------------------------------------------------------------------

#found a way to propagate stuff to grantlee without qt meta objects, no moc'ing necessaray anymore :)
#qt4_wrap_cpp (MOC_OUTFILES 
#    ${LIBRARY_SOURCE_DIRECTORY}/MPP/Shader/GrantleeShaderFeaturesContext.h
#    ${LIBRARY_SOURCE_DIRECTORY}/Simulator/ShaderFeatureRelevantEnums.h 
#)


###########################################################################################################
#finally, do some build stuff for libflewnit:
#tell cmake to create a library named "libflewnit", which uses auch library source file list as input for sources ;)
#shasred is bad for debugging :(
#add_library(flewnit SHARED ${LIBRARY_SOURCE_FILES})
add_library(flewnit ${LIBRARY_SOURCE_FILES} ${MOC_OUTFILES})



#tell cmake to link the libflewnit library against the given third party libraries
#TODO: add gpuTE ass dependency :D
#TODO: unterstand why only relevant qt stuff will be linked against ;) yes, I'm noob with cmake and qt ;)
#${QT_LIBRARIES}
target_link_libraries(flewnit ${OPENGL_LIB} ${OPENCL_LIBRARIES} ${QT_LIBRARIES} )

target_link_libraries(flewnit boost_filesystem)

target_link_libraries(flewnit tinyxml freeimageplus assimp)

target_link_libraries(flewnit grantlee_core)







IF(MEDIA_LAYER STREQUAL "SDL")
    if(USE_SDL_LOCAL)
        if(UNIX)
            #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${SDL_DIRECTORY}/build/.libs")
            message("trying to link against "${SDL_DIRECTORY}"/build/.libs/libSDL.a as user specified local SDL stuff; Make sure build dir is 'build'")
	        target_link_libraries(flewnit ${SDL_DIRECTORY}/build/.libs/libSDL.a)
	    elseif(WIN32)
	        if(MSVC)
	            #TODO some weird VS build path
	        endif(MSVC)
	    endif(UNIX)
    else(USE_SDL_LOCAL)
        if(UNIX)
    		target_link_libraries(flewnit SDL)
    	elseif(WIN32)
    	    #TODO ist there any "install" for SDL on Windows?
    	endif(UNIX)
	endif(USE_SDL_LOCAL)
	
elseif(MEDIA_LAYER STREQUAL "XCB")
    message("trying to link against XCB")
	target_link_libraries(flewnit X11-xcb xcb-keysyms)
	
elseif(MEDIA_LAYER STREQUAL "GLFW")
    message("trying to link against GLFW")
    target_link_libraries(flewnit glfw Xrandr)	
	
else()
    message("should never ended up here as script should have terminated earlies due to an error")
endif()
###########################################################################################################


###########################################################################################################
#now, build our app execuatble
#build flewnit application
file(GLOB_RECURSE FLEWNIT_APP_SOURCE_FILES ${APPLICATION_SOURCE_DIRECTORY}/*.cpp)
add_executable(appflewnit ${FLEWNIT_APP_SOURCE_FILES})
target_link_libraries(appflewnit flewnit)
###########################################################################################################
