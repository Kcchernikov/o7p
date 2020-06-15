#ifndef OPERAND_H
#define OPERAND_H

#include "context.h"

// Класс, определяющий основу для различных операндов, обрабатываемых инструкциями
class Operand
{
public:
    Operand();

    // Вывод отладочной информации
    virtual void debugOut();
private:
    Context* context;   // назначение операнда
};

#endif // OPERAND_H
