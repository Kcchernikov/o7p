#include "variable.h"

// Конструктор для создания переменной заданного типа
VarContext::VarContext(TypeContext* type): context(type) {}

// Вывод общей отладочной информации о переменной
void VarContext::debugOut(size_t tabcnt) {
    std::cout << "VAR ";
    // Добавление информации о типе переменной
    context->debugOut(tabcnt);
}

