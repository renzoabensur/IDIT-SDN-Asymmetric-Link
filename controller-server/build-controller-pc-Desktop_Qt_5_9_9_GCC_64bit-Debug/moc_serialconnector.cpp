/****************************************************************************
** Meta object code from reading C++ file 'serialconnector.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../controller-pc/serialconnector.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialconnector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SerialConnector_t {
    QByteArrayData data[15];
    char stringdata0[165];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialConnector_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialConnector_t qt_meta_stringdata_SerialConnector = {
    {
QT_MOC_LITERAL(0, 0, 15), // "SerialConnector"
QT_MOC_LITERAL(1, 16, 22), // "state_changed_stringfy"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 5), // "state"
QT_MOC_LITERAL(4, 46, 14), // "nodeIdReceived"
QT_MOC_LITERAL(5, 61, 10), // "sdnaddr_t*"
QT_MOC_LITERAL(6, 72, 6), // "nodeId"
QT_MOC_LITERAL(7, 79, 12), // "printfString"
QT_MOC_LITERAL(8, 92, 6), // "string"
QT_MOC_LITERAL(9, 99, 13), // "defaultPacket"
QT_MOC_LITERAL(10, 113, 6), // "packet"
QT_MOC_LITERAL(11, 120, 12), // "customPacket"
QT_MOC_LITERAL(12, 133, 11), // "ackReceived"
QT_MOC_LITERAL(13, 145, 6), // "status"
QT_MOC_LITERAL(14, 152, 12) // "nackReceived"

    },
    "SerialConnector\0state_changed_stringfy\0"
    "\0state\0nodeIdReceived\0sdnaddr_t*\0"
    "nodeId\0printfString\0string\0defaultPacket\0"
    "packet\0customPacket\0ackReceived\0status\0"
    "nackReceived"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialConnector[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,
       7,    1,   55,    2, 0x06 /* Public */,
       9,    1,   58,    2, 0x06 /* Public */,
      11,    1,   61,    2, 0x06 /* Public */,
      12,    1,   64,    2, 0x06 /* Public */,
      14,    1,   67,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QByteArray,   10,
    QMetaType::Void, QMetaType::QByteArray,   10,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   13,

       0        // eod
};

void SerialConnector::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SerialConnector *_t = static_cast<SerialConnector *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->state_changed_stringfy((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->nodeIdReceived((*reinterpret_cast< sdnaddr_t*(*)>(_a[1]))); break;
        case 2: _t->printfString((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->defaultPacket((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 4: _t->customPacket((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 5: _t->ackReceived((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->nackReceived((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (SerialConnector::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialConnector::state_changed_stringfy)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SerialConnector::*_t)(sdnaddr_t * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialConnector::nodeIdReceived)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SerialConnector::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialConnector::printfString)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (SerialConnector::*_t)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialConnector::defaultPacket)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (SerialConnector::*_t)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialConnector::customPacket)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (SerialConnector::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialConnector::ackReceived)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (SerialConnector::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialConnector::nackReceived)) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject SerialConnector::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SerialConnector.data,
      qt_meta_data_SerialConnector,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SerialConnector::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialConnector::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SerialConnector.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SerialConnector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void SerialConnector::state_changed_stringfy(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialConnector::nodeIdReceived(sdnaddr_t * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SerialConnector::printfString(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SerialConnector::defaultPacket(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SerialConnector::customPacket(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SerialConnector::ackReceived(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void SerialConnector::nackReceived(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
