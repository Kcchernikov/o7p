#include <iostream>

#include "context.h"

// Установка общего начального контекста
Context::Context(NamedArtefact* na): nameOfArtefact(na) {}

// Вывод отладочной иноформации (при необходимости) об имени контекста
void Context::debugInfoAboutName() {
    if(nameOfArtefact != nullptr) {
        std::cout << "(Name of Context = "
                  << nameOfArtefact->getName()
                  << ")";
    } else {
        std::cout << "(Context don't have a name)";
    }
}

// Формирование именованного артифакта
NamedArtefact::NamedArtefact(std::string name, Context* context, bool access):
    name(name), context(context), access(access)
{}

// Вывод отладочной информации о именованном артефакте
void NamedArtefact::debugOut() {
    std::cout << name << (access? "*: ":": ");
    context->debugOut();
}
