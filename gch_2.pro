#QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT       += core gui widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    rectangle.cpp \
    secondwindow.cpp \
    solver_program/solver.cpp

HEADERS += \
    mainwindow.h \
    rectangle.h \
    secondwindow.h \
    solver.h \
    solver_program/solver.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitignore \
    README \
    solver_program/CMakeLists.txt \
    solver_program/inputdata.json \
    solver_program/outputdata.json
