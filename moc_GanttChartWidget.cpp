/****************************************************************************
** Meta object code from reading C++ file 'GanttChartWidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "GanttChartWidget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GanttChartWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSGanttChartWidgetENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSGanttChartWidgetENDCLASS = QtMocHelpers::stringData(
    "GanttChartWidget",
    "OnSolveButtonClicked",
    "",
    "OnShowScheduleMetricsClicked",
    "OnSolveButtonClicked_SolverRestart",
    "OnZoomOutClicked",
    "OnZoomInClicked",
    "OnZoomInClickedScroll",
    "mousePos",
    "OnZoomOutClickedScroll",
    "findScrollArea",
    "QScrollArea*",
    "scrollBarValueToDouble",
    "QScrollBar*",
    "scrollBar",
    "zoom",
    "doubleToScrollBarValue",
    "value",
    "updateScrollBars"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGanttChartWidgetENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x08,    1 /* Private */,
       3,    0,   81,    2, 0x08,    2 /* Private */,
       4,    0,   82,    2, 0x08,    3 /* Private */,
       5,    0,   83,    2, 0x08,    4 /* Private */,
       6,    0,   84,    2, 0x08,    5 /* Private */,
       7,    1,   85,    2, 0x08,    6 /* Private */,
       9,    1,   88,    2, 0x08,    8 /* Private */,
      10,    0,   91,    2, 0x108,   10 /* Private | MethodIsConst  */,
      12,    2,   92,    2, 0x108,   11 /* Private | MethodIsConst  */,
      16,    2,   97,    2, 0x108,   14 /* Private | MethodIsConst  */,
      18,    0,  102,    2, 0x08,   17 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPointF,    8,
    QMetaType::Void, QMetaType::QPointF,    8,
    0x80000000 | 11,
    QMetaType::Double, 0x80000000 | 13, QMetaType::Double,   14,   15,
    QMetaType::Int, QMetaType::Double, QMetaType::Double,   17,   15,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject GanttChartWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSGanttChartWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGanttChartWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGanttChartWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GanttChartWidget, std::true_type>,
        // method 'OnSolveButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'OnShowScheduleMetricsClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'OnSolveButtonClicked_SolverRestart'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'OnZoomOutClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'OnZoomInClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'OnZoomInClickedScroll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPointF &, std::false_type>,
        // method 'OnZoomOutClickedScroll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPointF &, std::false_type>,
        // method 'findScrollArea'
        QtPrivate::TypeAndForceComplete<QScrollArea *, std::false_type>,
        // method 'scrollBarValueToDouble'
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<QScrollBar *, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'doubleToScrollBarValue'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'updateScrollBars'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void GanttChartWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GanttChartWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->OnSolveButtonClicked(); break;
        case 1: _t->OnShowScheduleMetricsClicked(); break;
        case 2: _t->OnSolveButtonClicked_SolverRestart(); break;
        case 3: _t->OnZoomOutClicked(); break;
        case 4: _t->OnZoomInClicked(); break;
        case 5: _t->OnZoomInClickedScroll((*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 6: _t->OnZoomOutClickedScroll((*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[1]))); break;
        case 7: { QScrollArea* _r = _t->findScrollArea();
            if (_a[0]) *reinterpret_cast< QScrollArea**>(_a[0]) = std::move(_r); }  break;
        case 8: { double _r = _t->scrollBarValueToDouble((*reinterpret_cast< std::add_pointer_t<QScrollBar*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 9: { int _r = _t->doubleToScrollBarValue((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 10: _t->updateScrollBars(); break;
        default: ;
        }
    }
}

const QMetaObject *GanttChartWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GanttChartWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGanttChartWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int GanttChartWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
