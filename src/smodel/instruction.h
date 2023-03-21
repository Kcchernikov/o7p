#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <list>

#include "operand.h"

// Коды операций, ипользуемые в командах семанитической модели
enum OperationCode {
    Add,    // сложение
    Sub,    // вычитание
    Min,    // смена знака
    Mult,   // умножение
    Div,    // деление
    Mod,    // остаток
    And,    // логическое И
    Or,     // логическое ИЛИ
    Not,    // логическое НЕ
    Xor,    // исключающее ИЛИ
    In,     // принадлежность
    Goto,   // безусловный переход
    If,     // переход, если TRUE
    IfNot,  // переход, если FALSE
    Call,   // вызов процедуры или функции
    Nop     // пустая команда
};

class Instruction {
public:
    Instruction();

    // Вывод отладочной информации
    virtual void debugOut(size_t tabcnt = 0) = 0;
private:
    OperationCode opc;  // код операции
    Operand* argument1;  // 1-й аргумент команды (если есть)
    Operand* argument2;  // 2-й аргумент команды (если есть)
    Operand* result;    // результат выполнения команды (если есть)
    Instruction* next; // следующая команда по отношению расположения/управления
};

// Список команд, определяющих блок
class InstructionList {
public:
    InstructionList();
private:
    std::list<Instruction*> iList;   // список команд
    Operand* opd;               // указатель на последний результат
};

#endif // INSTRUCTION_H
