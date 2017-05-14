cmake_minimum_required(VERSION 3.6)
project(Vr)
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES src/main.cpp include/ModelLoader/Loader.h src/ModelLoader/Loader.cpp include/Tree/ObbTree.h src/Tree/ObbTree.cpp)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ASSIMP_ARCHITECTURE "64")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ASSIMP_ARCHITECTURE "32")
endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

if (WIN32)
    set(ASSIMP_ROOT_DIR CACHE PATH "ASSIMP root directory")

    # Find path of each library
    find_path(ASSIMP_INCLUDE_DIR
            NAMES
            assimp/anim.h
            HINTS
            ${ASSIMP_ROOT_DIR}/include
            )

    if (MSVC12)
        set(ASSIMP_MSVC_VERSION "vc120")
    elseif (MSVC14)
        set(ASSIMP_MSVC_VERSION "vc140")
    endif (MSVC12)

    if (MSVC12 OR MSVC14)

        find_path(ASSIMP_LIBRARY_DIR
                NAMES
                assimp-${ASSIMP_MSVC_VERSION}-mt.lib
                HINTS
                ${ASSIMP_ROOT_DIR}/lib${ASSIMP_ARCHITECTURE}
                )

        find_library(ASSIMP_LIBRARY_RELEASE assimp-${ASSIMP_MSVC_VERSION}-mt.lib PATHS ${ASSIMP_LIBRARY_DIR})
        find_library(ASSIMP_LIBRARY_DEBUG assimp-${ASSIMP_MSVC_VERSION}-mtd.lib PATHS ${ASSIMP_LIBRARY_DIR})

        set(ASSIMP_LIBRARY
                optimized ${ASSIMP_LIBRARY_RELEASE}
                debug ${ASSIMP_LIBRARY_DEBUG}
                )

        set(ASSIMP_LIBRARIES "ASSIMP_LIBRARY_RELEASE" "ASSIMP_LIBRARY_DEBUG")

        FUNCTION(ASSIMP_COPY_BINARIES TargetDirectory)
            ADD_CUSTOM_TARGET(AssimpCopyBinaries
                    COMMAND ${CMAKE_COMMAND} -E copy ${ASSIMP_ROOT_DIR}/bin${ASSIMP_ARCHITECTURE}/assimp-${ASSIMP_MSVC_VERSION}-mtd.dll ${TargetDirectory}/Debug/assimp-${ASSIMP_MSVC_VERSION}-mtd.dll
                    COMMAND ${CMAKE_COMMAND} -E copy ${ASSIMP_ROOT_DIR}/bin${ASSIMP_ARCHITECTURE}/assimp-${ASSIMP_MSVC_VERSION}-mt.dll ${TargetDirectory}/Release/assimp-${ASSIMP_MSVC_VERSION}-mt.dll
                    COMMENT "Copying Assimp binaries to '${TargetDirectory}'"
                    VERBATIM)
        ENDFUNCTION(ASSIMP_COPY_BINARIES)

    endif ()

else (WIN32)

    find_path(
            assimp_INCLUDE_DIRS
            NAMES postprocess.h scene.h version.h config.h cimport.h
            PATHS /usr/local/include/
    )

    find_library(
            assimp_LIBRARIES
            NAMES assimp
            PATHS /usr/local/lib/
    )

    if (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)
        SET(assimp_FOUND TRUE)
    ENDIF (assimp_INCLUDE_DIRS AND assimp_LIBRARIES)

    if (assimp_FOUND)
        if (NOT assimp_FIND_QUIETLY)
            message(STATUS "Found asset importer library: ${assimp_LIBRARIES}")
        endif (NOT assimp_FIND_QUIETLY)
    else (assimp_FOUND)
        if (assimp_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find asset importer library")
        endif (assimp_FIND_REQUIRED)
    endif (assimp_FOUND)

endif (WIN32)
find_package(CGAL REQUIRED COMPONENTS Core)
set(ApproxMVBB_DIR /home/ojabassini/MVBB/build/lib/)
set(Meta_DIR /home/ojabassini/meta)
find_package(ApproxMVBB REQUIRED COMPONENTS SUPPORT_KDTREE SUPPORT_XML )
set(CMAKE_CXX_FLAGS ${ApproxMVBB_CXX_FLAGS})
include_directories(${ApproxMVBB_INCLUDE_DIRS})
include(${CGAL_USE_FILE})
include(CGAL_CreateSingleSourceCGALProgram)
include_directories("${PROJECT_SOURCE_DIR}/include")
#include_directories(${assimp_INCLUDE_DIRS})
add_executable(Vr ${SOURCE_FILES})
target_link_libraries(Vr ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} ${assimp_LIBRARIES} ${ApproxMVBB_LIBRARIES})
