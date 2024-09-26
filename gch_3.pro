QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Указываем пути к файлам исходного кода
SOURCES += \
#    GanttChart.cpp \
    GanttChart.cpp \
    GanttChartWidget.cpp \
    JsonReader.cpp \
    MainWindow.cpp \
    main.cpp

# Указываем пути к файлам заголовков
HEADERS += \
#    GanttChart.h \
    GanttChart.h \
    GanttChartWidget.h \
    InputData.h \
    JsonReader.h \
    MainWindow.h \
    OutputData.h
#    \
#    Solver.h


# Если есть формы Qt Designer, включаем их здесь
#FORMS +=
FORMS += mainwindow.ui


# Указываем JSON-файл в разделе ресурсов
DISTFILES += \
    .gitignore \
    operation_data.json \
    operation_data_2.json

# Пакеты и библиотеки, которые будут использоваться
QT += core gui widgets

# Подключаем динамическую библиотеку libsolver.so
#LIBS += -L$$PWD/ -lsolver
#LIBS += -L$$PWD/build_gch_3_Desktop_Debug/ -lsolver

# Для работы с библиотекой nlohmann_json (если она подключена локально или установлена)
#LIBS += -lnlohmann_json

# Указываем путь к библиотекам, если требуется
#INCLUDEPATH += /usr/local/include/nlohmann

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
