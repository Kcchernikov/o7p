#include "operand.h"

Operand::Operand()
{

}

// Вывод отладочной информации
void Operand::debugOut(size_t tabcnt) {
    std::cout << "OPERAND ";
    context->debugOut(tabcnt);
}
