/****************************************************************************
** Meta object code from reading C++ file 'GanttChartWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../GanttChartWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GanttChartWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GanttChartWidget_t {
    QByteArrayData data[22];
    char stringdata0[348];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GanttChartWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GanttChartWidget_t qt_meta_stringdata_GanttChartWidget = {
    {
QT_MOC_LITERAL(0, 0, 16), // "GanttChartWidget"
QT_MOC_LITERAL(1, 17, 17), // "statusTextChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 4), // "text"
QT_MOC_LITERAL(4, 41, 10), // "UpdateSize"
QT_MOC_LITERAL(5, 52, 20), // "OnSolveButtonClicked"
QT_MOC_LITERAL(6, 73, 28), // "OnShowScheduleMetricsClicked"
QT_MOC_LITERAL(7, 102, 34), // "OnSolveButtonClicked_SolverRe..."
QT_MOC_LITERAL(8, 137, 16), // "OnZoomOutClicked"
QT_MOC_LITERAL(9, 154, 15), // "OnZoomInClicked"
QT_MOC_LITERAL(10, 170, 21), // "OnZoomInClickedScroll"
QT_MOC_LITERAL(11, 192, 8), // "mousePos"
QT_MOC_LITERAL(12, 201, 22), // "OnZoomOutClickedScroll"
QT_MOC_LITERAL(13, 224, 14), // "findScrollArea"
QT_MOC_LITERAL(14, 239, 12), // "QScrollArea*"
QT_MOC_LITERAL(15, 252, 22), // "scrollBarValueToDouble"
QT_MOC_LITERAL(16, 275, 11), // "QScrollBar*"
QT_MOC_LITERAL(17, 287, 9), // "scrollBar"
QT_MOC_LITERAL(18, 297, 4), // "zoom"
QT_MOC_LITERAL(19, 302, 22), // "doubleToScrollBarValue"
QT_MOC_LITERAL(20, 325, 5), // "value"
QT_MOC_LITERAL(21, 331, 16) // "updateScrollBars"

    },
    "GanttChartWidget\0statusTextChanged\0\0"
    "text\0UpdateSize\0OnSolveButtonClicked\0"
    "OnShowScheduleMetricsClicked\0"
    "OnSolveButtonClicked_SolverRestart\0"
    "OnZoomOutClicked\0OnZoomInClicked\0"
    "OnZoomInClickedScroll\0mousePos\0"
    "OnZoomOutClickedScroll\0findScrollArea\0"
    "QScrollArea*\0scrollBarValueToDouble\0"
    "QScrollBar*\0scrollBar\0zoom\0"
    "doubleToScrollBarValue\0value\0"
    "updateScrollBars"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GanttChartWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   82,    2, 0x0a /* Public */,
       5,    0,   83,    2, 0x08 /* Private */,
       6,    0,   84,    2, 0x08 /* Private */,
       7,    0,   85,    2, 0x08 /* Private */,
       8,    0,   86,    2, 0x08 /* Private */,
       9,    0,   87,    2, 0x08 /* Private */,
      10,    1,   88,    2, 0x08 /* Private */,
      12,    1,   91,    2, 0x08 /* Private */,
      13,    0,   94,    2, 0x08 /* Private */,
      15,    2,   95,    2, 0x08 /* Private */,
      19,    2,  100,    2, 0x08 /* Private */,
      21,    0,  105,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPointF,   11,
    QMetaType::Void, QMetaType::QPointF,   11,
    0x80000000 | 14,
    QMetaType::Double, 0x80000000 | 16, QMetaType::Double,   17,   18,
    QMetaType::Int, QMetaType::Double, QMetaType::Double,   20,   18,
    QMetaType::Void,

       0        // eod
};

void GanttChartWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GanttChartWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->statusTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->UpdateSize(); break;
        case 2: _t->OnSolveButtonClicked(); break;
        case 3: _t->OnShowScheduleMetricsClicked(); break;
        case 4: _t->OnSolveButtonClicked_SolverRestart(); break;
        case 5: _t->OnZoomOutClicked(); break;
        case 6: _t->OnZoomInClicked(); break;
        case 7: _t->OnZoomInClickedScroll((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 8: _t->OnZoomOutClickedScroll((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 9: { QScrollArea* _r = _t->findScrollArea();
            if (_a[0]) *reinterpret_cast< QScrollArea**>(_a[0]) = std::move(_r); }  break;
        case 10: { double _r = _t->scrollBarValueToDouble((*reinterpret_cast< QScrollBar*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 11: { int _r = _t->doubleToScrollBarValue((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 12: _t->updateScrollBars(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GanttChartWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GanttChartWidget::statusTextChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GanttChartWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_GanttChartWidget.data,
    qt_meta_data_GanttChartWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GanttChartWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GanttChartWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GanttChartWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int GanttChartWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void GanttChartWidget::statusTextChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
