#ifndef CONST_H
#define CONST_H

#include "context.h"
#include "../object_model/operator.h"

// Класс, определяющий контекст константы.
// Предполагается, что в языке существуют константы основных типов
class ConstContext: public Context {
public:
    // Вывод отладочной информации о контексте импорта
    virtual void debugOut(size_t tabcnt = 0);
};

// Класс, определяющий контекст константы.
template<typename T>
class ConstValueContext: public ConstContext {
public:
    ///ConstValueContext();
    // Создание целочисленной константы
    ConstValueContext(T v): value(v) {}

    // Вывод отладочной информации о целочисленной константе
    virtual void debugOut(size_t tabcnt = 0);

private:
    // Значение константы
    T value;
};

// Класс, определяющий контекст целочисленной константы.
using ConstIntContext = ConstValueContext<int>;

#endif // CONST_H
