#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <vector>

#include "type_context.h"
#include "../object_model/declaration.h"

// Класс, определяющий булевский тип
class TypeBoolContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypeBoolContext() {
        typeSize = sizeof(bool);
    }

    virtual TypeContext* getType() {
        return this;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
};

// Класс, определяющий целочисленный тип
class TypeIntegerContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypeIntegerContext() {
        typeSize = sizeof(int);
    }

    virtual TypeContext* getType() {
        return this;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
};

// Класс, определяющий действительный тип
class TypeRealContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypeRealContext() {
        typeSize = sizeof(double);
    }

    virtual TypeContext* getType() {
        return this;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
};

// Класс, определяющий символьный тип
class TypeCharContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypeCharContext() {
        typeSize = sizeof(char);
    }

    virtual TypeContext* getType() {
        return this;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
};

// Класс, определяющий байтовый тип
class TypeByteContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypeByteContext() {
        typeSize = sizeof(char);
    }

    virtual TypeContext* getType() {
        return this;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
};

// Класс, определяющий строковый тип
// Возможно надо будет удалить и оставить только массив char
class TypeStringContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypeStringContext() {
        typeSize = sizeof(char);
    }

    virtual TypeContext* getType() {
        return this;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
private:
    size_t length; // значение 0 указывает о произвольной длине
};

// Класс, определяющий NIL
class TypeNilContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypeNilContext() {
        typeSize = sizeof(int*);
    }

    virtual TypeContext* getType() {
        return this;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
};

// Класс, определяющий множественный тип
class TypeSetContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypeSetContext() {
        typeSize = sizeof(unsigned);
    }

    virtual TypeContext* getType() {
        return this;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
};

// Класс, определяющий тип "запись"
class TypeRecordContext: public TypeContext {
public:
    // Создание оболочки типа и задание его контекста
    TypeRecordContext(TypeRecordContext* p = nullptr, bool init = true): parentContext(p), isInit(init) {
        if (p) {
            declaration = DeclarationSequence(p->getDeclaration());
            typeSize = p->typeSize;
        } else {
            typeSize = 0;
        }
    }

    virtual TypeContext* getType() {
        return this;
    }

    // Добавление к записи именованного поля
    void AddNamedModuleField(std::string name, TypeContext* typeContext, bool access = true);

    // Получение артефакта по имени
    NamedArtefact* getArtefactByName(const std::string& ident);

    // Получение DeclarationSequence*
    DeclarationSequence* getDeclaration() {
        return &declaration;
    }

    TypeRecordContext* getParent() {
        return parentContext;
    }

    void incTypeSize(size_t sz) {
        typeSize += sz;
    }

    bool getInit() const {
        return isInit;
    }

    void setInit(bool init) {
        isInit = init;
    }

    bool isOneOfParents(TypeContext* ctx) {
        TypeRecordContext* rec = dynamic_cast<TypeRecordContext*>(ctx);
        if (!rec) {
            return false;
        }
        TypeRecordContext* cur = this;
        while (cur && cur != rec) {
            cur = cur->getParent();
        }
        if (!cur) {
            return false;
        } else {
            return true;
        }
    }

    // Вывод отладочной информации о записи
    virtual void debugOut(size_t tabcnt = 0);
private:
    // Родительский контекст (если присутствует, иначе nullptr)
    TypeRecordContext* parentContext;
    // Список именованных типов записи, объявленной в модуле
    // std::vector<NamedArtefact> namedFields;
    // Список именованных типов записи
    DeclarationSequence declaration;
    bool isInit;
};

// Класс, определяющий тип - указатель
class TypePointerContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypePointerContext(TypeRecordContext* r = nullptr): recordType(r) {
        typeSize = sizeof(int*);
    }

    virtual TypeContext* getType() {
        return this;
    }

    void setRecord(TypeRecordContext* r) {
        recordType = r;
    }

    TypeRecordContext* getRecord() {
        return recordType;
    }

    bool isOneOfParents(TypeContext* ctx) {
        TypePointerContext* p = dynamic_cast<TypePointerContext*>(ctx);
        if (!p) {
            return false;
        }
        return recordType->isOneOfParents(p->getRecord());
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
private:
    TypeRecordContext* recordType;    // указатель ссылается только на запись
};

// Класс, определяющий тип - массив
class TypeArrayContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypeArrayContext(size_t len, TypeContext* v): length(len), valueType(v) {
        if (v) {
            typeSize = v->getTypeSize() * len;
        } else {
            std::cout << "\e[1;31m ERROR: VALUE TYPE IS EMPTY \e[0m" << std::endl;
        }
    }

    virtual TypeContext* getType() {
        return this;
    }

    size_t getLenght() {
        return length;
    }

    TypeContext* getElementType() {
        return valueType;
    }

    bool isEqual(TypeArrayContext* arr) {
        if (length == 0 || arr->getLenght() == 0 || length == arr->getLenght()) {
            if (valueType == arr->getElementType()) {
                return true;
            }
            TypeArrayContext* value1 = dynamic_cast<TypeArrayContext*>(valueType);
            TypeArrayContext* value2 = dynamic_cast<TypeArrayContext*>(arr->getElementType());
            if (value1 && value2) {
                return value1->isEqual(value2);
            }
        }
        return false;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
private:
    size_t length; // значение 0 указывает о произвольной длине
    TypeContext* valueType;    // указатель ссылается только на запись
};

#endif // TYPE_H
