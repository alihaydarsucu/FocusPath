cmake_minimum_required(VERSION 3.16)

project(fp_project1 VERSION 0.1 LANGUAGES CXX)

# ================== C++ ==================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for IDE IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ================== Qt ==================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# ================== X11 ==================
find_package(X11 REQUIRED)

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
    app_usage_analyzer.hpp

    system_handler.cpp
    system_handler.hpp

    efficiency_meter.cpp
    efficiency_meter.hpp

    system_controller.cpp
    system_controller.hpp

    inputwindow.cpp
    inputwindow.h

    outputwindow.cpp
    outputwindow.h

    workflowpage.cpp
    workflowpage.h

    resultpage.cpp
    resultpage.h

    templatespage.cpp
    templatespage.h

    WorkflowCard.cpp
    WorkflowCard.h

    resources.qrc
)

# ================== EXECUTABLE ==================
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(fp_project1
        ${PROJECT_SOURCES}
    )
else()
    add_executable(fp_project1
        ${PROJECT_SOURCES}
    )
endif()

# ================== INCLUDE ==================
target_include_directories(fp_project1 PRIVATE
    ${X11_INCLUDE_DIR}
)

# ================== LINK ==================
target_link_libraries(fp_project1 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${X11_LIBRARIES}
)

# ================== INSTALL ==================
include(GNUInstallDirs)
install(TARGETS fp_project1
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
