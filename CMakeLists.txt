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

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS SvgWidgets)
endif()

# ================== X11 ==================
find_package(X11 REQUIRED)

# ================== SOURCES ==================
set(PROJECT_SOURCES
    src/main.cpp

    src/ui/MainWindow.cpp
    src/ui/MainWindow.h

    src/ui/ClickableLabel.cpp
    src/ui/ClickableLabel.h

    src/ui/FocusCard.cpp
    src/ui/FocusCard.h

    src/ui/AppCard.cpp
    src/ui/AppCard.h

    src/ui/ClickableFrame.cpp
    src/ui/ClickableFrame.h

    src/ui/CountdownWidget.cpp
    src/ui/CountdownWidget.h

    src/ui/ImageCard.cpp
    src/ui/ImageCard.h

    src/ui/DashboardPage.cpp
    src/ui/DashboardPage.h

    src/ui/WorkflowSetupPage.cpp
    src/ui/WorkflowSetupPage.h

    src/ui/CustomSlider.cpp
    src/ui/CustomSlider.h

    src/ui/EmojiSelector.cpp
    src/ui/EmojiSelector.h

    src/core/Workflow.cpp
    src/core/Workflow.h

    src/core/WindowTracker.cpp
    src/core/WindowTracker.hpp

    src/core/LogParser.cpp
    src/core/LogParser.hpp

    src/core/AttentionAnalyzer.cpp
    src/core/AttentionAnalyzer.hpp

    src/core/AppUsageAnalyzer.cpp
    src/core/AppUsageAnalyzer.hpp

    src/core/SystemHandler.cpp
    src/core/SystemHandler.hpp

    src/core/EfficiencyMeter.cpp
    src/core/EfficiencyMeter.hpp

    src/utils/GraphicGenerator.cpp
    src/utils/GraphicGeneratorStructs.h

    src/core/SystemController.cpp
    src/core/SystemController.hpp

    src/ui/InputWindow.cpp
    src/ui/InputWindow.h

    src/ui/OutputWindow.cpp
    src/ui/OutputWindow.h

    src/ui/WorkflowPage.cpp
    src/ui/WorkflowPage.h

    src/ui/ResultPage.cpp
    src/ui/ResultPage.h

    src/ui/TemplatesPage.cpp
    src/ui/TemplatesPage.h

    src/ui/HistoryPage.cpp
    src/ui/HistoryPage.h

    src/ui/WorkflowCard.cpp
    src/ui/WorkflowCard.h

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${X11_INCLUDE_DIR}
)

# ================== LINK ==================
target_link_libraries(fp_project1 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    ${X11_LIBRARIES}
)
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    target_link_libraries(fp_project1 PRIVATE Qt${QT_VERSION_MAJOR}::SvgWidgets)
endif()

# ================== INSTALL ==================
include(GNUInstallDirs)
install(TARGETS fp_project1
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
