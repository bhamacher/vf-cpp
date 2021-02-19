cmake_minimum_required(VERSION 3.1)

#set project name and language
project(vf-cpp LANGUAGES CXX)


#Automoc and RCC ON. Needed in QT projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#C++ 11 Flag# spawn out some info on configuration
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Set project Version
set(VfCpp_VERSION_MAJOR "0")
set(VfCpp_VERSION_MINOR "0")
set(VfCpp_VERSION_PATCH "1")

set(PROJECT_VERSION "${VfCpp_VERSION_MAJOR}.${VfCpp_VERSION_MINOR}.${VfCpp_VERSION_PATCH}")


#Adding cmake functions like feature_summary()
include(FeatureSummary)
include(GNUInstallDirs)

#Find dependecies
find_package(Qt5 COMPONENTS Core Gui CONFIG REQUIRED)
find_package(VfComponent REQUIRED)
find_package(VfHelpers REQUIRED)
find_package(VfEvent REQUIRED)


#sum up project Files 
file(GLOB SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/src/*.h
    )

file(GLOB RESOURCES 
    ${PROJECT_SOURCE_DIR}/*.qrc
    )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
#create library 
add_library(VfCpp SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )




#link dependencies
target_link_libraries(VfCpp 
    PUBLIC
    Qt5::Gui
    PRIVATE
    Qt5::Core
    VeinMeta::VfHelpers
    VeinMeta::VfEvent
    VeinMeta::VfComponent
    )


#set target Version
set_target_properties(VfCpp PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(VfCpp PROPERTIES SOVERSION ${VfCpp_VERSION_MAJOR})

#generate export header
include(GenerateExportHeader)
generate_export_header(VfCpp)


#install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vfcpp_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/VfCpp
    )

# announce headers - target perspective
target_include_directories(VfCpp
    PUBLIC
    $<INSTALL_INTERFACE:include/VfCpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

# install library
install(TARGETS VfCpp
    EXPORT VfCppExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *.cmake File
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    VfCppConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *.cmake File 
configure_file(VfCppConfig.cmake.in VfCppConfig.cmake @ONLY)


# install *(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/VfCppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/VfCppConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VfCpp
    )

# install targets cmake-files
install(EXPORT VfCppExport
    FILE VfCppTargets.cmake
    NAMESPACE VeinMeta::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VfCpp
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/VfCpp
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)


