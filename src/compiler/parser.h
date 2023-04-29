#ifndef PARSER_H
#define PARSER_H

#include "../smodel/module.h"
#include "../smodel/creator.h"
#include "../object_model/declaration.h"
#include "../object_model/designator.h"
#include "../object_model/procedure.h"

#include <iostream>
#include <string>
#include <vector>
#include <variant>

#include <cctype>

// Структура для временного хранения позиций во время разбора
struct Location {
    int pos;
    int line;
    int column;
};

struct CompileOpts {
    std::string outDir = "./";         // каталог для сохранения выходного файла
    std::string outFile = "code";        // имя выходного файла
    bool isDebug = false;
};

// Класс, задающий компилятор модуля
class ModuleCompiler {
    // Данные для хранения текущего местоположения в тексте
    int pos;        // Позиция в тексте
    int line;       // Номер строки
    int column;     // Номер столбца

    const char* moduleStr = nullptr; // ссылка на строку с текстом обрабатываемого модуля
    ///int pos{0};    // позиция текущего символа в обрабатываемом модуле
    std::string lexValue;   // значение текущей подстроки, набранной в ходе парсинга

    // Координаты текущей позиции в тексте, номера текущей строки, номера текущего столбца
    // Location location{0,1,1}; -- в конструктор

    // Начальные установки параметров компилятора и его запуск
    //void Compile(const char* str); - внешняя функция...

    // Данные для хранения старого местоположения в тексте
    int oldPos;                         // Позиция в тексте
    int oldLine;                        // Номер строки
    int oldColumn;                      // Номер столбца
    uint32_t errCnt;                    // Число ошибок
    Creator creator;                    // Вспомогательный класс для создания
    DeclarationSequence* declaration;   // Текущая таблица имен
public:
    // Конструктор, формирующий начальные установки параметров компилятора
    ModuleCompiler(const char* str);
    void InitParser(const char* str);

    // Module
    bool isModule(Module& module);

    // Число ошибок
    uint32_t getErrCnt() {
        return errCnt;
    }
    
    // ImportList
    bool isImportList(Module& module);
    // DeclarationSequence
    bool isDeclarationSequence(DeclarationSequence** ds,
                               std::unordered_map<std::string, NamedArtefact*>* reserved,
                               std::unordered_map<std::string, DeclarationSequence*>* import);
    // ConstDeclaration
    bool isConstDeclaration(ConstDeclaration** cd);
    // ConstExpression
    bool isConstExpression(ConstFactor** factor);
    // SimpleConstExpression
    bool isSimpleConstExpression(ConstFactor** factor);
    // ConstTerm
    bool isConstTerm(ConstFactor** factor);
    // ConstFactor
    bool isConstFactor(ConstFactor** factor);
    // ConstSet
    bool isConstSet(ConstFactor** factor);
    // ConstElement
    bool isConstElement(ConstFactor** factor);
    // TypeDeclaration
    bool isTypeDeclaration(DeclarationSequence* ds);
    // type
    bool isType(TypeContext** ctx, bool createIfNotExists = false);
    // ArrayType
    bool isArrayType(TypeContext** type);
    // RecordType
    bool isRecordType(TypeContext** type);
    // FieldList
    bool isFieldList(FieldList& fl);
    // PointerType
    bool isPointerType(TypeContext** type);
    // ProcedureType
    bool isProcedureType(TypeContext** type);
    // FormalParameters
    bool isFormalParameters(FormalParameters** fp);
    // FPSection
    bool isFPSection(FPSection** fps);
    // VariableDeclaration
    bool isVariableDeclaration(DeclarationSequence* ds);
    // ProcedureDeclaration
    bool isProcedureDeclaration(NamedArtefact** procedure);
    // ProcedureHeading
    bool isProcedureHeading(Procedure* proc);
    // ProcedureBody
    bool isProcedureBody(Procedure* proc);
    // StatementSequence
    bool isStatementSequence(StatementSequence** stSeq);
    // statement
    bool isStatement(Statement** st);
    // assignment = designator ":=" expression.
    bool isAssignment(Assigment** st);
    // ProcedureCall = designator [ActualParameters].
    bool isProcedureCall(ProcedureCall** st);
    // IfStatement
    bool isIfStatement(IfStatement** st);
    // CaseStatement
    bool isCaseStatement(CaseStatement** st);
    // WhileStatement
    bool isWhileStatement(WhileStatement** st);
    // RepeatStatement
    bool isRepeatStatement(RepeatStatement** st);
    // ForStatement
    bool isForStatement(ForStatement** st);
    // expression
    bool isExpression(Expression** exp);
    // SimpleExpression
    bool isSimpleExpression(SimpleExpression** exp);
    // term
    bool isTerm(Term** term);
    // factor
    bool isFactor(Factor** factor);
    // ConstDesignator
    bool isConstDesignator(ConstFactor** factor);
    // designator
    bool isDesignator(Designator** des);
    // set
    bool isSet(Set** st);
    // ConstActualParameters
    bool isConstActualParameters(std::vector<ConstFactor*>& factor);
    // ActualParameters
    bool isActualParameters(std::vector<Expression*>& params);

    //-----------------------------------------------------------------------------
    // Правила, определяющие лексический анализ
    //-----------------------------------------------------------------------------
    // Идентификатор без игнорируемых символов
    // Id = Letter {Letter | Digit}.
    bool isId(std::string& id);
    // Ident = Letter {Letter | Digit}.
    bool isIdent(std::string& ident);
    // Identdef = ident ["*"].
    bool isIdentdef(Identdef& ident);
    // qualident = [ident "."] ident.
    bool isQualident(Qualident& qualident, bool createIfNotExists = false);
    // KeyWord = Ident(str).
    bool isKeyWord(const std::string& str);
    // AssignSymbol = ":="
    bool isAssignSymbol();
    // relation = "=" | "#" | "<" | "<=" | ">" | ">=" | IN | IS.
    bool isRelation(Relation& rel);
    // AddOperator = "+" | "-" | OR.
    bool isAddOperator(BinaryOperator& op);
    // MulOperator = "*" | "/" | DIV | MOD | "&".
    bool isMulOperator(BinaryOperator& op);
    // number = integer | real.
    bool isNumber(std::variant<long long, double>& number);
    // Real
    bool isReal(double& real);
    // digit {digit} | digit {hexDigit} "H".
    bool isInteger(long long& integer);
    // string = """ {character} """ | digit {hexDigit} "X".
    bool isString(std::string& str);
    // string = """ {character} """ | digit {hexDigit} "X".
    bool isString();
    // comment = "(*" {character} "*)".
    bool isComment();

    //-----------------------------------------------------------------------------
    // Правила, определяющие транслитерацию
    //-----------------------------------------------------------------------------
    bool isEndOfString(); // проверка на окончание разбираемой строки
    inline bool isLetter(char ch) {return bool(isalpha(int(ch)));}
    inline bool isDigit(char ch) {return bool(isdigit(int(ch)));}
    inline bool isHexDigit(char ch) {
        return (bool(isdigit(int(ch)))||(ch>='A' && ch <='H'));
    }
    inline bool isLetterOrDigit(char ch) {return bool(isalnum(int(ch)));}
    inline bool isSymbol(char ch, int const sym) {return bool(int(ch)==sym);}
    inline bool isOneSymbol(char ch) {return moduleStr[pos]==ch;}

    //-----------------------------------------------------------------------------
    // Вспомогательные утилиты
    //-----------------------------------------------------------------------------
    // Пропуск пробельных символов и комментариев
    // Ignore = {/ '\s' | '\n' | Comment }
    void ignore();
    // Проверка на принадлежность идентификатора множеству
    // ключевых или зарезервированных слов
    bool isKeyWordValue(std::string& ident);
    // Выдача сообщения об ошибке
    bool erMessage(std::string&& str);
    // Формирование типового тестового сообщения
    void testMessage(std::string str);
    // Временное сохранение текущей позиции
    inline void storeLocation(Location& l) {
        l.pos = pos;
        l.line = line;
        l.column = column;
    }
    // Восстановление текущей позиции
    inline void restoreLocation(Location& l) {
        pos = l.pos;
        line = l.line;
        column = l.column;
    }
    
};
#endif // PARSER_H
