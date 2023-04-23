#ifndef GENERATOR_H
#define GENERATOR_H

#include "../smodel/type.h"
#include "../smodel/module.h"
#include "../smodel/procedure_context.h"
#include "../object_model/procedure.h"
#include "../object_model/declaration.h"

#include <sstream>
#include <string>

class Generator {
public:
    Generator() = default;
    virtual ~Generator() = default;
    virtual void GenerateModule(const Module& module, std::stringstream& hcode, std::stringstream& ccode) = 0;
    virtual void GenerateDeclaration(const DeclarationSequence& declaration, std::stringstream& cur, bool needInit) = 0;
    virtual void GenerateConstDeclaration(const ConstDeclaration& declaration, std::stringstream& hcode, std::stringstream& ccode) = 0;
    virtual void GenerateConstDeclaration(const ConstDeclaration& declaration, std::stringstream& cur) = 0;
    virtual void GenerateTypeBool(const TypeBoolContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateTypeInteger(const TypeIntegerContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateTypeChar(const TypeCharContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateTypeReal(const TypeRealContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateTypeByte(const TypeByteContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateTypeString(const TypeStringContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateTypeNil(const TypeNilContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateTypeSet(const TypeSetContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateTypeRecord(const TypeRecordContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateRecordElements(const TypeRecordContext& type, std::stringstream& cur) = 0;
    virtual void GenerateTypePointer(const TypePointerContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateTypeArray(const TypeArrayContext& type, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateConstFactor(const ConstFactor& ctx, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateProcedureContext(const ProcContext& ctx, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateProcedureHeading(const Procedure& ctx, std::stringstream& cur, const std::string& name) = 0;
    virtual void GenerateProcedureBody(const Procedure& ctx, std::stringstream& cur, const std::string& name) = 0;

    virtual void GenerateStatement(const StatementSequence& statements, std::stringstream& cur) = 0;
    virtual void GenerateAssigment(const Assigment& statement, std::stringstream& cur) = 0;
    virtual void GenerateProcedureCall(const ProcedureCall& statement, std::stringstream& cur) = 0;
    virtual void GenerateIf(const IfStatement& statement, std::stringstream& cur) = 0;
    virtual void GenerateCase(const CaseStatement& statement, std::stringstream& cur) = 0;
    virtual void GenerateWhile(const WhileStatement& statement, std::stringstream& cur) = 0;
    virtual void GenerateRepeat(const RepeatStatement& statement, std::stringstream& cur) = 0;
    virtual void GenerateFor(const ForStatement& statement, std::stringstream& cur) = 0;
    virtual void GenerateDesignator(const Designator& designator, std::stringstream& cur) = 0;
    virtual void GenerateRecordSelector(const RecordSelector& selector, std::stringstream& cur) = 0;
    virtual void GenerateIndexSelector(const IndexSelector& selector, std::stringstream& cur) = 0;
    virtual void GenerateAssertSelector(const AssertSelector& selector, std::stringstream& cur) = 0;
    virtual void GeneratePointerSelector(const PointerSelector& selector, std::stringstream& cur) = 0;
    virtual void GenerateExpression(const Expression& expression, std::stringstream& cur) = 0;
    virtual void GenerateSimpleExpression(const SimpleExpression& expression, std::stringstream& cur) = 0;
    virtual void GenerateTerm(const Term& term, std::stringstream& cur) = 0;
    virtual void GenerateFactor(const Factor& factor, std::stringstream& cur) = 0;
    virtual void GenerateSet(const Set& st, std::stringstream& cur) = 0;
    virtual void GenerateDesignatorWrapper(const DesignatorWrapper& st, std::stringstream& cur) = 0;
};

#endif // GENERATOR_H
