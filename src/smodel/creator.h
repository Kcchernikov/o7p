#ifndef CREATOR_H
#define CREATOR_H

#include <string>

#include "context.h"
#include "import.h"
#include "const.h"
#include "type.h"
#include "variable.h"
#include "module.h"


// Класс, обеспечивающий создание различных артефактов
class Creator
{
public:
    Creator();

    // Создание контекста импорта
    ImportContext* CreateImportContext(std::string importedName, std::string alias);

    // Создание целочисленной константы
    ConstContext* CreateConstInt(int v);
    // Создание действительной константы
    ConstContext* CreateConstReal(double v);
    // Создание булевской константы
    ConstContext* CreateConstBool(bool v);
    // Создание строковой константы
    ConstContext* CreateConstString(std::string v);

    // Создание целочисленного типа
    TypeIntegerContext* CreateTypeInt();
    // Создание действительного типа
    TypeRealContext* CreateTypeReal();
    // Создание булевского типа
    TypeBoolContext* CreateTypeBool();
    // Создание символьного типа
    TypeCharContext* CreateTypeChar();
    // Создание множественного типа
    TypeSetContext* CreateTypeSet();

    // Создание типа Запись (пустой оболочки)
    TypeRecordContext* CreateTypeRecord(TypeRecordContext* parent = nullptr);

    // Создание типа-указателя на запись
    TypePointerContext* CreateTypePointer(TypeRecordContext* r);

    // Создание переменной с установкой соответствующего типа
    VarContext* CreateVariable(TypeContext* type);
};

#endif // CREATOR_H
