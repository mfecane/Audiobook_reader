cmake_minimum_required(VERSION 3.18)

include_directories("D:/dev/lib/soundtouch_dll-2.1.1")
link_directories("D:/dev/lib/soundtouch_dll-2.1.1")

project("audiobook_reader")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Quick Core QuickControls2 Multimedia REQUIRED)

qt5_add_resources(QTRESOURCES src/res.qrc)

file(GLOB_RECURSE SRC_UI
    src/main.qml
    src/qtquickcontrols2.conf
    src/imports/Theme/Theme.qml
    src/Components/Appbutton.qml
    src/Components/Player2.qml
    src/Components/BookList.qml
    src/Components/Playlist.qml
    src/Components/OpenFolderDialog.qml
    src/Components/Ass.qml
    src/Components/SettingsButton.qml
    src/Components/PlayButton.qml
    src/Components/AppButton.qml
)

set(RESOURCES
    src/res.rc
)

add_executable(${PROJECT_NAME} WIN32 
    src/main.cpp
    src/backend.h
    src/backend.cpp
    src/audiobook.h
    src/audiobook.cpp
    src/audiobookinfo.h
    src/audiobookinfo.cpp
    src/audiobookmodel.h
    src/audiobookmodel.cpp
    src/audiobooklist.h
    src/audiobooklist.cpp
    src/audiobooklistmodel.h
    src/audiobooklistmodel.cpp
    src/filesizerequest.h
    src/filesizerequest.cpp
    src/audiobookc.h
    src/audiobookc.cpp
    src/globaljson.h
    src/globaljson.cpp
    src/player.h
    src/player.cpp
    src/blwindow.h
    src/blwindow.cpp
    src/blheader.h
    src/blheader.cpp
    README.md
    installer/installer.nsi

    ${SRC_UI}
    ${RESOURCES}
    ${QTRESOURCES}
    )

source_group("ui" FILES ${SRC_UI})

set(QML_IMPORT_PATH 
    "${CMAKE_SOURCE_DIR}/src/imports" 
	CACHE STRING "" FORCE)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Qml)
target_link_libraries(${PROJECT_NAME} Qt5::Quick)
target_link_libraries(${PROJECT_NAME} Qt5::Multimedia)
target_link_libraries(${PROJECT_NAME} "SoundTouchDLL_x64.lib")
target_link_libraries(${PROJECT_NAME} "dwmapi.lib")
	
# set startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "D:/dev/lib/soundtouch_dll-2.1.1/SoundTouchDLL_x64.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# RUNNUNG WINDEPLOYQT

# Retrieve the absolute path to qmake and then use that path to find
# the binaries

get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
            "$<TARGET_FILE:${PROJECT_NAME}>" ARGS --qmldir ${CMAKE_SOURCE_DIR}/src/
    COMMENT "Running windeployqt..."
)

#useful garbage

#ADD_DEFINITIONS(-DUNICODE)
#ADD_DEFINITIONS(-D_UNICODE)

#target_link_libraries(${PROJECT_NAME} "Comctl32.lib")
