#include <string>
#include <cassert>

#include "const.h"

// Вывод отладочной информации об общем контексте константы
void ConstContext::debugOut(size_t tabcnt) {
    std::cout << "CONST ";
}

// Вывод отладочной информации о целочисленной константе
template<>
void ConstIntContext::debugOut(size_t tabcnt) {
    ConstContext::debugOut(tabcnt);
    std::cout << "INT " << value; // << std::endl;
}

/*
// Вывод отладочной информации о константе
template<typename T>
void ConstValueContext<T>::debugOut() {
    ConstContext::debugOut();
    std::cout << value; // << std::endl;
}
*/

// Вывод отладочной информации о булевской константе
template<>
void ConstValueContext<bool>::debugOut(size_t tabcnt) {
    ConstContext::debugOut(tabcnt);
    std::cout << (value? "true":"false"); // << std::endl;
}

// Вывод отладочной информации о действительной константе
template<>
void ConstValueContext<double>::debugOut(size_t tabcnt) {
    ConstContext::debugOut(tabcnt);
    std::cout << value; // << std::endl;
}

// Вывод отладочной информации о символьной константе
template<>
void ConstValueContext<std::string>::debugOut(size_t tabcnt) {
    ConstContext::debugOut(tabcnt);
    std::cout << "\"" << value << "\""; // << std::endl;
}