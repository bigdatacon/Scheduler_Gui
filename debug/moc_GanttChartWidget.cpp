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
    QByteArrayData data[24];
    char stringdata0[366];
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
QT_MOC_LITERAL(5, 52, 7), // "setZoom"
QT_MOC_LITERAL(6, 60, 9), // "zoomLevel"
QT_MOC_LITERAL(7, 70, 20), // "OnSolveButtonClicked"
QT_MOC_LITERAL(8, 91, 28), // "OnShowScheduleMetricsClicked"
QT_MOC_LITERAL(9, 120, 34), // "OnSolveButtonClicked_SolverRe..."
QT_MOC_LITERAL(10, 155, 16), // "OnZoomOutClicked"
QT_MOC_LITERAL(11, 172, 15), // "OnZoomInClicked"
QT_MOC_LITERAL(12, 188, 21), // "OnZoomInClickedScroll"
QT_MOC_LITERAL(13, 210, 8), // "mousePos"
QT_MOC_LITERAL(14, 219, 22), // "OnZoomOutClickedScroll"
QT_MOC_LITERAL(15, 242, 14), // "findScrollArea"
QT_MOC_LITERAL(16, 257, 12), // "QScrollArea*"
QT_MOC_LITERAL(17, 270, 22), // "scrollBarValueToDouble"
QT_MOC_LITERAL(18, 293, 11), // "QScrollBar*"
QT_MOC_LITERAL(19, 305, 9), // "scrollBar"
QT_MOC_LITERAL(20, 315, 4), // "zoom"
QT_MOC_LITERAL(21, 320, 22), // "doubleToScrollBarValue"
QT_MOC_LITERAL(22, 343, 5), // "value"
QT_MOC_LITERAL(23, 349, 16) // "updateScrollBars"

    },
    "GanttChartWidget\0statusTextChanged\0\0"
    "text\0UpdateSize\0setZoom\0zoomLevel\0"
    "OnSolveButtonClicked\0OnShowScheduleMetricsClicked\0"
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
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   87,    2, 0x0a /* Public */,
       5,    1,   88,    2, 0x0a /* Public */,
       7,    0,   91,    2, 0x08 /* Private */,
       8,    0,   92,    2, 0x08 /* Private */,
       9,    0,   93,    2, 0x08 /* Private */,
      10,    0,   94,    2, 0x08 /* Private */,
      11,    0,   95,    2, 0x08 /* Private */,
      12,    1,   96,    2, 0x08 /* Private */,
      14,    1,   99,    2, 0x08 /* Private */,
      15,    0,  102,    2, 0x08 /* Private */,
      17,    2,  103,    2, 0x08 /* Private */,
      21,    2,  108,    2, 0x08 /* Private */,
      23,    0,  113,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPointF,   13,
    QMetaType::Void, QMetaType::QPointF,   13,
    0x80000000 | 16,
    QMetaType::Double, 0x80000000 | 18, QMetaType::Double,   19,   20,
    QMetaType::Int, QMetaType::Double, QMetaType::Double,   22,   20,
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
        case 2: _t->setZoom((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->OnSolveButtonClicked(); break;
        case 4: _t->OnShowScheduleMetricsClicked(); break;
        case 5: _t->OnSolveButtonClicked_SolverRestart(); break;
        case 6: _t->OnZoomOutClicked(); break;
        case 7: _t->OnZoomInClicked(); break;
        case 8: _t->OnZoomInClickedScroll((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 9: _t->OnZoomOutClickedScroll((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 10: { QScrollArea* _r = _t->findScrollArea();
            if (_a[0]) *reinterpret_cast< QScrollArea**>(_a[0]) = std::move(_r); }  break;
        case 11: { double _r = _t->scrollBarValueToDouble((*reinterpret_cast< QScrollBar*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 12: { int _r = _t->doubleToScrollBarValue((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 13: _t->updateScrollBars(); break;
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
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
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
