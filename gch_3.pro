QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += release


contains(QT_CONFIG, emscripten) {
    QMAKE_LFLAGS += -s ALLOW_MEMORY_GROWTH=1
}



# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Указываем пути к файлам исходного кода
SOURCES += \
    GlobalState.cpp \
    GanttChart.cpp \
    GanttChartWidget.cpp \
    MainWindow.cpp \
    main.cpp \
    #SolverExport.cpp 

# Указываем пути к файлам заголовков
HEADERS += \
    GlobalState.h \
    GanttChart.h \
    GanttChartWidget.h \
    MainWindow.h \
    InputData.h \
    Solver.h \
    OutputData.h 
    #SolverExport.h

# Если есть формы Qt Designer, включаем их здесь
# FORMS +=
FORMS += mainwindow.ui
RESOURCES += resources.qrc

# Указываем JSON-файл в разделе ресурсов
DISTFILES += \
    .gitignore \
    .gitignore \
    operation_data.json \
    operation_data_2.json \
    resources/icons8-ракета-64.png

# Пакеты и библиотеки, которые будут использоваться
QT += core gui widgets

# Указываем путь к библиотекам, если требуется
INCLUDEPATH += $$PWD/json/include

# Указываем путь к динамической библиотеке (для сборки)
LIBS += -L$$PWD -lSchedulingSolver

# Указываем путь поиска для динамических библиотек (при выполнении программы)
LIBS += -Wl,-rpath,$$PWD


CONFIG += debug

#CONFIG += wasm

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
