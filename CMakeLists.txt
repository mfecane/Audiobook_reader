cmake_minimum_required(VERSION 3.18)

include_directories(
    "D:/dev/lib/soundtouch_dll-2.1.1"
    "D:/dev/lib/ffmpeg_build/inc"
    "D:/dev/lib/x264/"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    )

link_directories(
    "D:/dev/lib/soundtouch_dll-2.1.1"
    "D:/dev/lib/ffmpeg_build/prefix/lib"
    "D:/dev/lib/x264/installed/lib/"
    )

project("audiobook_reader")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

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
    src/player2.h
    src/player2.cpp
    src/pch.h
    src/pch.cpp
    src/lib.h
    src/lib.cpp

    README.md
    installer/installer.nsi

    ${SRC_UI}
    ${RESOURCES}
    ${QTRESOURCES}
    )

target_precompile_headers(${PROJECT_NAME}
    PUBLIC src/pch.h
    )

source_group("ui" FILES ${SRC_UI})

set(QML_IMPORT_PATH 
    "${CMAKE_SOURCE_DIR}/src/imports" 
	CACHE STRING "" FORCE)

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")

    
add_executable(${PROJECT_NAME}_test1 WIN32
    test1/main.cpp
    src/pch.h
    src/pch.cpp
    src/lib.h
    src/lib.cpp
    src/player2.h
    src/player2.cpp
    )


# Do i really need to link all this SHIT?

set(LIBRARIES
    Qt5::Widgets
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    Qt5::Multimedia
    SoundTouchDLL_x64
    dwmapi
    D:/dev/lib/x264/installed/lib/libx264.lib
    D:/dev/lib/ffmpeg_build/prefix/lib/libavcodec.a
    D:/dev/lib/ffmpeg_build/prefix/lib/libavdevice.a
    D:/dev/lib/ffmpeg_build/prefix/lib/libavfilter.a
    D:/dev/lib/ffmpeg_build/prefix/lib/libavformat.a
    D:/dev/lib/ffmpeg_build/prefix/lib/libavutil.a
    D:/dev/lib/ffmpeg_build/prefix/lib/libpostproc.a
    D:/dev/lib/ffmpeg_build/prefix/lib/libswresample.a
    D:/dev/lib/ffmpeg_build/prefix/lib/libswscale.a
    bcrypt
    gdi32
    kernel32
    user32
    winspool
    comdlg32
    advapi32
    shell32
    ole32
    oleaut32
    uuid
    odbc32
    odbccp32
    Ws2_32
    Secur32
    Strmiids
    Mf
    Mfcore
    Mfplat
    mfplay
    mfreadwrite
    mfsensorgroup
    Mfsrcsnk
    mfuuid
    )

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
target_link_libraries(${PROJECT_NAME}_test1 ${LIBRARIES})

# set startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "D:/dev/lib/soundtouch_dll-2.1.1/SoundTouchDLL_x64.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

#if(CMAKE_BUILD_TYPE STREQUAL "Release")
if(1)

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

endif()
