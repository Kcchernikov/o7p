#ifndef TYPE_CONTEXT_H
#define TYPE_CONTEXT_H

#include "context.h"

#include <string>

// Класс, задающий обобщение для всех типов языка
class TypeContext: public Context {
public:
    // Получение размера типа
    int getTypeSize() { return typeSize; }

    virtual TypeContext* getType() {
        return this;
    }

    void setImportFrom(std::string imp) {
        importFrom = imp;
    }
    // Вывод отладочной информации о базовом типе
    virtual void debugOut(size_t tabcnt = 0);
protected:
    // Размер типа
    int typeSize;
    // Откуда импортировано
    std::string importFrom;
};

#endif // TYPE_CONTEXT_H