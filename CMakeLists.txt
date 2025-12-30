cmake_minimum_required(VERSION 3.16)

project(fp_project1 VERSION 0.1 LANGUAGES CXX)

# ================== C++ ==================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ================== Qt ==================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# ================== SOURCES ==================
set(PROJECT_SOURCES
    main.cpp

    mainwindow.cpp
    mainwindow.h

    clickablelabel.cpp
    clickablelabel.h

    FocusCard.cpp
    FocusCard.h

    imagecard.cpp
    imagecard.h

    dashboardpage.cpp
    dashboardpage.h

    workflowsetuppage.cpp
    workflowsetuppage.h

    workflow.cpp
    workflow.h

    window_tracker.cpp
    window_tracker.hpp

    log_parser.cpp
    log_parser.hpp

    attention_analyzer.cpp
    attention_analyzer.hpp

    app_usage_analyzer.cpp
    attention_analyzer.hpp

    system_handler.hpp
    system_handler.cpp

    efficiency_meter.cpp
    efficiency_meter.hpp
)

# ================== EXECUTABLE ==================
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(fp_project1
        ${PROJECT_SOURCES}
    )
else()
    add_executable(fp_
project1
        ${PROJECT_SOURCES}
        outputwindow.h outputwindow.cpp
        inputwindow.h inputwindow.cpp
    )
endif()

# ================== LINK ==================
target_link_libraries(fp_project1
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
)

# ================== INSTALL (Linux için önemli) ==================
include(GNUInstallDirs)
install(TARGETS fp_project1
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

