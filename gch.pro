QT       += core gui
QT       += charts  # Добавляем модуль QtCharts
QT       += printsupport
QT += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17  # Использование стандарта C++17

# Disable warnings
QMAKE_CXXFLAGS += -w

SOURCES += \
    barchartview.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    draggablebarchart.cpp  # Добавьте этот файл

HEADERS += \
    barchartview.h \
    mainwindow.h \
    qcustomplot.h \
    draggablebarchart.h  # Добавьте этот файл

FORMS += \
    mainwindow.ui


CONFIG += moc

#find_package(Qt5 COMPONENTS Widgets Charts REQUIRED)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    data.json
