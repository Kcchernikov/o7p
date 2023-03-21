//#include "error.h"
#include "parser.h"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Список ключевых и зарезервированных слов,
// которые не могут использоваться в качестве идентификатора
namespace {
    std::vector<std::string> keyWords = {
        "ARRAY",
        "BEGIN",
        "BY",
        "CASE",
        "CONST",
        "DIV",
        "DO",
        "ELSE",
        "ELSIF",
        "END",
        "FALSE",
        "FOR",
        "IMPORT",
        "IF",
        "IN",
        "IS",
        "MOD",
        "MODULE",
        "NIL",
        "OF",
        "OR",
        "POINTER",
        "PROCEDURE",
        "RECORD",
        "REPEAT",
        "RETURN",
        "THEN",
        "TO",
        "TRUE",
        "TYPE",
        "UNTIL",
        "VAR",
        "WHILE"
    };
}

// Начальные установки параметров компилятора и его запуск
void Compile(const char* str) {
    ModuleCompiler mc(str);
    
    std::cout << "***** MODULE *****" << std::endl;
    std::cout << str << std::endl;

    std::cout << "***** COMPILER STARTED *****" << std::endl;
    Module module;
    if(mc.isModule(module) && mc.getErrCnt() == 0) {
        std::cout << "\e[1;32m" << "OK" << "\e[0m" << std::endl;
        module.debugOut();
    } else {
        std::cout << "\e[1;31m" << "FAIL" << "\e[0m" << std::endl;
    }
}

// Конструктор, формирующий начальные установки параметров компилятора
ModuleCompiler::ModuleCompiler(const char* str): moduleStr{str},
    pos{0}, line{1}, column{1}, errCnt{0}
{
    DeclarationSequence* base = new DeclarationSequence(nullptr, nullptr, nullptr);
    // Базовые типы
    base->addNamedArtefact(new NamedArtefact("BOOLEAN", new TypeBoolContext(), false));
    base->addNamedArtefact(new NamedArtefact("CHAR", new TypeCharContext(), false));
    base->addNamedArtefact(new NamedArtefact("INTEGER", new TypeIntegerContext(), false));
    base->addNamedArtefact(new NamedArtefact("STRING", new TypeStringContext(), false));
    base->addNamedArtefact(new NamedArtefact("REAL", new TypeRealContext(), false));
    base->addNamedArtefact(new NamedArtefact("BYTE", new TypeByteContext(), false));
    base->addNamedArtefact(new NamedArtefact("SET", new TypeSetContext(), false));
    base->addNamedArtefact(new NamedArtefact("NIL", new TypeNilContext(), false));
    // Константные функции
    base->addConstNamedArtefact(new ConstDeclaration("ABS", new ConstFactor(CreateConstABS(), nullptr), false));
    base->addConstNamedArtefact(new ConstDeclaration("ODD", new ConstFactor(CreateConstODD(), nullptr), false));
    base->addConstNamedArtefact(new ConstDeclaration("LSL", new ConstFactor(CreateConstLSL(), nullptr), false));
    base->addConstNamedArtefact(new ConstDeclaration("ASR", new ConstFactor(CreateConstASR(), nullptr), false));
    base->addConstNamedArtefact(new ConstDeclaration("ROR", new ConstFactor(CreateConstROR(), nullptr), false));
    base->addConstNamedArtefact(new ConstDeclaration("FLOOR", new ConstFactor(CreateConstFLOOR(), nullptr), false));
    base->addConstNamedArtefact(new ConstDeclaration("FLT", new ConstFactor(CreateConstFLT(), nullptr), false));
    base->addConstNamedArtefact(new ConstDeclaration("ORD", new ConstFactor(CreateConstORD(), nullptr), false));
    base->addConstNamedArtefact(new ConstDeclaration("CHR", new ConstFactor(CreateConstCHR(), nullptr), false));

    // TODO Добавить оствшиеся неконстантные функции, такие как INC, LEN, NEW
    declaration = base;
}

//-----------------------------------------------------------------------------
// Module = MODULE ident ";" [ImportList] DeclarationSequence
//          [BEGIN StatementSequence] END ident "." .
bool ModuleCompiler::isModule(Module& module) {
//_0:
    DeclarationSequence* oldDeclaration = declaration;
    ignore();
    if(isKeyWord("MODULE")) {
        ignore();
        goto _1;
    }
    return erMessage("The Module must start using key word MODULE");
_1:
    std::string moduleName;
    if(isIdent(moduleName)) {
        module.setModuleName(moduleName);
        goto _2;
    }
    return erMessage("It's not the name of Module");
_2:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("Semicolon expected");
_3:
    if(isImportList(module)) {
        goto _4;
    }
    // if(isDeclarationSequence()) {
    //     goto _5;
    // }
    // if(isKeyWord("BEGIN")) {
    //     goto _6;
    // }
    // if(isKeyWord("END")) {
    //     goto _8;
    // }
    // return erMessage("ImportList or DeclarationSequence or StatementSequence or END expected");
_4:
    DeclarationSequence* ds = nullptr;
    if(isDeclarationSequence(&ds, module.getReserved(), module.getImport())) {
        module.setDeclarationSequence(ds);
        delete ds;
        declaration = module.getDeclarationSequence();
        goto _5;
    } else {
        declaration = oldDeclaration;
    }
    // if(isKeyWord("BEGIN")) {
    //     goto _6;
    // }
    // if(isKeyWord("END")) {
    //     goto _8;
    // }
    // return erMessage("DeclarationSequence or StatementSequence or END expected");
_5:
    StatementSequence* st = nullptr;
    if(isKeyWord("BEGIN")) {
        goto _6;
    }
    if(isKeyWord("END")) {
        goto _8;
    }
    return erMessage("BEGIN or END expected");
_6:
    if(isStatementSequence(&st)) {
        module.setStatementSequence(st);
        goto _7;
    } else {
        module.setStatementSequence(nullptr);
    }
    if(isKeyWord("END")) {
        goto _8;
    }
    return erMessage("StatementSequence or END expected");
_7:
    if(isKeyWord("END")) {
        goto _8;
    }
    return erMessage("END expected");
_8:
    std::string endModuleName;
    if(isIdent(endModuleName) && endModuleName == moduleName) {
        goto _9;
    }
    return erMessage("It's not the name of Module");
_9:
    if(isSymbol(moduleStr[pos], '.')) {
        ++pos;
        ++column;
        ignore();
        goto _10;
    }
    return erMessage("Point expected");
_10:
    if(isEndOfString()) {
        goto _end;
    }
    return erMessage("End of Module expected");
_end:
    /// Тестовый вывод (закомментировать после отработки)
    ///testMessage("It's Module");
    declaration = oldDeclaration;
    return true;
}

//-----------------------------------------------------------------------------
// ImportList = IMPORT import {"," import} ";".
// import = ident [":=" ident].
bool ModuleCompiler::isImportList(Module& module) {
//_0:
    std::string alias;
    std::string import;
    if(isKeyWord("IMPORT")) {
        goto _1;
    }
    return false;
_1:
    if(isIdent(alias)) {
        goto _2;
    }
    return erMessage("It's not the imported name or alias");
_2:
    if(isSymbol(moduleStr[pos], ',')) {
        ImportContext* iContext = creator.CreateImportContext(alias, alias);
        module.AddNamedArtefact(alias, iContext);
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ImportContext* iContext = creator.CreateImportContext(alias, alias);
        module.AddNamedArtefact(alias, iContext);
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isAssignSymbol()) {
        goto _3;
    }
    return erMessage("It's not comma or  semicolon the imported name or alias or asignment");
_3:
    if(isIdent(import)) {
        goto _4;
    }
    return erMessage("It's not the imported name");
_4:
    if(isSymbol(moduleStr[pos], ',')) {
        ImportContext* iContext = creator.CreateImportContext(import, alias);
        module.AddNamedArtefact(alias, iContext);
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ImportContext* iContext = creator.CreateImportContext(import, alias);
        module.AddNamedArtefact(alias, iContext);
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    return erMessage("It's not ',' or ';'");
_end:
    return true;
}

//-----------------------------------------------------------------------------
// DeclarationSequence = [CONST {ConstDeclaration ";"}]
//    [TYPE {TypeDeclaration ";"}]
//    [VAR {VariableDeclaration ";"}]
//    {ProcedureDeclaration ";"}.
bool ModuleCompiler::isDeclarationSequence(DeclarationSequence** ds,
                                           std::unordered_map<std::string, NamedArtefact*>* reserved,
                                           std::unordered_map<std::string, DeclarationSequence*>* import) {
//_0:
    DeclarationSequence* curDS = nullptr;
    bool needDelete = false;
    if ((*ds)) {
        curDS = (*ds);
    } else {
        curDS = creator.CreateDeclarationSequence(declaration, reserved, import);
        needDelete = true;
    }
    declaration = curDS;
    ConstDeclaration* cd;
    NamedArtefact* decl = nullptr;
    if(isKeyWord("CONST")) {
        goto _1;
    }
    if(isKeyWord("TYPE")) {
        goto _4;
    }
    if(isKeyWord("VAR")) {
        goto _7;
    }
    if(isProcedureDeclaration(&decl)) {
        curDS->addNamedArtefact(decl);
        decl = nullptr;
        goto _10;
    }
    if (needDelete) {
        delete curDS;
    }
    return false;
_1:
    if(isConstDeclaration(&cd)) {
        curDS->addConstNamedArtefact(cd);
        goto _2;
    }
    return erMessage("It's not the Declaration of Constants");
_2:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("Semicolon expected");
_3:
    if(isConstDeclaration(&cd)) {
        curDS->addConstNamedArtefact(cd);
        goto _2;
    }
    if(isKeyWord("TYPE")) {
        goto _4;
    }
    if(isKeyWord("VAR")) {
        goto _7;
    }
    if(isProcedureDeclaration(&decl)) {
        curDS->addNamedArtefact(decl);
        decl = nullptr;
        goto _10;
    }
    goto _end;  // выход без ошибки при отсутствии правила
_4:
    if(isTypeDeclaration(curDS)) {
        // curDS->addNamedArtefact(decl);
        // decl = nullptr;
        goto _5;
    }
    return erMessage("It's not the Type Declaration");
_5:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _6;
    }
    return erMessage("Semicolon expected");
_6:
    if(isTypeDeclaration(curDS)) {
        // curDS->addNamedArtefact(decl);
        // decl = nullptr;
        goto _5;
    }
    if(isKeyWord("VAR")) {
        goto _7;
    }
    if(isProcedureDeclaration(&decl)) {
        curDS->addNamedArtefact(decl);
        decl = nullptr;
        goto _10;
    }
    goto _end;  // выход без ошибки при отсутствии правила
_7:
    if(isVariableDeclaration(curDS)) {
        goto _8;
    }
    return erMessage("It's not the Declaration of Variables");
_8:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _9;
    }
    return erMessage("Semicolon expected");
_9:
    if(isVariableDeclaration(curDS)) {
        goto _8;
    }
    if(isProcedureDeclaration(&decl)) {
        curDS->addNamedArtefact(decl);
        decl = nullptr;
        goto _10;
    }
    goto _end;  // выход без ошибки при отсутствии правила
_10:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _11;
    }
    return erMessage("Semicolon expected");
_11:
    if(isProcedureDeclaration(&decl)) {
        curDS->addNamedArtefact(decl);
        decl = nullptr;
        goto _10;
    }
    goto _end;  // выход без ошибки при отсутствии правила
_end:
    if (curDS->getNotInit() != 0) {
        return erMessage("Not all records are initialized");
    }
    if (ds) {
        (*ds) = curDS;
    }
    return true;
}

//-----------------------------------------------------------------------------
// ConstDeclaration = identdef "=" ConstExpression.
// identdef = ident ["*"].
// ConstExpression = expression.
bool ModuleCompiler::isConstDeclaration(ConstDeclaration** cd) {
//_0:
    Identdef ident;
    ConstFactor* factor;
    if(isIdentdef(ident)) {
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], '=')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    return erMessage("Symbol '=' expected");
_2:
    if(isConstExpression(&factor)) {
        goto _end;
    }
    return erMessage("Constant Expression expected");
_end:
    ConstDeclaration* curCD = creator.CreateConstDeclaration(ident.name, factor, ident.access);
    if (cd) {
        (*cd) = curCD;
    }
    return true;
}

//-----------------------------------------------------------------------------
// ConstExpression = SimpleConstExpression [relation SimpleConstExpression].
bool ModuleCompiler::isConstExpression(ConstFactor** factor) {
//_0:
    ConstFactor* factor1 = nullptr;
    ConstFactor* factor2 = nullptr;
    Relation rel;
    if(isSimpleConstExpression(&factor1)) {
        goto _1;
    }
    return false;
_1:
    if(isRelation(rel)) {
        goto _2;
    }
    goto _end;
_2:
    if(isSimpleConstExpression(&factor2)) {
        factor1->applyRelation(rel, factor2);
        delete factor2;
        goto _end;
    }
    return erMessage("Simple Constant Expression expected");
_end:
    if (factor) {
        (*factor) = factor1;
    }
    return true;
}

//-----------------------------------------------------------------------------
// SimpleConstExpression = ["+" | "-"] ConstTerm {AddOperator ConstTerm}.
bool ModuleCompiler::isSimpleConstExpression(ConstFactor** factor) {
//_0:
    ConstFactor* term = nullptr;
    ConstFactor* result = nullptr;
    UnaryOperator unOperator = UnaryOperator::NONE;
    BinaryOperator binOperator;
    if(isSymbol(moduleStr[pos], '+')) {
        unOperator = UnaryOperator::UN_PLUS;
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '-')) {
        unOperator = UnaryOperator::UN_MINUS;
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isConstTerm(&term)) {
        if (!result) {
            result = term;
            result->applyUnaryOperator(unOperator);
        } else {
            result->applyBinaryOperator(binOperator, term);
            delete term;
        }
        term = nullptr;
        goto _2;
    }
    return false;
_1:
    if(isConstTerm(&term)) {
        if (!result) {
            result = term;
            result->applyUnaryOperator(unOperator);
        } else {
            result->applyBinaryOperator(binOperator, term);
            delete term;
        }
        term = nullptr;
        goto _2;
    }
    return erMessage("Constant Term expected");
_2:
    if(isAddOperator(binOperator)) {
        goto _1;
    }
    goto _end;
_end:
    if (factor) {
        *factor = result;
    }
    return true;
}

//-----------------------------------------------------------------------------
// ConstTerm = ConstFactor {MulOperator ConstFactor}.
bool ModuleCompiler::isConstTerm(ConstFactor** factor) {
//_0:
    ConstFactor* factor1 = nullptr;
    ConstFactor* result = nullptr;
    BinaryOperator binOperator;
    if(isConstFactor(&factor1)) {
        result = factor1;
        factor1 = nullptr;
        goto _1;
    }
    return false;
_1:
    if(isMulOperator(binOperator)) {
        goto _2;
    }
    goto _end;
_2:
    if(isConstFactor(&factor1)) {
        result->applyBinaryOperator(binOperator, factor1);
        delete factor1;
        factor1 = nullptr;
        goto _1;
    }
    return erMessage("Constant Factor expected");
_end:
    if (factor) {
        *factor = result;
    }
    return true;
}

//-----------------------------------------------------------------------------
// ConstFactor = number | string | NIL | TRUE | FALSE |
//             ConstSet | designator [ActualParameters] | "(" ConstExpression ")" | "~" ConstFactor.
bool ModuleCompiler::isConstFactor(ConstFactor** factor) {
//_0:
    std::string str;
    std::variant<long long, double> number;
    ConstFactor* designator;
    std::vector<ConstFactor*> parameters;
    if(isKeyWord("NIL")) {
        if (factor) {
            NIL n;
            *factor = new ConstFactor(n, declaration);
        }
        goto _end;
    }
    if(isKeyWord("TRUE")) {
        if (factor) {
            *factor = new ConstFactor(true, declaration);
        }
        goto _end;
    }
    if(isKeyWord("FALSE")) {
        if (factor) {
            *factor = new ConstFactor(false, declaration);
        }
        goto _end;
    }
    if(isString(str)) {
        if (factor) {
            *factor = new ConstFactor(str, declaration);
        }
        goto _end;
    }
    if(isNumber(number)) {
        if (factor) {
            if (number.index() == 0) {
                *factor = new ConstFactor(std::get<long long>(number), declaration);
            } else {
                *factor = new ConstFactor(std::get<double>(number), declaration);
            }
        }
        goto _end;
    }
    if(isConstSet(factor)) {
        goto _end;
    }
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '~')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isConstDesignator(&designator)) {
        goto _4;
    }
    return false;
_1:
    if(isConstExpression(factor)) {
        goto _2;
    }
    return erMessage("Constant Expression expected");
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    return erMessage("Right bracket expected");
_3:
    if(isConstFactor(factor)) {
        (*factor)->applyUnaryOperator(UnaryOperator::NEGATION);
        goto _end;
    }
    return erMessage("Constant Factor expected");
_4:
    if(isConstActualParameters(parameters)) {
        if (factor) {
            *factor = designator->execute(parameters, declaration);
        }
        for (size_t i = 0; i < parameters.size(); ++i) {
            delete parameters[i];
        }
        delete designator;
        goto _end;
    }
    if (factor) {
        *factor = designator;
    }
    goto _end;
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ConstSet = "{" [ConstElement {"," ConstElement}] "}".
bool ModuleCompiler::isConstSet(ConstFactor** factor) {
//_0:
    ConstFactor* element = nullptr;
    unsigned result = 0;
   if(isSymbol(moduleStr[pos], '{')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
   }
   return false;
_1:
   if(isSymbol(moduleStr[pos], '}')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
   }
   if(isConstElement(&element)) {
        result |= element->getSetValue();
        delete element;
        element = nullptr;
        goto _2;
   }
   return erMessage("Constant Element or '}' expected");
_2:
   if(isSymbol(moduleStr[pos], '}')) {
       ++pos;
       ++column;
       ignore();
       goto _end;
   }
   if(isSymbol(moduleStr[pos], ',')) {
       ++pos;
       ++column;
       ignore();
       goto _3;
   }
   return erMessage("'}' or ',' expected");
_3:
   if(isConstElement(&element)) {
        result |= element->getSetValue();
        delete element;
        element = nullptr;
        goto _2;
   }
   return erMessage("Constant Element expected");
_end:
    *factor = new ConstFactor(result, declaration);
    return true;
}

//-----------------------------------------------------------------------------
// ConstElement = ConstExpression [".." ConstExpression].
bool ModuleCompiler::isConstElement(ConstFactor** factor) {
//_0:
    ConstFactor* exp1 = nullptr;
    ConstFactor* exp2 = nullptr;
    unsigned result = 0;
    int l = 0, r = -1;
    if(isConstExpression(&exp1)) {
        l = exp1->getIntValue();
        if (l < 0 || l > sizeof(unsigned) * 8) {
            return erMessage("ConstElement must be positive and lower than sizeof(unsigned) * 8");
        }
        result |= (1ll << l);
        delete exp1;
        goto _1;
    }
    return false;
_1:
    if(moduleStr[pos]=='.' && moduleStr[pos+1]=='.') {
        pos += 2;
        column +=2;
        lexValue = "..";
        ignore();
        goto _2;
    }
    goto _end;
_2:
    if(isConstExpression(&exp2)) {
        r = exp2->getIntValue();
        if (r < 0 || r > sizeof(unsigned) * 8) {
            return erMessage("ConstElement must be positive and lower than sizeof(unsigned) * 8");
        }
        if (r < l) {
            return erMessage("the left border should be smaller than the right one");
        }
        result = (1ll << r) - 1 + (1ll << r);
        result -= (1ll << l) - 1;
        delete exp2;
        // goto _1;
        goto _end;
    }
    return erMessage("Constant Expression expected");
_end:
    *factor = new ConstFactor(result, declaration);
    return true;
}

//-----------------------------------------------------------------------------
// TypeDeclaration = identdef "=" type.
bool ModuleCompiler::isTypeDeclaration(DeclarationSequence* ds) {
//_0:
    Identdef ident;
    TypeContext* ctx;
    if(isIdentdef(ident)) {
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], '=')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    return erMessage("Symbol '=' expected");
_2:
    TypeContext* oldCtx = ctx;
    NamedArtefact* cur = ds->getCurrentArtefactByName(ident.name);
    TypeRecordContext* rec;
    bool isPartDecl = false;
    if (cur) {
        rec = dynamic_cast<TypeRecordContext*>(cur->getContext());
        isPartDecl = (rec && (rec->getInit() == false));
    }
    if (isPartDecl) {
        ds->decNotInit();
        ctx = nullptr;
    } else {
        ctx = new TypePointerContext();
        NamedArtefact* type = new NamedArtefact(ident.name, ctx, ident.access);
        ctx->setNamedArtefact(type);
        oldCtx = ctx;
        ds->addNamedArtefact(type);
    }
    if(isType(&ctx)) {
        if (isPartDecl) {
            (*rec) = (*dynamic_cast<TypeRecordContext*>(ctx));
        }
        goto _end;
    }
    return erMessage("Type value expected");
_end:
    if (!isPartDecl && oldCtx != ctx) {
        NamedArtefact* art = ds->getArtefactByName(ident.name);
        art->setContext(ctx);
        ctx->setNamedArtefact(art);
        delete oldCtx;
    }
    return true;
}

//-----------------------------------------------------------------------------
// type = qualident | ArrayType | RecordType | PointerType | ProcedureType.
bool ModuleCompiler::isType(TypeContext** type, bool createIfNotExists) {
//_0:
    Context* ctx = nullptr;
    Qualident qualident;
    if(isArrayType(type)) {
        goto _end;
    }
    if(isRecordType(type)) {
        goto _end;
    }
    if(isPointerType(type)) {
        goto _end;
    }
    if(isProcedureType(type)) {
        goto _end;
    }
    if(isQualident(qualident, createIfNotExists)) {
        // (*type) = dynamic_cast<TypeContext*>(ctx);
        (*type) = qualident.type;
        goto _end;
    }
    return false;
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ArrayType = ARRAY length {"," length} OF type.
// length = ConstExpression.
bool ModuleCompiler::isArrayType(TypeContext** type) {
//_0:
    if(isKeyWord("ARRAY")) {
        goto _1;
    }
    return false;
_1:
    ConstFactor* len = nullptr;
    if(isConstExpression(&len)) {
        goto _2;
    }
    return erMessage("Constant Expression expected");
_2:
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isKeyWord("OF")) {
        goto _3;
    }
    return erMessage("Symbol ',' or Key word OF expected");
_3:
    TypeContext* ctx = nullptr;
    if(isType(&ctx)) {
        goto _end;
    }
    return erMessage("Type value expected");
_end:
    (*type) = new TypeArrayContext(len->getIntValue(), ctx);
    return true;
}

//-----------------------------------------------------------------------------
// RecordType = RECORD ["(" BaseType ")"] [FieldListSequence] END.
// BaseType = qualident.
// FieldListSequence = FieldList {";" FieldList}.
bool ModuleCompiler::isRecordType(TypeContext** type) {
//_0:
    TypeRecordContext* rec;
    TypeContext* baseType = nullptr;
    Qualident qualident;
    if(isKeyWord("RECORD")) {
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    goto _4;
    // if(isFieldList()) {
    //     goto _5;
    // }
    // if(isKeyWord("END")) {
    //     goto _end;
    // }
    // return erMessage("Symbol '(' or Key word END or Field list expected");
_2:
    if(isQualident(qualident)) {
        baseType = qualident.type;
        goto _3;
    }
    return erMessage("Qualident expected");
_3:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _4;
    }
    return erMessage("Symbol ')' expected");
_4:
    rec = new TypeRecordContext(dynamic_cast<TypeRecordContext*>(baseType));
    FieldList fl;
    if(isFieldList(fl)) {
        DeclarationSequence* ds = rec->getDeclaration();
        for (auto ident : fl.idents) {
            VarContext* ctx = new VarContext(fl.type);
            NamedArtefact* v = new NamedArtefact(ident.name, ctx, ident.access);
            ctx->setNamedArtefact(v);
            ds->addNamedArtefact(v);
            rec->incTypeSize(ctx->getType()->getTypeSize());
        }
        goto _5;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Key word END or Field list expected");
_5:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _6;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Key word END or Symbol ';' expected");
_6:
    fl.clear();
    if(isFieldList(fl)) {
        DeclarationSequence* ds = rec->getDeclaration();
        for (auto ident : fl.idents) {
            VarContext* ctx = new VarContext(fl.type);
            NamedArtefact* v = new NamedArtefact(ident.name, ctx, ident.access);
            ctx->setNamedArtefact(v);
            ds->addNamedArtefact(v);
        }
        goto _5;
    }
    return erMessage("Field list expected");
_end:
    (*type) = rec;
    return true;
}

//-----------------------------------------------------------------------------
// FieldList = IdentList ":" type.
// IdentList = identdef {"," identdef}.
bool ModuleCompiler::isFieldList(FieldList& fl) {
//_0:
    Identdef ident;
    if(isIdentdef(ident)) {
        fl.idents.push_back(ident);
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("Symbols ',' or ':' expected");
_2:
    if(isIdentdef(ident)) {
        fl.idents.push_back(ident);
        goto _1;
    }
    return erMessage("Idendef expected");
_3:
    TypeContext* ctx = nullptr;
    if(isType(&ctx)) {
        fl.type = ctx;
        goto _end;
    }
    return erMessage("Type expected");
_end:
    return true;
}


//-----------------------------------------------------------------------------
// PointerType = POINTER TO type.
bool ModuleCompiler::isPointerType(TypeContext** type) {
//_0:
    if(isKeyWord("POINTER")) {
        goto _1;
    }
    return false;
_1:
    if(isKeyWord("TO")) {
        goto _2;
    }
    return erMessage("POINTER TO expected");
_2:
    TypeContext* ctx = nullptr;
    if(isType(&ctx, true)) {
        goto _end;
    }
    return erMessage("Type expected");
_end:
    TypeRecordContext* rec = dynamic_cast<TypeRecordContext*>(ctx);
    if (!ctx || !rec) {
        return erMessage("Type to point is null");
    } else {
        if (*type) {
            dynamic_cast<TypePointerContext*>(*type)->setRecord(rec);
        } else {
            (*type) = new TypePointerContext(rec);
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// ProcedureType = PROCEDURE [FormalParameters].
bool ModuleCompiler::isProcedureType(TypeContext** type) {
//_0:
    FormalParameters* fp = nullptr;
    if(isKeyWord("PROCEDURE")) {
        goto _1;
    }
    return false;
_1:
    if(isFormalParameters(&fp)) {
        goto _end;
    }
    goto _end;
_end:
    ProcContext* proc = new ProcContext(declaration);
    if (fp) {
        proc->setFormalParameters(fp);
    }
    (*type) = proc;
    return true;
}

//-----------------------------------------------------------------------------
// FormalParameters = "(" [FPSection {";" FPSection}] ")" [":" qualident].
bool ModuleCompiler::isFormalParameters(FormalParameters** fp) {
//_0:
    FormalParameters* curFP = new FormalParameters();
    FPSection* fps = nullptr;
    Qualident result;
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    delete curFP;
    return false;
_1:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isFPSection(&fps)) {
        curFP->addFPSection(fps);
        goto _2;
    }
    return erMessage("FPSection expected");
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return erMessage("')' or ';' expected");
_3:
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _4;
    }
    // Процедура без результата
    curFP->setResultType(nullptr);
    goto _end;
    ///return erMessage("':' expected");
_4:
    if(isQualident(result)) {
        // TypeContext* tp = dynamic_cast<TypeContext*>(result);
        TypeContext* tp = result.type;
        if (!tp) {
            return erMessage("Procedure result must be a type");
        }
        curFP->setResultType(tp);
        goto _end;
    }
    return erMessage("Qualident expected");
_end:
    (*fp) = curFP;
    return true;
}

//-----------------------------------------------------------------------------
// FPSection = [VAR] ident {"," ident} ":" FormalType.
// FormalType = {ARRAY OF} qualident.
bool ModuleCompiler::isFPSection(FPSection** fps) {
//_0:
    FPSection* section = new FPSection();
    std::string ident;
    Qualident qualident;
    if(isKeyWord("VAR")) {
        section->setIsVar(true);
        goto _1;
    }
    if(isIdent(ident)) {
        section->addParameter(ident);
        goto _2;
    }
    delete section;
    return false;
_1:
    if(isIdent(ident)) {
        section->addParameter(ident);
        goto _2;
    }
    // Вывод сообщения об ошибке
    return erMessage("Name of formal parameter expected");
_2:
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("',' or ':' expected");
_3:
    // TODO: support array
    if(isKeyWord("ARRAY")) {
        goto _4;
    }
    if(isQualident(qualident)) {
        TypeContext* type = qualident.type;
        if (!type) {
            return erMessage("Type is not declarated");
        }
        section->setType(type);
        goto _end;
    }
    return erMessage("Key word ARRAY or Qualident expected");
_4:
    if(isKeyWord("OF")) {
        goto _5;
    }
    return erMessage("Key word OF expected");
_5:
    if(isQualident(qualident)) {
        TypeContext* type = qualident.type;
        if (!type) {
            return erMessage("Type is not declarated");
        }
        TypeContext* array = new TypeArrayContext(0, type);
        section->setType(array);
        goto _end;
    }
    return erMessage("Qualident expected");
_end:
    (*fps) = section;
    return true;
}

//-----------------------------------------------------------------------------
// VariableDeclaration = IdentList ":" type.
// IdentList = identdef {"," identdef}.
// Поскольку декларируется сразу несколько переменных, то они сразу
// записываются в переданный DeclarationSequence
bool ModuleCompiler::isVariableDeclaration(DeclarationSequence* ds) {
//_0:
    std::vector<Identdef> idents;
    Identdef ident;
    if(isIdentdef(ident)) {
        idents.push_back(ident);
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("Symbols ',' or ':' expected");
_2:
    if(isIdentdef(ident)) {
        idents.push_back(ident);
        goto _1;
    }
    return erMessage("Idendef expected");
_3:
    TypeContext* type = nullptr;
    if(isType(&type)) {
        goto _end;
    }
    return erMessage("Type expected");
_end:
    if (ds) {
        for (auto id : idents) {
            VarContext* ctx = new VarContext(type);
            NamedArtefact* v = new NamedArtefact(id.name, ctx, id.access);
            ctx->setNamedArtefact(v);
            ds->addNamedArtefact(v);
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// ProcedureDeclaration = ProcedureHeading ";" ProcedureBody ident.
bool ModuleCompiler::isProcedureDeclaration(NamedArtefact** procedure) {
//_0:
    DeclarationSequence* oldDeclaration = declaration;
    Procedure* proc = new Procedure(declaration);
    if(isProcedureHeading(proc)) {
        goto _1;
    }
    delete proc;
    return false;
_1:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    return erMessage("Symbol ';' expected");
_2:
    if(isProcedureBody(proc)) {
        goto _3;
    }
    return erMessage("Procedure body expected");
_3:
    std::string name;
    if(isIdent(name)) {
        if (name != proc->getIdent().name) {
            return erMessage("Begin and end procedure names are different");
        }
        goto _end;
    }
    return erMessage("Name of Procedure expected");
_end:
    if (procedure) {
        // ProcContext* proc = new Procedure(heading, body);
        (*procedure) = new NamedArtefact(name, proc, proc->getIdent().access);
        proc->setNamedArtefact(*procedure);
    }
    // declaration = oldDeclaration;
    return true;
}

//-----------------------------------------------------------------------------
// ProcedureHeading = PROCEDURE identdef [FormalParameters].
bool ModuleCompiler::isProcedureHeading(Procedure* proc) {
//_0:
    if(isKeyWord("PROCEDURE")) {
        goto _1;
    }
    return false;
_1:
    Identdef id;
    if(isIdentdef(id)) {
        proc->setIdent(id);
        goto _2;
    }
    return erMessage("Identdef expected");
_2:
    FormalParameters* fp = nullptr;
    if(isFormalParameters(&fp)) {
        proc->setFormalParameters(fp);
        goto _end;
    }
    goto _end;
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ProcedureBody = DeclarationSequence [BEGIN StatementSequence]
//                 [RETURN expression] END.
bool ModuleCompiler::isProcedureBody(Procedure* proc) {
//_0:
    DeclarationSequence* ds = proc->getDeclaration();
    DeclarationSequence* oldDeclaration = declaration;
    ProcedureBody body;
    body.result = nullptr;
    StatementSequence* st;
    Expression* result;
    if(isDeclarationSequence(&ds, nullptr, nullptr)) {
        goto _1;
    } else {
        declaration = proc->getDeclaration();
    }
    if(isKeyWord("BEGIN")) {
        goto _2;
    }
    if(isKeyWord("RETURN")) {
        goto _4;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return false;
_1:
    if(isKeyWord("BEGIN")) {
        goto _2;
    }
    if(isKeyWord("RETURN")) {
        goto _4;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("");
_2:
    if(isStatementSequence(&st)) {
        body.statement = st;
        goto _3;
    } else {
        body.statement = nullptr;
    }
    if(isKeyWord("RETURN")) {
        goto _4;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Statement Sequence or RETURN or END expected");
_3:
    if(isKeyWord("RETURN")) {
        goto _4;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("RETURN or END expected");
_4:
    if(isExpression(&result)) {
        body.result = result;
        goto _5;
    }
    return erMessage("Expression expected");
_5:
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("END expected");
_end:
    declaration = oldDeclaration;
    proc->setBody(body);
    return true;
}

//-----------------------------------------------------------------------------
// StatementSequence = statement {";" statement}.
bool ModuleCompiler::isStatementSequence(StatementSequence** stSeq) {
//_0:
    StatementSequence* cur = new StatementSequence();
    Statement* st = nullptr;
    if(isStatement(&st)) {
        cur->addStatement(st);
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    goto _end;
_2:
    if(isStatement(&st)) {
        cur->addStatement(st);
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ';')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    goto _end;
_end:
    (*stSeq) = cur;
    return true;
}

//-----------------------------------------------------------------------------
// statement = [assignment | ProcedureCall | IfStatement | CaseStatement |
//             WhileStatement | RepeatStatement | ForStatement].
bool ModuleCompiler::isStatement(Statement** st) {
    IfStatement* ifSt = nullptr;
    if(isIfStatement(&ifSt)) {
        (*st) = ifSt;
        return true;
    }
    CaseStatement* caseSt = nullptr;
    if(isCaseStatement(&caseSt)) {
        (*st) = caseSt;
        return true;
    }
    WhileStatement* whileSt = nullptr;
    if(isWhileStatement(&whileSt)) {
        (*st) = whileSt;
        return true;
    }
    RepeatStatement* repeatSt = nullptr;
    if(isRepeatStatement(&repeatSt)) {
        (*st) = repeatSt;
        return true;
    }
    ForStatement* forSt = nullptr;
    if(isForStatement(&forSt)) {
        (*st) = forSt;
        return true;
    }
    ///if(isAssignmentOrProcedureCall()) {
    ///    return true;
    ///}
    Assigment* assignmentSt = nullptr;
    if(isAssignment(&assignmentSt)) {
        (*st) = assignmentSt;
        return true;
    }
    ProcedureCall* callSt = nullptr;
    if(isProcedureCall(&callSt)) {
        (*st) = callSt;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// assignment = designator ":=" expression.
bool ModuleCompiler::isAssignment(Assigment** st) {
    Location l;
    storeLocation(l);
    /// Тестовая точка входа в правило
    ///std::cout << "----> isAssignment" << std::endl;
//_0:
    Designator* des = nullptr;
    Expression* exp = nullptr;
    if(isDesignator(&des)) {
        if (!des->getIsVar()) {
            delete des;
            return false;
            // return erMessage("Only vars are assignable");
        }
        goto _1;
    }
    restoreLocation(l);
    return false;
_1:
    if(moduleStr[pos]==':' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = ":=";
        ignore();
        goto _2;
    }
    // Откат на анализ вызова процедуры
    restoreLocation(l);
    return false;
    // return erMessage("':=' expected");
_2:
    if(isExpression(&exp)) {
        goto _end;
    }
    return erMessage("Expression expected");
_end:
    /// Тестовая точка выхода из правила
    ///std::cout << "isAssignmentOrProcedure ---->" << std::endl;
    Assigment* assignment = new Assigment(des, exp);
    (*st) = assignment;
    return true;
}

//-----------------------------------------------------------------------------
// ProcedureCall = designator [ActualParameters].
bool ModuleCompiler::isProcedureCall(ProcedureCall** st) {
//_0:
    Designator* des = nullptr;
    std::vector<Expression*> params;
    if(isDesignator(&des)) {
        goto _1;
    }
    return false;
_1:
    if(isActualParameters(params)) {
        goto _end;
    }
    goto _end;
_end:
    (*st) = new ProcedureCall(des, params);
    return true;
}

//-----------------------------------------------------------------------------
// IfStatement = IF expression THEN StatementSequence
//              {ELSIF expression THEN StatementSequence}
//              [ELSE StatementSequence] END.
bool ModuleCompiler::isIfStatement(IfStatement** st) {
    IfStatement* ifSt = new IfStatement();
    StatementSequence* stSeq = nullptr;
    Expression* exp = nullptr;
    bool isMainExp = true;
//_0:isAssignmentOrProcedure
    if(isKeyWord("IF")) {
        goto _1;
    }
    return false;
_1:
    if(isExpression(&exp)) {
        goto _2;
    }
    return erMessage("Expression expected");
_2:
    if(isKeyWord("THEN")) {
        goto _3;
    }
    return erMessage("Key word THEN expected");
_3:
    if(isStatementSequence(&stSeq)) {
        if (isMainExp) {
            ifSt->setMainExpression(exp, stSeq);
            exp = nullptr;
            stSeq = nullptr;
            isMainExp = false;
        } else {
            ifSt->addElsifExpression(exp, stSeq);
            exp = nullptr;
            stSeq = nullptr;
        }
        goto _4;
    }
    if(isKeyWord("ELSIF")) {
        goto _1;
    }
    if(isKeyWord("ELSE")) {
        goto _5;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Statement Sequence or ELSIF or ELSE or END expected");
_4:
    if(isKeyWord("ELSIF")) {
        goto _1;
    }
    if(isKeyWord("ELSE")) {
        goto _5;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("ELSIF or ELSE or END expected");
_5:
    if(isStatementSequence(&stSeq)) {
        ifSt->setElseExpression(stSeq);
        goto _6;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Statement Sequence or END expected");
_6:
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("END expected");
_end:
    (*st) = ifSt;
    return true;
}

//-----------------------------------------------------------------------------
// CaseStatement = CASE expression OF case {"|" case} END.
// case = [CaseLabelList ":" StatementSequence].
// CaseLabelList = LabelRange {"," LabelRange}.
// LabelRange = label [".." label].
// label = integer | string | qualident.
bool ModuleCompiler::isCaseStatement(CaseStatement** st) {
//_0:
    CaseStatement* caseSt = new CaseStatement();
    Expression* exp = nullptr;
    CaseLabelList* list = new CaseLabelList();
    StatementSequence* stSeq = nullptr;
    long long integer = 0;
    std::string s;
    Qualident qualident;
    Context* ctx1 = nullptr;
    Context* ctx2 = nullptr;
    if(isKeyWord("CASE")) {
        goto _1;
    }
    delete list;
    return false;
_1:
    if(isExpression(&exp)) {
        caseSt->setExpression(exp);
        goto _2;
    }
    return erMessage("Expression expected");
_2:
    if(isKeyWord("OF")) {
        goto _3;
    }
    return erMessage("Key word OF expected");
_3:
    if(isInteger(integer)) {
        ctx1 = new ConstFactor(integer, declaration);
        goto _4;
    }
    if(isString(s)) {
        ctx1 = new ConstFactor(s, declaration);
        goto _4;
    }
    if(isQualident(qualident)) {
        ctx1 = qualident.type;
        goto _4;
    }
    return erMessage("Integer or String or Qualified Ident expected");
_4:
    if(moduleStr[pos]=='.' && moduleStr[pos+1]=='.') {
        pos += 2;
        column += 2;
        lexValue = "..";
        ignore();
        goto _5;
    }
    list->list.push_back(std::pair<Context*, Context*>(ctx1, nullptr));
    ctx1 = nullptr;
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _7;
    }
    return erMessage("'..' or ',' or ':' expected");
_5:
    if(isInteger(integer)) {
        ctx2 = new ConstFactor(integer, declaration);
        goto _6;
    }
    if(isString(s)) {
        ctx2 = new ConstFactor(s, declaration);
        goto _6;
    }
    if(isQualident(qualident)) {
        ctx2 = qualident.type;
        goto _6;
    }
    return erMessage("Integer or String or Qualified Ident expected");
_6:
    list->list.push_back(std::pair<Context*, Context*>(ctx1, ctx2));
    ctx1 = nullptr;
    ctx2 = nullptr;
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isSymbol(moduleStr[pos], ':')) {
        ++pos;
        ++column;
        ignore();
        goto _7;
    }
    return erMessage("',' or ':' expected");
_7:
    if(isStatementSequence(&stSeq)) {
        caseSt->addCase(list, stSeq);
        goto _8;
    }
    if(isSymbol(moduleStr[pos], '|')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Statement Sequence or '|' or END expected");
_8:
    if(isSymbol(moduleStr[pos], '|')) {
        ++pos;
        ++column;
        ignore();
        list = new CaseLabelList();
        goto _3;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("'|' or END expected");
_end:
    (*st) = caseSt;
    return true;
}

//-----------------------------------------------------------------------------
// WhileStatement = WHILE expression DO StatementSequence
//                 {ELSIF expression DO StatementSequence} END.
bool ModuleCompiler::isWhileStatement(WhileStatement** st) {
    //_0:
    WhileStatement* whileSt = new WhileStatement();
    StatementSequence* stSeq = nullptr;
    Expression* exp = nullptr;
    bool isMainExp = true;
    if(isKeyWord("WHILE")) {
        goto _1;
    }
    return false;
_1:
    if(isExpression(&exp)) {
        goto _2;
    }
    return erMessage("Expression expected");
_2:
    if(isKeyWord("DO")) {
        goto _3;
    }
    return erMessage("Key word DO expected");
_3:
    if(isStatementSequence(&stSeq)) {
        if (isMainExp) {
            whileSt->setMainExpression(exp, stSeq);
            isMainExp = false;
        } else {
            whileSt->addElsifExpression(exp, stSeq);
        }
        exp = nullptr;
        stSeq = nullptr;
        goto _4;
    }
    if(isKeyWord("ELSIF")) {
        goto _1;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Statement Sequence or ELSIF or END expected");
_4:
    if(isKeyWord("ELSIF")) {
        goto _1;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("ELSIF or END expected");
_end:
    (*st) = whileSt;
    return true;
}

//-----------------------------------------------------------------------------
// RepeatStatement = REPEAT StatementSequence UNTIL expression.
bool ModuleCompiler::isRepeatStatement(RepeatStatement** st) {
//_0:
    Expression* exp = nullptr;
    StatementSequence* stSeq = nullptr;
    if(isKeyWord("REPEAT")) {
        goto _1;
    }
    return false;
_1:
    if(isStatementSequence(&stSeq)) {
        goto _2;
    }
    if(isKeyWord("UNTIL")) {
        goto _3;
    }
    return erMessage("Statement Sequence or UNTIL expected");
_2:
    if(isKeyWord("UNTIL")) {
        goto _3;
    }
    return erMessage("Key word UNTIL expected");
_3:
    if((isExpression(&exp))) {
        goto _end;
    }
    return erMessage("Expression expected");
_end:
    RepeatStatement* repeatSt = new RepeatStatement(exp, stSeq);
    (*st) = repeatSt;
    return true;
}

//-----------------------------------------------------------------------------
// ForStatement = FOR ident ":=" expression TO expression [BY ConstExpression]
//                DO StatementSequence END.
bool ModuleCompiler::isForStatement(ForStatement** st) {
//_0:
    std::string ident;
    StatementSequence* stSeq = nullptr;
    Expression* start = nullptr;
    Expression* stop = nullptr;
    ConstFactor* by = nullptr;
    if(isKeyWord("FOR")) {
        goto _1;
    }
    return false;
_1:
    if((isIdent(ident))) {
        goto _2;
    }
    return erMessage("Identifier expected");
_2:
    if(moduleStr[pos]==':' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = ":=";
        ignore();
        goto _3;
    }
    return erMessage("':=' expected");
_3:
    if(isExpression(&start)) {
        goto _4;
    }
    return erMessage("Expression expected");
_4:
    if(isKeyWord("TO")) {
        goto _5;
    }
    return erMessage("Key word TO expected");
_5:
    if(isExpression(&stop)) {
        goto _6;
    }
    return erMessage("Expression expected");
_6:
    if(isKeyWord("BY")) {
        goto _7;
    }
    if(isKeyWord("DO")) {
        goto _9;
    }
    return erMessage("Key word BY or DO expected");
_7:
    if((isConstExpression(&by))) {
        goto _8;
    }
    return erMessage("Constant Expression expected");
_8:
    if(isKeyWord("DO")) {
        goto _9;
    }
    return erMessage("Key word DO expected");
_9:
    if(isStatementSequence(&stSeq)) {
        goto _10;
    }
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("Statement Sequence or END expected");
_10:
    if(isKeyWord("END")) {
        goto _end;
    }
    return erMessage("END expected");
_end:
    NamedArtefact* arc = declaration->getArtefactByName(ident);
    if (!arc) {
        return erMessage("Variable " + ident + " is not declarated");
    }
    Context* ctx = arc->getContext();
    VarContext* var = dynamic_cast<VarContext*>(ctx);
    if (!var) {
        return erMessage("Variable " + ident + " has empty context");
    }
    if (!dynamic_cast<TypeIntegerContext*>(var->getType())) {
        return erMessage("Variable " + ident + " must be integer");
    }
    if (!by) {
        by = new ConstFactor((long long) 1, declaration);
    }
    ForStatement* forSt = new ForStatement(var, start, stop, by, stSeq);
    (*st) = forSt;
    return true;
}


//-----------------------------------------------------------------------------
// expression = SimpleExpression [relation SimpleExpression].
bool ModuleCompiler::isExpression(Expression** expression) {
//_0:
    Relation rel;
    Expression* exp = new Expression();
    SimpleExpression* firstSimpleExp = nullptr;
    SimpleExpression* secondSimpleExp = nullptr;
    std::string tmpValue = "";
    if(isSimpleExpression(&firstSimpleExp)) {
        exp->setFirstSimpleExpression(firstSimpleExp);
        goto _1;
    }
    return false;
_1:
    if(isRelation(rel)) {
        goto _2;
    }
    goto _end;
_2:
    if(isSimpleExpression(&secondSimpleExp)) {
        exp->setSecondSimpleExpression(rel, secondSimpleExp, declaration->getArtefactByName("BOOLEAN")->getContext()->getType());
        goto _end;
    }
    return erMessage("Simple Expression expected");
_end:
    (*expression) = exp;
    return true;
}

//-----------------------------------------------------------------------------
// SimpleExpression = ["+" | "-"] term {AddOperator term}.
bool ModuleCompiler::isSimpleExpression(SimpleExpression** expression) {
//_0:
    std::string tmpValue = "";
    UnaryOperator unaryOp = UnaryOperator::NONE;
    BinaryOperator op;
    Term* term = nullptr;
    SimpleExpression* exp = new SimpleExpression();
    bool isFirst = true;
    if(isSymbol(moduleStr[pos], '+')) {
        ++pos;
        ++column;
        ignore();
        unaryOp = UnaryOperator::UN_PLUS;
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '-')) {
        ++pos;
        ++column;
        ignore();
        unaryOp = UnaryOperator::UN_MINUS;
        goto _1;
    }
    if(isTerm(&term)) {
        exp->setStartTerm(unaryOp, term);
        term = nullptr;
        isFirst = false;
        goto _2;
    }
    return false;
_1:
    if(isTerm(&term)) {
        if (isFirst) {
            exp->setStartTerm(unaryOp, term);
            isFirst = false;
        } else {
            exp->addTerm(op, term);
        }
        term = nullptr;
        goto _2;
    }
    return erMessage("Term expected");
_2:
    if(isAddOperator(op)) {
        goto _1;
    }
    goto _end;
_end:
    (*expression) = exp;
    return true;
}

//-----------------------------------------------------------------------------
// term = factor {MulOperator factor}.
bool ModuleCompiler::isTerm(Term** term) {
//_0:
    BinaryOperator op;
    Factor* factor = nullptr;
    Term* cur = new Term();
    if(isFactor(&factor)) {
        cur->setStartFactor(factor);
        factor = nullptr;
        goto _1;
    }
    return false;
_1:
    if(isMulOperator(op)) {
        goto _2;
    }
    goto _end;
_2:
    if(isFactor(&factor)) {
        cur->addFactor(op, factor);
        factor = nullptr;
        goto _1;
    }
    return erMessage("Factor expected");
_end:
    (*term) = cur;
    return true;
}

//-----------------------------------------------------------------------------
// factor = number | string | NIL | TRUE | FALSE |
//       set | designator [ActualParameters] | "(" expression ")" | "~" factor.
bool ModuleCompiler::isFactor(Factor** factor) {
//_0:
    Set* st = nullptr;
    DesignatorWrapper* des;
    Expression* exp = nullptr;
    std::vector<Expression*> params;
    Factor* negFactor = nullptr;
    ConstFactor* constFactor = nullptr;
    std::string str;
    std::variant<long long, double> number;
    if(isKeyWord("NIL")) {
        NIL n;
        *factor = new Factor(new ConstFactor(n, declaration));
        goto _end;
    }
    if(isKeyWord("TRUE")) {
        *factor = new Factor(new ConstFactor(true, declaration));
        goto _end;
    }
    if(isKeyWord("FALSE")) {
        *factor = new Factor(new ConstFactor(false, declaration));
        goto _end;
    }
    if(isString(str)) {
        *factor = new Factor(new ConstFactor(str, declaration));
        goto _end;
    }
    if(isNumber(number)) {
        if (number.index() == 0) {
            *factor = new Factor(new ConstFactor(std::get<long long>(number), declaration));
        } else {
            *factor = new Factor(new ConstFactor(std::get<double>(number), declaration));
        }
        goto _end;
    }
    if(isSet(&st)) {
        (*factor) = new Factor(st);
        goto _end;
    }
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '~')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    des = new DesignatorWrapper();
    des->designator = nullptr;
    des->params = nullptr;
    if(isDesignator(&(des->designator))) {
        goto _4;
    }
    delete des;
    return false;
_1:
    if(isExpression(&exp)) {
        (*factor) = new Factor(exp);
        goto _2;
    }
    return erMessage("Expression expected");
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    return erMessage("Right bracket expected");
_3:
    if(isFactor(&negFactor)) {
        (*factor) = new Factor(negFactor);
        goto _end;
    }
    return erMessage("Factor expected");
_4:
    if(isActualParameters(params)) {
        des->params = new ActualParameters(params);
    }
    (*factor) = new Factor(des);
    goto _end;
_end:
    return true;
}

//-----------------------------------------------------------------------------
// ConstDesignator = ident ["." ident].
// Константные десигнаторы могут опираться лишь на константные переменные,
// то есть, отсутствует проверка типа, взаимодействия с указателями и
// обращения к массивам. По тем же соображениям, цепочка не может содержать
// более 2х идентификаторов
bool ModuleCompiler::isConstDesignator(ConstFactor** factor) {
    Location l;
//_0:
    std::string ident1;
    std::string ident2;
    if(isIdent(ident1)) {
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], '.')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    goto _end;
_2:
    if(isIdent(ident2)) {
        goto _end;
    }
    return erMessage("'.' expected");
_end:
    if (ident2.empty()) {
        if (factor) {
            *factor = declaration->getConstFactorByName(ident1);
            if (!(*factor)) {
                return erMessage(ident1 + " wasn't declarated");
            }
        }
    } else {
        DeclarationSequence* import = declaration->getDeclarationSequenceFromImport(ident1);
        if (!import) {
            return erMessage(ident1 + " wasn't imported");
        }
        if (factor) {
            *factor = import->getConstFactorByName(ident2);
            if (!(*factor)) {
                return erMessage(ident2 + " wasn't declarated in import " + ident1);
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
// designator = qualident {selector}.
// selector = "." ident | "[" ExpList "]" | "^" | "(" qualident ")".
bool ModuleCompiler::isDesignator(Designator** des) {
    Location l;
//_0:
    Designator* cur = new Designator();
    Context* ctx = nullptr;
    std::string ident;
    std::vector<Expression*> exps;
    Expression* exp = nullptr;
    Qualident qual;
    if(isQualident(qual)) {
        cur->addQualident(qual);
        // TODO проверять, что десигнатор обозначает переменную или процеруду, кроме случая exp IS T
        // if (!qual.isVariable) {
        //     return erMessage("Designator's qualident must be variable or procedure");
        // }
        goto _1;
    }
    delete cur;
    return false;
_1:
    if(isSymbol(moduleStr[pos], '.')) {
        ++pos;
        ++column;
        ignore();
        goto _2;
    }
    if(isSymbol(moduleStr[pos], '[')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    if(isSymbol(moduleStr[pos], '^')) {
        ++pos;
        ++column;
        ignore();
        cur->addPointerSelector();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], '(')) {
        // Фиксируем позицию скобки для возможного отката назад в случае,
        // когда обозначение не содержит круглые скобки,
        // а является вызовом процедуры.
        // В дальнейшем более точный анализ буде возлагаться на
        // семантический разбор
        storeLocation(l);
        ++pos;
        ++column;
        ignore();
        goto _5;
    }
    goto _end;
_2:
    if(isIdent(ident)) {
        cur->addRecordSelector(ident);
        goto _1;
    }
    return erMessage("'.' expected");
_3:
    if(isExpression(&exp)) {
        exps.push_back(exp);
        exp = nullptr;
        goto _4;
    }
    return erMessage("ExpList expected");
_4:
    if(isSymbol(moduleStr[pos], ']')) {
        ++pos;
        ++column;
        ignore();
        cur->addIndexSelector(exps);
        exps.clear();
        goto _1;
    }
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _3;
    }
    return erMessage("']' or ',' expected");
_5:
    if(isQualident(qual)) {
        if (qual.isVariable || qual.isConstant) {
            restoreLocation(l);
            goto _end;
        }
        goto _6;
    }
    // Возможна процедура без параметров. Поэтому откат на анализ
    // фактических параметров
    restoreLocation(l);
    goto _end;
    ///return erMessage("Qualident expected");
_6:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        cur->addAssertSelector(qual);
        goto _1;
    }
    // Здесь возможен возврат обозначения без круглых скобок,
    // котрые могут принадлежать списку фактических параметров.
    // Поэтому ситуации, когда не закрывающаяся круглая скобка,
    // откатываемся назад.
    // Хотя возможно, что под обозначение может попасть и процедура
    // с одним фактическим параметром.
    // Но в этой ситуации дальше будет разбираться семантический анализ.
    restoreLocation(l);
    goto _end;
    ///return erMessage("')' expected");
_end:
    (*des) = cur;
    return true;
}

//-----------------------------------------------------------------------------
// set = "{" [element {"," element}] "}".
// element = expression [".." expression].
bool ModuleCompiler::isSet(Set** st) {
//_0:
    Expression* exp1 = nullptr;
    Expression* exp2 = nullptr;
    Set* cur = new Set(declaration->getArtefactByName("SET")->getContext()->getType());
    if(isSymbol(moduleStr[pos], '{')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], '}')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isExpression(&exp1)) {
        goto _2;
    }
    return erMessage("'}' or Expression expected");
_2:
    if(moduleStr[pos]=='.' && moduleStr[pos+1]=='.') {
        pos += 2;
        column += 2;
        lexValue = "..";
        ignore();
        goto _3;
    }
    cur->addElement(exp1, nullptr);
    exp1 = nullptr;
    if(isSymbol(moduleStr[pos], '}')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return erMessage("'..'or '}' or ',' expected");
_3:
    if(isExpression(&exp2)) {
        cur->addElement(exp1, exp2);
        exp1 = nullptr;
        exp2 = nullptr;
        goto _4;
    }
    return erMessage("Expression expected");
_4:
    if(isSymbol(moduleStr[pos], '}')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return erMessage("'}' or ',' expected");
_end:
    (*st) = cur;
    return true;
}

/**
//-----------------------------------------------------------------------------
// ExpList = expression {"," expression}.
bool ModuleCompiler::isExpList() {
//_0:
    return false;
_end:
    return true;
}
*/

//-----------------------------------------------------------------------------
// ConstActualParameters = "(" [ConstExpList] ")" .
// ConstExpList = ConstExpression {"," ConstExpression}.
bool ModuleCompiler::isConstActualParameters(std::vector<ConstFactor*>& factors) {
    /// Тестовая точка входа в правило
    ///std::cout << "----> isConstActualParameters" << std::endl;
//_0:
    ConstFactor* factor;
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isConstExpression(&factor)) {
        factors.push_back(factor);
        factor = nullptr;
        goto _2;
    }
    return erMessage("')' or Expression expected");
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return erMessage("')' or ',' expected");
_end:
    /// Тестовая точка входа в правило
    ///std::cout << "isActualParameters ---->OK" << std::endl;
    return true;
}

//-----------------------------------------------------------------------------
// ActualParameters = "(" [ExpList] ")" .
// ExpList = expression {"," expression}.
bool ModuleCompiler::isActualParameters(std::vector<Expression*>& params) {
    /// Тестовая точка входа в правило
    ///std::cout << "----> isActualParameters" << std::endl;
//_0:
    params.clear();
    Expression* exp = nullptr;
    if(isSymbol(moduleStr[pos], '(')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return false;
_1:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isExpression(&exp)) {
        params.push_back(exp);
        exp = nullptr;
        goto _2;
    }
    return erMessage("')' or Expression expected");
_2:
    if(isSymbol(moduleStr[pos], ')')) {
        ++pos;
        ++column;
        ignore();
        goto _end;
    }
    if(isSymbol(moduleStr[pos], ',')) {
        ++pos;
        ++column;
        ignore();
        goto _1;
    }
    return erMessage("')' or ',' expected");
_end:
    /// Тестовая точка входа в правило
    ///std::cout << "isActualParameters ---->OK" << std::endl;
    return true;
}

//-----------------------------------------------------------------------------
// Правила, определяющие лексический анализ
//-----------------------------------------------------------------------------

// Идентификатор без игнорируемых символов - основа всех других буквенно-цифровых
// конструкций
// Id = Letter {Letter | Digit}.
bool ModuleCompiler::isId(std::string& id) {
    Location l;
    storeLocation(l);
    std::string tmpValue = "";
    if(isLetter(moduleStr[l.pos])) {
        tmpValue += moduleStr[l.pos];
        ++l.pos;
        ++l.column;
        while(isLetterOrDigit(moduleStr[l.pos])) {
            tmpValue += moduleStr[l.pos];
            ++l.pos;
            ++l.column;
        }
        if(!isKeyWordValue(tmpValue)) {
            restoreLocation(l);
            lexValue = tmpValue;
            /// Тестовый вывод (закомментировать после отработки)
            ///testMessage("It's Identdef = " + lexValue);
            std::swap(id, tmpValue);
            return true;
        }
    }
    return false;
}

// Ident = Letter {Letter | Digit}.
bool ModuleCompiler::isIdent(std::string& ident) {
    if(isId(ident)) {
        /// Тестовый вывод (закомментировать после отработки)
        testMessage("It's Identdef = " + lexValue);
        ignore();
        return true;
    }
    return false;
}

// identdef = ident ["*"].
bool ModuleCompiler::isIdentdef(Identdef& ident) {
    std::string tmpValue = "";
    if(isId(ident.name)) {
        // Возможна звездочка
        if(isSymbol(moduleStr[pos], '*')) {
            ++pos;
            ++column;
            lexValue += '*';
            ident.access = true;
        } else {
            ident.access = false;
        }
        /// Тестовый вывод (закомментировать после отработки)
        testMessage("It's Identdef = " + lexValue);
        ignore();
        return true;
    }
    return false;
}

// qualident = [ident "."] ident.
bool ModuleCompiler::isQualident(Qualident& qualident, bool createIfNotExists) {
    std::string ident1, ident2;
    qualident.type = nullptr;
    qualident.idents.clear();
    qualident.isVariable = false;
    qualident.isConstant = false;
    Location constExp;
    storeLocation(constExp);
    Location l;
    std::string tmpValue = "";
    // Проверка на первый идентификатор
    if(isId(ident1)) {
        qualident.idents.push_back(ident1);
        tmpValue = lexValue;
        goto _1;
    }
    return false;
_1:
    // При наличии первого идентификатора возможна вторая часть
    //или корректный выход
    if(isSymbol(moduleStr[pos], '.')) {
        storeLocation(l);   // Возможно, что точка от перечисления диапазона
        ++pos;
        ++column;
        goto _2;
    } else {
        NamedArtefact* art = declaration->getArtefactByName(ident1);
        if (!art) {
            if (createIfNotExists) {
                declaration->incNotInit();
                TypeRecordContext* rec = new TypeRecordContext(nullptr, false);
                NamedArtefact* newArt = new NamedArtefact(ident1, rec, false);
                rec->setNamedArtefact(newArt);
                declaration->addNamedArtefact(newArt);
                qualident.type = rec;
                qualident.isVariable = true;
            } else {
                restoreLocation(constExp);
                ConstFactor* factor = nullptr;
                if (isConstExpression(&factor)) {
                    qualident.type = factor->getResultType();
                    qualident.isVariable = false;
                    qualident.isConstant = true;
                } else {
                    return erMessage("Invalid qualident '" + tmpValue + "'");
                }
            }
        } else {
            Context* ctx = art->getContext();
            qualident.type = ctx->getType();
            if (ctx != qualident.type || dynamic_cast<ProcContext*>(ctx)) {
                qualident.isVariable = true;
            }
        }
    }
    goto _end;
_2:
    // Проверка на второй идентификатор
    if(isId(ident2)) {
        qualident.idents.push_back(ident2);
        tmpValue += '.' + lexValue;
        DeclarationSequence* ds = declaration->getDeclarationSequenceFromImport(ident1);
        if (ds) {
            NamedArtefact* art = ds->getArtefactByName(ident2);
            if (!art) {
                return erMessage("Invalid qualident '" + tmpValue + "'");
            }
            Context* ctx = art->getContext();
            qualident.type = ctx->getType();
            if (ctx != qualident.type || dynamic_cast<ProcContext*>(ctx)) {
                qualident.isVariable = true;
            }
            qualident.type->setImportFrom(ident1);
        } else {
            NamedArtefact* rec = declaration->getArtefactByName(ident1);
            if (!rec) {
                return erMessage("Invalid qualident '" + tmpValue + "'");
            }
            Context* ctx = rec->getContext();
            if (ctx != ctx->getType() || dynamic_cast<ProcContext*>(ctx)) {
                qualident.isVariable = true;
            } else {
                return erMessage("Invalid qualident '" + tmpValue + "'");
            }
            TypeRecordContext* record = dynamic_cast<TypeRecordContext*>(ctx->getType());
            if (!record) {
                return erMessage("Invalid qualident '" + tmpValue + "'");
            }
            ds = record->getDeclaration();
            if (!ds) {
                return erMessage("Invalid qualident '" + tmpValue + "'");
            }
            NamedArtefact* art = ds->getArtefactByName(ident2);
            if (!art) {
                return erMessage("Invalid qualident '" + tmpValue + "'");
            }
            qualident.type = art->getContext()->getType();
        }
        goto _end;
    }
    // Если не второй идентификатор, то возможен qualid из одного перед перечислением
    // Поэтому возвращается накопленное значение с восстановлением позиции
    restoreLocation(l);
    goto _end;
_end:
    lexValue = tmpValue;
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's Qualident = " + lexValue);
    ignore();
    return true;
}

// KeyWord.
bool ModuleCompiler::isKeyWord(const std::string& str) {
    Location l;
    storeLocation(l);
    std::string tmpValue = "";
    if(isLetter(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        while(isLetterOrDigit(moduleStr[pos])) {
            tmpValue += moduleStr[pos];
            ++pos;
            ++column;
        }
        if(tmpValue==str) {
            lexValue = tmpValue;
            /// Тестовый вывод (закомментировать после отработки)
            testMessage("It's Key word = " + lexValue);
            ignore();
            return true;
        }
    }
    // Откат, когда не то слово
    restoreLocation(l);
    return false;
}

// AssignSymbol = ":="
bool ModuleCompiler::isAssignSymbol() {
    if(moduleStr[pos]==':' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = ":=";
        ignore();
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// relation = "=" | "#" | "<" | "<=" | ">" | ">=" | IN | IS.
bool ModuleCompiler::isRelation(Relation& rel) {
    if(moduleStr[pos]=='=') {
        ++pos;
        ++column;
        lexValue = "=";
        ignore();
        rel = Relation::EQUAL;
        return true;
    } else if(moduleStr[pos]=='#') {
        ++pos;
        ++column;
        lexValue = "#";
        ignore();
        rel = Relation::UNEQUAL;
        return true;
    } else if(moduleStr[pos]=='<' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = "<=";
        ignore();
        rel = Relation::LESS_OR_EQUAL;
        return true;
    } else if(moduleStr[pos]=='<') {
        ++pos;
        ++column;
        lexValue = "<";
        ignore();
        rel = Relation::LESS;
        return true;
    } else if(moduleStr[pos]=='>' && moduleStr[pos+1]=='=') {
        pos += 2;
        column += 2;
        lexValue = ">=";
        ignore();
        rel = Relation::GREATER_OR_EQUAL;
        return true;
    } else if(moduleStr[pos]=='>') {
        ++pos;
        ++column;
        lexValue = ">";
        ignore();
        rel = Relation::GREATER;
        return true;
    } else if(isKeyWord("IN")) {
        rel = Relation::SET_MEMBERSHIP;
        return true;
    } else if(isKeyWord("IS")) {
        rel = Relation::TYPE_TEST;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// AddOperator = "+" | "-" | OR.
bool ModuleCompiler::isAddOperator(BinaryOperator& op) {
    if(moduleStr[pos]=='+') {
        ++pos;
        ++column;
        lexValue = "+";
        ignore();
        op = BinaryOperator::PLUS;
        return true;
    } else if(moduleStr[pos]=='-') {
        ++pos;
        ++column;
        lexValue = "-";
        ignore();
        op = BinaryOperator::MINUS;
        return true;
    } else if(isKeyWord("OR")) {
        ignore();
        op = BinaryOperator::OR;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// MulOperator = "*" | "/" | DIV | MOD | "&".
bool ModuleCompiler::isMulOperator(BinaryOperator& op) {
    if(moduleStr[pos]=='*') {
        ++pos;
        ++column;
        lexValue = "*";
        ignore();
        op = BinaryOperator::PRODUCT;
        return true;
    } else if(moduleStr[pos]=='/') {
        ++pos;
        ++column;
        lexValue = "/";
        ignore();
        op = BinaryOperator::QUOTIENT;
        return true;
    } else if(moduleStr[pos]=='&') {
        ++pos;
        ++column;
        lexValue = "&";
        ignore();
        op = BinaryOperator::LOGICAL_CONJUNCTION;
        return true;
    } else if(isKeyWord("DIV")) {
        ignore();
        op = BinaryOperator::INTEGER_QUOTIENT;
        return true;
    } else if(isKeyWord("MOD")) {
        ignore();
        op = BinaryOperator::MODULUS;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// number = integer | real.
bool ModuleCompiler::isNumber(std::variant<long long, double>& number) {
    double real;
    if(isReal(real)) {
        number = real;
        return true;
    }
    long long integer;
    if(isInteger(integer)) {
        number = integer;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// digit {digit} "." {digit} [ScaleFactor].
// ScaleFactor = "E" ["+" | "-"] digit {digit}.
bool ModuleCompiler::isReal(double& real) {
//_0:
    Location l;
    storeLocation(l);
    std::string tmpValue = "";
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    return false;
_1:
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    if (moduleStr[pos]=='.' && moduleStr[pos + 1]=='.') {
        restoreLocation(l);
        return false;
    }
    if(moduleStr[pos]=='.') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _2;
    }
    restoreLocation(l);
    return false;
_2:
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _2;
    }
    if(moduleStr[pos]=='E') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _3;
    };
    goto _end;
_3:
    if((moduleStr[pos]=='+') || (moduleStr[pos]=='-')) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _4;
    };
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _5;
    }
    restoreLocation(l);
    return false;
_4:
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _5;
    }
    restoreLocation(l);
    return false;
_5:
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _5;
    }
    goto _end;
_end:
    lexValue = tmpValue;
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's Real = " + lexValue);
    ignore();
    std::istringstream strToDouble(lexValue);
    if (!(strToDouble >> real)) {
        return erMessage("Bad real value");
    }
    return true;
}

//-----------------------------------------------------------------------------
// digit {digit} | digit {hexDigit} "H".
bool ModuleCompiler::isInteger(long long& integer) {
//_0:
    ///storeLocation(l);
    std::string tmpValue = "";
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    return false;
_1:
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    if(moduleStr[pos]=='H') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _end;
    }
    if((moduleStr[pos]>='A') && (moduleStr[pos]<='F')) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _2;
    }
_2:
    if(isHexDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    if(moduleStr[pos]=='H') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _end;
    }
_end:
    /// Возможно, где-то здесь нужно добавить дополнительную проверку, что после цифр
    /// сразу не идут буквы. То есть, что нет слияния числа с идентификатором.
    /// И в этом случае сделать откат.
    lexValue = tmpValue;
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's Integer = " + lexValue);
    ignore();
    if (lexValue.back() == 'H') {
        lexValue.pop_back();
        std::stringstream strToInteger;
        strToInteger << std::hex << lexValue;
        if (!(strToInteger >> integer)) {
            return erMessage("Bad integer value");
        }
    } else {
        std::istringstream strToInteger(lexValue);
        if (!(strToInteger >> integer)) {
            return erMessage("Bad integer value");
        }
    }
    return true;
}

// string = """ {character} """ | digit {hexDigit} "X".
bool ModuleCompiler::isString(std::string& str) {
    Location l;
    storeLocation(l);
//_0:
    std::string tmpValue = "";
    if(moduleStr[pos]=='"') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _1;
    }
    if(isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _2;
    }
    return false;
_1:
    while(moduleStr[pos]!='"') {
        if(isEndOfString()) {
            return false;
        }
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
    }
    tmpValue += moduleStr[pos];
    ++pos;
    ++column;
    goto _end;
_2:
    if(((moduleStr[pos]>='A') && (moduleStr[pos]<='F'))
            || isDigit(moduleStr[pos])) {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _2;
    }
    if(moduleStr[pos]=='X') {
        tmpValue += moduleStr[pos];
        ++pos;
        ++column;
        goto _end;
    }
    // Откат назад, если не набраны символьные данные, для проверки чисел
    restoreLocation(l);
    return false;
_end:
    lexValue = tmpValue;
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's String = " + lexValue);
    ignore();
    str.swap(tmpValue);
    return true;
}

bool ModuleCompiler::isString() {
    std::string str;
    return isString(str);
}
// comment = "(*" {character} "*)".
bool ModuleCompiler::isComment() {
//_0:
    std::string tmpValue = "";
    if((moduleStr[pos]=='(') && (moduleStr[pos+1]=='*')) {
        pos += 2;
        column += 2;
        tmpValue = "(*";
        goto _1;
    }
    return false;
_1:
    if(moduleStr[pos]=='*' && moduleStr[pos+1]==')') {
        // Конец текущего комментария
        pos += 2;
        column += 2;
        tmpValue += "*)";
        goto _end;
    }
    if(isSymbol(moduleStr[pos], '\n')) {
        // Символ перевода строки внутри комментария
        // Замена в лексеме на обратный слэш
        tmpValue += "\\";
        ++pos;
        ++line;
        column = 1;
        goto _1;
    }
    if(isComment()) {
        // рекурсивное обращение к вложенному комментарию
        // Накопленный подкомментарий добавляется к текущему
        tmpValue += lexValue;
        goto _1;
    }
    if(isEndOfString()) {
        // Данный комментарий не закрыт, а строка закончилась.
        // В принципе с точки зрения языка это не ошибка.
        // Но лучше фиксировать эту ситуацию
        return erMessage("Comment = " + tmpValue + " isn't closed");
    }
    // Продолжение комментария при любых других ситуациях
    tmpValue += moduleStr[pos];
    ++pos;
    ++column;
    goto _1;
_end:
    lexValue = tmpValue;
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's Comment = " + lexValue);
    //ignore();
    return true;
}

//-----------------------------------------------------------------------------
// Правила, определяющие транслитерацию
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Вспомогательные утилиты
//-----------------------------------------------------------------------------

// Пропуск пробельных символов и комментариев
// Ignore = {/ '\s' | '\n' | '\t' | Comment }
void ModuleCompiler::ignore() {
_0:
    if(isSymbol(moduleStr[pos], ' ')
            || isSymbol(moduleStr[pos], '\r')
            || isSymbol(moduleStr[pos], '\t'))
    {
        ++pos;
        ++column;
        goto _0;
    }
    if(isSymbol(moduleStr[pos], '\n')) {
        ++pos;
        ++line;
        column = 1;
        goto _0;
    }
    if(isComment()) {
        goto _0;
    }
    /// Тестовый вывод (закомментировать после отработки)
    testMessage("It's ignored. Next symbol = " + std::string{moduleStr[pos]});
}

// Проверка на принадлежность идентификатора одному из
// множества ключевых или зарезервированных слов
bool ModuleCompiler::isKeyWordValue(std::string& ident) {
    for(std::string iStr: keyWords) {
        if(ident == iStr) {
            return true;
        }
    }
    return false;
}

// Проверка на окончание разбираемой строки
bool ModuleCompiler::isEndOfString() {
    ///return pos >= int(moduleStr.length());
    return moduleStr[pos] == '\0';
}

// Выдача сообщения об ошибке
bool ModuleCompiler::erMessage(std::string&& str) {
    // Вывод сообщения об ошибке
    errCnt++;
    std::cout << "ERROR ("
              << line << ", "
              << column << " {"
              << pos << "}): ";
    std::cout << str << std::endl;
    return false;
}

// Формирование типового тестового сообщения
void ModuleCompiler::testMessage(std::string str) {
    // Вывод сообщения об ошибке
    std::cout << "TEST ("
              << line << ", "
              << column << " {"
              << pos << "}): ";
    std::cout << str << std::endl;
}
