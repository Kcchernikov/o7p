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
    // генерация кода
    virtual void generate(Generator* gen, std::stringstream& cur, const std::string& name);

    virtual std::string getTypeName() const {
        return "TypeBoolContext";
    }
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
    // генерация кода
    virtual void generate(Generator* gen, std::stringstream& cur, const std::string& name);
    virtual std::string getTypeName() const {
        return "TypeIntegerContext";
    }
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
    // генерация кода
    virtual void generate(Generator* gen, std::stringstream& cur, const std::string& name);
    virtual std::string getTypeName() const {
        return "TypeRealContext";
    }
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
    // генерация кода
    virtual void generate(Generator* gen, std::stringstream& cur, const std::string& name);
    virtual std::string getTypeName() const {
        return "TypeCharContext";
    }
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
    // генерация кода
    virtual void generate(Generator* gen, std::stringstream& cur, const std::string& name);
    virtual std::string getTypeName() const {
        return "TypeByteContext";
    }
};

// Класс, определяющий строковый тип
// Возможно надо будет удалить и оставить только массив char
class TypeStringContext: public TypeContext {
public:
    // Создание типа и определение его размера
    TypeStringContext(size_t len = 0): length(len) {
        typeSize = sizeof(char);
    }

    virtual TypeContext* getType() {
        return this;
    }

    size_t getLenght() const {
        return length;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
    // генерация кода
    virtual void generate(Generator* gen, std::stringstream& cur, const std::string& name);
    virtual std::string getTypeName() const {
        return "TypeStringContext";
    }
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
    // генерация кода
    virtual void generate(Generator* gen, std::stringstream& cur, const std::string& name);
    virtual std::string getTypeName() const {
        return "TypeNilContext";
    }
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
    // генерация кода
    virtual void generate(Generator* generator, std::stringstream& cur, const std::string& name);
    virtual std::string getTypeName() const {
        return "TypeSetContext";
    }
};

// Класс, определяющий тип "запись"
class TypeRecordContext: public TypeContext {
    friend class GeneratorC;
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

    virtual void generate(Generator* gen, std::stringstream& cur, const std::string& name);

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
        TypeRecordContext* rec = (ctx->getTypeName() == "TypeRecordContext"
            ? dynamic_cast<TypeRecordContext*>(ctx)
            : nullptr
        );
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
    virtual std::string getTypeName() const {
        return "TypeRecordContext";
    }
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
    friend class GeneratorC;
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
        TypePointerContext* p = (ctx->getTypeName() == "TypePointerContext"
            ? dynamic_cast<TypePointerContext*>(ctx)
            : nullptr
        );
        if (!p) {
            return false;
        }
        return recordType->isOneOfParents(p->getRecord());
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
    virtual void generate(Generator* gen, std::stringstream& cur, const std::string& name);
    virtual std::string getTypeName() const {
        return "TypePointerContext";
    }
private:
    TypeRecordContext* recordType;    // указатель ссылается только на запись
};

class Expression;

// Класс, определяющий тип - массив
class TypeArrayContext: public TypeContext {
    friend class GeneratorC;
public:
    // Создание типа и определение его размера
    TypeArrayContext(size_t len, TypeContext* v, bool allowEmpty = false);

    TypeArrayContext(TypeContext* v, Expression* expLen, size_t len = 0);

    TypeArrayContext(TypeContext* v, DeclarationSequence* ds, size_t len = 0);

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
            if (valueType == arr->getElementType() || valueType == nullptr || arr->getElementType() == nullptr) {
                return true;
            }
            TypeArrayContext* value1 = (valueType->getTypeName() == "TypeArrayContext"
                ? dynamic_cast<TypeArrayContext*>(valueType)
                : nullptr
            );
            TypeArrayContext* value2 = (arr->getElementType()->getTypeName() == "TypeArrayContext"
                ? dynamic_cast<TypeArrayContext*>(arr->getElementType())
                : nullptr
            );
            if (value1 && value2) {
                return value1->isEqual(value2);
            }
        }
        return false;
    }

    // Вывод отладочной информации о типе
    virtual void debugOut(size_t tabcnt = 0);
    virtual void generate(Generator* gen, std::stringstream& cur, const std::string& name);
    virtual std::string getTypeName() const {
        return "TypeArrayContext";
    }
private:
    size_t length; // значение 0 указывает о произвольной длине
    Expression* expLen; // Длина, которая может быть задана выражением
    TypeContext* valueType;    // указатель ссылается только на запись, nullptr означает любой подходящий тип
};

#endif // TYPE_H
