#include "procedure.h"
#include "declaration.h"

// void FormalParameters::addFPSection(FPSection* fps) {
//     parameters.push_back(fps);
// }

// void FormalParameters::setResultType(TypeContext *ctx) {
//     resultType = ctx;
// }

// void FPSection::debugOut(size_t tabcnt) {
//     std::cout << "FPSection";
// }

// void FormalParameters::debugOut(size_t tabcnt) {
//     for (auto p : parameters) {
//         p->debugOut(tabcnt);
//         std::cout << " ";
//     }
//     if (resultType) {
//         std::cout << ": ";
//         resultType->debugOut(tabcnt);
//     }
//     std::cout << "\n";
// }

void Procedure::setIdent(const Identdef& id) {
    ident = id;
}

Identdef Procedure::getIdent() {
    return ident;
}

void Procedure::setBody(ProcedureBody bd) {
    body = std::move(bd);
}
 
void Procedure::debugOut(size_t tabcnt) {
    ProcContext::debugOut(tabcnt);
    // if (heading.parameters) {
        // heading.parameters->debugOut(tabcnt);
    // }
    std::cout << "\nBODY:\n";
    declaration->debugOut(tabcnt + 1);
    if (body.statement) {
        body.statement->debugOut(tabcnt + 1);
    }
    if (body.result) {
        std::cout << "RESULT ";
        body.result->debugOut(0);
    }
    // ProcContext::getDeclaration()
}