#include "declaration.h"
#include "../generator/generator.h"

#include <cmath>

#define COUT(tabcnt) CoutTabs(tabcnt); std::cout

void CoutTabs(size_t tabcnt) {
    for (size_t i = 0; i < tabcnt; ++i) {
        std::cout << "    ";
    }
}

class ConstABS: public ConstProcedure {
public:
    ConstFactor* execute(ConstFactor* arg, DeclarationSequence* resultDecl) const override {
        const auto value = arg->getValue();
        if (value.index() == 0) {
            return new ConstFactor(static_cast<long long>(abs(std::get<0>(value))), resultDecl);
        } else if (value.index() == 1) {
            return new ConstFactor(static_cast<double>(abs(std::get<1>(value))), resultDecl);
        } else {
            assert(false && "Incorect argument for constant ABS function");  
        }
    }
    void debugOut(size_t tabcnt = 0) const override {
        std::cout << "ABS";
    }
};

class ConstODD: public ConstProcedure {
public:
    ConstFactor* execute(ConstFactor* arg, DeclarationSequence* resultDecl) const override {
        const auto value = arg->getValue();
        if (value.index() == 0) {
            return new ConstFactor(static_cast<bool>(std::get<0>(value) % 2 == 1), resultDecl);
        } else {
            assert(false && "Incorect argument for constant ODD function");  
        }
    }
    void debugOut(size_t tabcnt = 0) const override {
        std::cout << "ODD";
    }
};

class ConstLSL: public ConstProcedure {
public:
    ConstFactor* execute(ConstFactor* arg1, ConstFactor* arg2, DeclarationSequence* resultDecl) const override {
        const auto value1 = arg1->getValue();
        const auto value2 = arg2->getValue();
        if (value1.index() == 0 && value2.index() == 0) {
            return new ConstFactor(static_cast<long long>(std::get<0>(value1) << std::get<0>(value2)), resultDecl);
        } else {
            assert(false && "Incorect arguments for constant LSL function");  
        }
    }
    void debugOut(size_t tabcnt = 0) const override {
        std::cout << "LSL";
    }
};

class ConstASR: public ConstProcedure {
public:
    ConstFactor* execute(ConstFactor* arg1, ConstFactor* arg2, DeclarationSequence* resultDecl) const override {
        const auto value1 = arg1->getValue();
        const auto value2 = arg2->getValue();
        if (value1.index() == 0 && value2.index() == 0) {
            return new ConstFactor(static_cast<long long>(std::get<0>(value1) >> std::get<0>(value2)), resultDecl);
        } else {
            assert(false && "Incorect arguments for constant ASR function");  
        }
    }
    void debugOut(size_t tabcnt = 0) const override {
        std::cout << "ASR";
    }
};

class ConstROR: public ConstProcedure {
public:
    ConstFactor* execute(ConstFactor* arg1, ConstFactor* arg2, DeclarationSequence* resultDecl) const override {
        const auto value1 = arg1->getValue();
        const auto value2 = arg2->getValue();
        if (value1.index() == 0 && value2.index() == 0) {
            size_t bit_cnt = (sizeof(long long) * 8);
            size_t n = std::get<0>(value2) % bit_cnt;
            long long x = std::get<0>(value1);
            long long result = (x >> n) + ((((1ll << n) - 1) & x) << (bit_cnt - n));
            return new ConstFactor(static_cast<long long>(result), resultDecl);
        } else {
            assert(false && "Incorect arguments for constant ROR function");  
        }
    }
    void debugOut(size_t tabcnt = 0) const override {
        std::cout << "ROR";
    }
};

class ConstFLOOR: public ConstProcedure {
public:
    ConstFactor* execute(ConstFactor* arg, DeclarationSequence* resultDecl) const override {
        const auto value = arg->getValue();
        if (value.index() == 1) {
            return new ConstFactor(static_cast<long long>(std::floor(std::get<1>(value))), resultDecl);
        } else {
            assert(false && "Incorect argument for constant FLOOR function");  
        }
    }
    void debugOut(size_t tabcnt = 0) const override {
        std::cout << "FLOOR";
    }
};

class ConstFLT: public ConstProcedure {
public:
    ConstFactor* execute(ConstFactor* arg, DeclarationSequence* resultDecl) const override {
        const auto value = arg->getValue();
        if (value.index() == 0) {
            return new ConstFactor(static_cast<double>(std::get<0>(value)), resultDecl);
        } else {
            assert(false && "Incorect argument for constant FLT function");  
        }
    }
    void debugOut(size_t tabcnt = 0) const override {
        std::cout << "FLT";
    }
};

class ConstORD: public ConstProcedure {
public:
    ConstFactor* execute(ConstFactor* arg, DeclarationSequence* resultDecl) const override {
        const auto value = arg->getValue();
        if (value.index() == 0) {
            return new ConstFactor(static_cast<long long>(std::get<0>(value)), resultDecl);
        } else if (value.index() == 2) {
            return new ConstFactor(static_cast<long long>(std::get<2>(value)), resultDecl);
        } else if (value.index() == 3) {
            std::string val = std::get<3>(value);
            if (val.size() == 1) {
                return new ConstFactor(static_cast<long long>(val[0]), resultDecl);
            } else {
                assert(false && "Incorect argument for constant ORD function");
            }
        } else if (value.index() == 4) {
            return new ConstFactor(static_cast<long long>(std::get<4>(value)), resultDecl);
        } else {
            assert(false && "Incorect argument for constant ORD function");  
        }
    }
    void debugOut(size_t tabcnt = 0) const override {
        std::cout << "ORD";
    }
};

class ConstCHR: public ConstProcedure {
public:
    ConstFactor* execute(ConstFactor* arg, DeclarationSequence* resultDecl) const override {
        const auto value = arg->getValue();
        if (value.index() == 0) {
            return new ConstFactor(static_cast<char>(std::get<0>(value)), resultDecl);
        } else {
            assert(false && "Incorect argument for constant CHR function");  
        }
    }
    void debugOut(size_t tabcnt = 0) const override {
        std::cout << "CHR";
    }
};

ConstProcedure* CreateConstABS() {return new ConstABS();}
ConstProcedure* CreateConstODD() {return new ConstODD();}
ConstProcedure* CreateConstLSL() {return new ConstLSL();}
ConstProcedure* CreateConstASR() {return new ConstASR();}
ConstProcedure* CreateConstROR() {return new ConstROR();}
ConstProcedure* CreateConstFLOOR() {return new ConstFLOOR();}
ConstProcedure* CreateConstFLT() {return new ConstFLT();}
ConstProcedure* CreateConstORD() {return new ConstORD();}
ConstProcedure* CreateConstCHR() {return new ConstCHR();}

ConstFactor::ConstFactor(long long val, DeclarationSequence* prevDecl):
    value(val),
    prevDeclaration(prevDecl),
    resultType(prevDeclaration->getArtefactByName("INTEGER")->getContext()->getType()) {
}
ConstFactor::ConstFactor(double val, DeclarationSequence* prevDecl):
    value(val),
    resultType(prevDeclaration->getArtefactByName("REAL")->getContext()->getType()),
    prevDeclaration(prevDecl) {
}
ConstFactor::ConstFactor(bool val, DeclarationSequence* prevDecl):
    value(val),
    resultType(prevDeclaration->getArtefactByName("BOOLEAN")->getContext()->getType()),
    prevDeclaration(prevDecl) {
}
ConstFactor::ConstFactor(std::string val, DeclarationSequence* prevDecl):
    value(val),
    resultType(prevDeclaration->getArtefactByName("STRING")->getContext()->getType()),
    prevDeclaration(prevDecl) {
}
ConstFactor::ConstFactor(unsigned val, DeclarationSequence* prevDecl):
        value(val),
        resultType(prevDeclaration->getArtefactByName("SET")->getContext()->getType()),
        prevDeclaration(prevDecl) {
}
ConstFactor::ConstFactor(char val, DeclarationSequence* prevDecl):
        value((long long)val),
        resultType(prevDeclaration->getArtefactByName("CHAR")->getContext()->getType()),
        prevDeclaration(prevDecl) {
}
ConstFactor::ConstFactor(NIL val, DeclarationSequence* prevDecl):
    value(val),
    resultType(prevDeclaration->getArtefactByName("NIL")->getContext()->getType()),
    prevDeclaration(prevDecl) {
}
ConstFactor::ConstFactor(ConstProcedure* val, DeclarationSequence* prevDecl): value(val), prevDeclaration(prevDecl) {}
ConstFactor::ConstFactor(ConstFactor* factor) {
    if (!factor) {
        assert(false && "Empty factor");  
    } else {
        value = factor->value;
        prevDeclaration = factor->prevDeclaration;
        resultType = factor->resultType;
    }
}

// Вывод отладочной информации
void ConstFactor::debugOut(size_t tabcnt) {
    for (size_t i = 0; i < tabcnt; ++i) {
        std::cout << "  ";
    }
    if (value.index() == 0) {
        std::cout << "int ";
        std::cout << std::get<0>(value); // << std::endl;
    } else if (value.index() == 1) {
        std::cout << "double ";
        std::cout << std::get<1>(value); // << std::endl;
    } else if (value.index() == 2) {
        std::cout << "bool ";
        std::cout << std::get<2>(value); // << std::endl;
    } else if (value.index() == 3) {
        std::cout << "string ";
        std::cout << std::get<3>(value); // << std::endl;
    } else if (value.index() == 4) {
        std::cout << "set ";
        unsigned st = std::get<4>(value);
        std::cout << "{ ";
        for (size_t i = 0; i < sizeof(unsigned) * 8; ++i) {
            if (st & (1ll << i)) {
                std::cout << i << " ";
            }
        }
        std::cout << "}\n";
    } else if (value.index() == 5) {
        std::cout << "NIL"; // << std::endl;
    } else if (value.index() == 6) {
        std::cout << "Const Procedure";
        // ConstProcedure* pr = std::get<6>(value);
        // if (!pr) {
        //     std::cout << "nullptr";
        // } else {
        //     pr->debugOut();
        // }
        // std::cout << "\n";
    }
}

// Получить тип результата
TypeContext* ConstFactor::getResultType() {
    return resultType;
}

// Получить тип результата
TypeContext* ConstFactor::getType() {
    return resultType;
}

// Применение унарного оператора
void ConstFactor::applyUnaryOperator(UnaryOperator op) {
    if (value.index() == 6) {
        assert(false && "Operators are disable for constant function");
    }
    switch (op) {
        case UnaryOperator::NEGATION:
            if (value.index() == 2) {
                value = !std::get<bool>(value);
            } else {
                assert(false && "Negation is defined only for bool"); 
            }
            break;
        case UnaryOperator::UN_MINUS:
            if (value.index() == 0) {
                value = -std::get<long long>(value);
            } else if (value.index() == 1) {
                value = -std::get<double>(value);
            } else {
                assert(false && "Unary minus is defined only for number"); 
            }
            break;
        case UnaryOperator::UN_PLUS:
            if (value.index() > 1) {
                assert(false && "Unary plus is defined only for number"); 
            }
            break;
        case UnaryOperator::NONE:
            break;
    }
}

// Применение relation
void ConstFactor::applyRelation(Relation rel, ConstFactor* factor) {
    if (rel == Relation::TYPE_TEST) {
        assert(false && "Type set relation is not defined for constant variables"); 
    }
    if (rel == Relation::SET_MEMBERSHIP) {
        if (factor->value.index() != 4 || value.index() != 0) {
            assert(false && "Set membership relation is defined only for integer and set"); 
        }
        long long val = std::get<0>(value);
        if (val < 0 || val > sizeof(unsigned) * 8) {
            assert(false && "incorect integer in set membership relation"); 
        }
        value = ((1ll << val) & std::get<unsigned>(factor->value)) != 0;
        return;
    }
    if (value.index() == 6) {
        assert(false && "Relations are disable for constant function");
    }
    if (factor->value.index() != value.index()) {
        assert(false && "Relation is defined only for same types"); 
    }
    switch (rel) {
        case Relation::EQUAL:
            value = (value == factor->value);
            break;
        case Relation::UNEQUAL:
            value = (value != factor->value);
            break;
        case Relation::LESS:
            if (value.index() == 2 || value.index() > 3) {
                assert(false && "Less relation is defined only for numbers and strings"); 
            } else {
                value = (value < factor->value);
            }
            break;
        case Relation::LESS_OR_EQUAL:
            if (value.index() == 2 || value.index() > 3) {
                assert(false && "Less or equal relation is defined only for numbers and strings"); 
            } else {
                value = (value <= factor->value);
            }
            break;
        case Relation::GREATER:
            if (value.index() == 2 || value.index() > 3) {
                assert(false && "Greater relation is defined only for numbers and strings"); 
            } else {
                value = (value > factor->value);
            }
            break;
        case Relation::GREATER_OR_EQUAL:
            if (value.index() == 2 || value.index() > 3) {
                assert(false && "Greater or equal relation is defined only for numbers and strings"); 
            } else {
                value = (value >= factor->value);
            }
            break;
        default:
            break;
    }
}

// Применение бинарного оператора
void ConstFactor::applyBinaryOperator(BinaryOperator op, ConstFactor* factor) {
    if (value.index() == 6) {
        assert(false && "Operators are disable for constant function");
    }
    if (factor->value.index() != value.index()) {
        assert(false && "Binary operator is defined only for same types"); 
    }
    switch (op) {
        case BinaryOperator::MINUS:
            if (value.index() == 0) {
                value = std::get<0>(value) - std::get<0>(factor->value);
            } else if (value.index() == 1) {
                value = std::get<1>(value) - std::get<1>(factor->value);
            } else if (value.index() == 4) {
                value = std::get<unsigned>(value) & (~std::get<unsigned>(factor->value));
            } else {
                assert(false && "Binary operator 'minus' is not defined for this type"); 
            }
            break;
        case BinaryOperator::PLUS:
            if (value.index() == 0) {
                value = std::get<0>(value) + std::get<0>(factor->value);
            } else if (value.index() == 1) {
                value = std::get<1>(value) + std::get<1>(factor->value);
            } else if (value.index() == 4) {
                value = std::get<unsigned>(value) | std::get<unsigned>(factor->value);
            } else {
                assert(false && "Binary operator 'plus' is not defined for this type"); 
            }
            break;
        case BinaryOperator::OR:
            if (value.index() == 2) {
                value = std::get<bool>(value) || std::get<bool>(factor->value);
            } else {
                assert(false && "Binary operator 'or' is defined only for bool"); 
            }
            break;
        case BinaryOperator::PRODUCT:
            if (value.index() == 0) {
                value = std::get<0>(value) * std::get<0>(factor->value);
            } else if (value.index() == 1) {
                value = std::get<1>(value) * std::get<1>(factor->value);
            } else if (value.index() == 4) {
                value = std::get<unsigned>(value) & std::get<unsigned>(factor->value);
            } else {
                assert(false && "Binary operator 'product' is not defined for this type"); 
            }
            break;
        case BinaryOperator::QUOTIENT:
            if (value.index() == 0) {
                value = (double)std::get<0>(value) / (double)std::get<0>(factor->value);
            } else if (value.index() == 1) {
                value = std::get<1>(value) / std::get<1>(factor->value);
            } else if (value.index() == 4) {
                value = std::get<unsigned>(value) ^ std::get<unsigned>(factor->value);
            } else {
                assert(false && "Binary operator 'quotient' is not defined for this type"); 
            }
            break;
        case BinaryOperator::INTEGER_QUOTIENT:
            if (value.index() == 0) {
                value = std::get<0>(value) / std::get<0>(factor->value);
            } else {
                assert(false && "Binary operator 'integer quotient' is defined only for integers"); 
            }
            break;
        case BinaryOperator::MODULUS:
            if (value.index() == 0) {
                if (std::get<0>(factor->value) <= 0) {
                    assert(false && "Second value in binary operator 'modulus' must be positive"); 
                }
                value = std::get<0>(value) % std::get<0>(factor->value);
            } else {
                assert(false && "Binary operator 'modulus' is defined only for integers"); 
            }
            break;
        case BinaryOperator::LOGICAL_CONJUNCTION:
            if (value.index() == 2) {
                value = std::get<bool>(value) && std::get<bool>(factor->value);
            } else {
                assert(false && "Binary operator 'logical conjunction' is defined only for bool"); 
            }
            break;
    }
}

// Возвращает целую константну, если она действительно целая
long long ConstFactor::getIntValue() const {
    if (value.index() != 0) {
        assert(false && "Value is not integer");
    } else {
        return std::get<0>(value);
    }
}

// Возвращает set константну, если она действительно set
unsigned ConstFactor::getSetValue() const {
    if (value.index() != 4) {
        assert(false && "Value is not set");
    } else {
        return std::get<4>(value);
    }
}

// Вызывает константную функцию
ConstFactor* ConstFactor::execute(std::vector<ConstFactor*> args, DeclarationSequence* ds) const {
    if (value.index() != 6) {
        assert(false && "Value type is not const procedure");
    }
    if (args.empty()) {
        assert(false && "There is no constant procedure with 0 arguments");
    } else if (args.size() == 1) {
        return std::get<6>(value)->execute(args[0], ds);
    } else if (args.size() == 2) {
        return std::get<6>(value)->execute(args[0], args[1], ds);
    } else {
        assert(false && bool("There is no constant procedure with more than 2 arguments"));
    }
}

void ConstFactor::generate(class Generator* generator, std::stringstream& cur, const std::string& name) {
    generator->GenerateConstFactor(*this, cur, name);
}

void ConstDeclaration::generate(Generator* gen, std::stringstream& hcode, std::stringstream& ccode) {
    gen->GenerateConstDeclaration(*this, hcode, ccode);
}

void ConstDeclaration::generate(Generator* gen, std::stringstream& cur) {
    gen->GenerateConstDeclaration(*this, cur);
}

// Ищет константную переменную по имени в импорте
DeclarationSequence* DeclarationSequence::getDeclarationSequenceFromImport(const std::string& ident) {
    if (!importArtefacts) {
        assert(false && "Import is empty");
    }
    if (importArtefacts->count(ident)) {
        return (*importArtefacts)[ident];
    } else {
        return nullptr;
    }

}

// Возвращает константную переменную по имени
ConstFactor* DeclarationSequence::getConstFactorByName(const std::string& ident) {
    if (constNamedArtefacts.count(ident)) {
        return new ConstFactor(constNamedArtefacts[ident]->getFactor());
    } else if (prevDeclaration) {
        return prevDeclaration->getConstFactorByName(ident);
    } else {
        return nullptr;
        // assert(false && "There was not declarated value with this name"); 
    }
}

// Возвращает NamedArtefact по имени
NamedArtefact* DeclarationSequence::getArtefactByName(const std::string& ident) {
    if (namedArtefacts.count(ident)) {
        return namedArtefacts[ident][0];
    } else if (prevDeclaration) {
        return prevDeclaration->getArtefactByName(ident);
    } else {
        return nullptr;
        // assert(false && "There was not declarated value with this name"); 
    }
}

 // Возвращает NamedArtefact по имени только из текущего скоупа
NamedArtefact* DeclarationSequence::getCurrentArtefactByName(const std::string& ident) {
    if (namedArtefacts.count(ident)) {
        return namedArtefacts[ident][0];
    } else {
        return nullptr;
        // assert(false && "There was not declarated value with this name"); 
    }
}

void DeclarationSequence::setImport(std::unordered_map<std::string, DeclarationSequence*>* import) {
    importArtefacts = import;
}

void DeclarationSequence::debugOut(size_t tabcnt) {
    COUT(tabcnt) << "Reserved artefats: " << std::endl;
    COUT(tabcnt) << "------------------ " << std::endl;
    COUT(tabcnt) << "Artefats: " << std::endl;
    COUT(tabcnt) << "--------- " << std::endl;
    for(NamedArtefact* artefact: getNamedArtefacts()) {
        if (!artefact) {
            COUT(tabcnt) << "NULL ARTEFACT\n";
            continue;
        }
        COUT(tabcnt) << artefact->getName() << (artefact->getAccess()? "*: ":": ");
        if (!(artefact->getContext())) {
            COUT(tabcnt) << "NULL ARTEFACT CONTEXT\n";
            continue;
        }
        artefact->getContext()->debugOut(tabcnt);
        COUT(tabcnt) << std::endl;
    }
    COUT(tabcnt) << "Const Artefats: " << std::endl;
    COUT(tabcnt) << "--------- " << std::endl;
    for(ConstDeclaration* artefact: getConstNamedArtefacts()) {
        COUT(tabcnt) << artefact->getName() << (artefact->getAccess()? "*: ":": ");
        artefact->getFactor()->debugOut(tabcnt);
        std::cout << "\n";
        // std::cout << std::endl;
    }
}