cmake_minimum_required(VERSION 3.18)

include_directories("C:/dev/lib/soundtouch_dll-2.1.1")
link_directories("C:/dev/lib/soundtouch_dll-2.1.1")

project("audiobook_reader")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Quick Core QuickControls2 Multimedia REQUIRED)

qt5_add_resources(RESOURCES src/res.qrc)

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
)

add_executable(${PROJECT_NAME} WIN32 
			src/main.cpp
			src/backend.h
			src/backend.cpp
			src/audiobook.h
			src/audiobook.cpp
                        src/audiobooklist.h
                        src/audiobooklist.cpp
                        src/audiobookfile.h
                        src/audiobookfile.cpp
			src/book.h
			src/book.cpp
			src/bookfile.h
			src/bookfile.cpp
			src/booklist.h
			src/booklist.cpp
                        src/booklistmodel.h
                        src/booklistmodel.cpp
                        src/globaljson.h
                        src/globaljson.cpp
                        src/player.h
                        src/player.cpp
			${SRC_UI}
			${RESOURCES}
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
target_link_libraries(${PROJECT_NAME} Qt5::Multimedia)
target_link_libraries(${PROJECT_NAME} "SoundTouchDLL_x64.lib")
	
# set startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

#install

# Retrieve the absolute path to qmake and then use that path to find
# the binaries
# get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
# get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
# find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "C:/dev/lib/soundtouch_dll-2.1.1/SoundTouchDLL_x64.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND "${CMAKE_COMMAND}" -E
#         env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
#             "$<TARGET_FILE:${PROJECT_NAME}>" ARGS --qmldir ${CMAKE_SOURCE_DIR}/src/
#     COMMENT "Running windeployqt..."
# )


#useful garbage

#include_directories("C:/dev/lib/glfw-3.3.2.bin.WIN64/include")
#link_directories("C:/dev/lib/glfw-3.3.2.bin.WIN64/lib-vc2019")
#ADD_DEFINITIONS(-DUNICODE)
#ADD_DEFINITIONS(-D_UNICODE)
#target_link_libraries(${PROJECT_NAME} "glfw3")
#target_link_libraries(${PROJECT_NAME} "opengl32.lib")
#target_link_libraries(${PROJECT_NAME} "Dxva2.lib")
#target_link_libraries(${PROJECT_NAME} "d2d1.lib")
#target_link_libraries(${PROJECT_NAME} "Dwmapi.lib")
#target_link_libraries(${PROJECT_NAME} "User32.lib")
#target_link_libraries(${PROJECT_NAME} "Comctl32.lib")
