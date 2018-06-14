project(QtUI)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
MESSAGE( STATUS "CMake Version: " ${CMAKE_VERSION} )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui OpenGL Svg Sql Charts) 

set(QtUI_SRCS   mainwindow.cpp 
                openglwidget.cpp
                fbxexporterdialog.cpp
                balloonwidget.cpp balloondialog.cpp balloonmessagedialog.cpp balloonvaluedialog.cpp
                overwritedialog.cpp
                customtableview.cpp
                validateddelegate.cpp intvalidatorreporter.cpp doublevalidatorreporter.cpp controlpointvalidator.cpp
                activelayertreeview.cpp activelayermodel.cpp activelayerdelegate.cpp activelayerbuttondelegate.cpp activelayeritem.cpp
                disklayertreeview.cpp disklayermodel.cpp disklayeritemdelegate.cpp
                opacityeditorwidget.cpp 
                palettevaluewidget.cpp 
                palettegraphicsview.cpp
                paletteeditorwidget.cpp palettecreatorwidget.cpp palettetableview.cpp palettemodel.cpp 
                palettelistdelegate.cpp palettelistmodel.cpp palettelistview.cpp 
                createlayerdialog.cpp lightingdialog.cpp
                databaseresourceview.cpp databaseresourcemodel.cpp
                databasetableschemaview.cpp databasetableschemamodel.cpp databasetableschemadelegate.cpp
                databasetabledataview.cpp databaserowdataview.cpp databasetabledatamodel.cpp databasetabledatadelegate.cpp customspinbox.cpp
                visiblecolumnview.cpp visiblecolumnmodel.cpp visiblecolumndelegate.cpp
                columnselectionmodel.cpp
                databasequeryview.cpp
                tableproxyfilter.cpp
                databaseeditorwidget.cpp
                columnvisibilitywidget.cpp columnvisibilitymodel.cpp
                vsizedlayout.cpp hsizedlayout.cpp
                layeroperationwidget.cpp customtextedit.cpp
                histogramdialog.cpp areapervaluemodel.cpp
    )

add_library(QtUI ${QtUI_SRCS})
target_compile_definitions(QtUI PUBLIC GLM_ENABLE_EXPERIMENTAL)
target_link_libraries(QtUI Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::Sql Qt5::Charts)
target_include_directories(QtUI INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/QtUI_autogen/include)
