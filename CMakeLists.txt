cmake_minimum_required(VERSION 3.13)

set(PROJECT PianoTrainer)
project(${PROJECT})

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-g2")

set(CMAKE_CXX_STANDARD 11)

set(HEADER
        src/trainerwindow.h
        src/playcontroller.h
        src/abstractplayview.h
        src/playsoundlistener.h
        src/playsoundview.h
        src/playpianoview.h
        src/songmodel.h
        src/keyboard.h

        )

set(SOURCE
        src/main.cpp
        src/trainerwindow.cpp
        src/playcontroller.cpp
        src/abstractplayview.cpp
        src/playsoundlistener.cpp
        src/playsoundview.cpp
        src/playpianoview.cpp
        src/songmodel.cpp
        src/keyboard.cpp

        )

    set(UI
        src/trainerwindow.ui
        )

set(RESOURCE src/resources.qrc)

ADD_DEFINITIONS(${QT_DEFINITIONS})

find_package(Qt5 COMPONENTS Core Gui Widgets Qml REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)


# Common QMidi source files & library
aux_source_directory("${PROJECT_SOURCE_DIR}/src/QMidi/src" SOURCES)
add_subdirectory("${PROJECT_SOURCE_DIR}/src/QMidi")

set(LIBRARIES Qt5::Core Qt5::Gui Qt5::Qml)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

add_executable(${PROJECT} ${HEADER} ${SOURCE} ${RESOURCE} ${UI})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/QMidi)
target_link_libraries(${PROJECT} ${LIBRARIES} Qt5::Core Qt5::Gui Qt5::Widgets QMidi)
