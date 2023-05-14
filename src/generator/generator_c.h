#ifndef GENERATOR_C_H
#define GENERATOR_C_H

#include "generator.h"
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class GeneratorC: public Generator {
public:
    GeneratorC() = default;
    ~GeneratorC() = default;
    void GenerateModule(const Module& module, std::string fileName, std::string baselibPath, std::stringstream& hcode, std::stringstream& ccode) override;
    void GenerateModuleDeclaration(const DeclarationSequence& declaration, std::stringstream& hcode, std::stringstream& ccode);
    void GenerateDeclaration(const DeclarationSequence& declaration, std::stringstream& cur, bool needInit) override;
    void GenerateConstDeclaration(const ConstDeclaration& declaration, std::stringstream& hcode, std::stringstream& ccode) override;
    void GenerateConstDeclaration(const ConstDeclaration& declaration, std::stringstream& cur) override;
    void GenerateTypeBool(const TypeBoolContext& type, std::stringstream& cur, const std::string& name) override;
    void GenerateTypeInteger(const TypeIntegerContext& type, std::stringstream& cur, const std::string& name) override;
    void GenerateTypeReal(const TypeRealContext& type, std::stringstream& cur, const std::string& name) override;
    void GenerateTypeChar(const TypeCharContext& type, std::stringstream& cur, const std::string& name) override;
    void GenerateTypeByte(const TypeByteContext& type, std::stringstream& cur, const std::string& name) override;
    void GenerateTypeString(const TypeStringContext& type, std::stringstream& cur, const std::string& name) override;
    void GenerateTypeNil(const TypeNilContext& type, std::stringstream& cu, const std::string& namer) override;
    void GenerateTypeSet(const TypeSetContext& type, std::stringstream& cur, const std::string& name) override;
    void GenerateTypeRecord(const TypeRecordContext& type, std::stringstream& cur, const std::string& name) override;
    void GenerateRecordElements(const TypeRecordContext& type, std::stringstream& cur) override;
    void GenerateTypePointer(const TypePointerContext& type, std::stringstream& cur, const std::string& name) override;
    void GenerateTypeArray(const TypeArrayContext& type, std::stringstream& cur, const std::string& name) override;
    void GenerateConstFactor(const ConstFactor& ctx, std::stringstream& cur, const std::string& name) override;
    void GenerateProcedureContext(const ProcContext& ctx, std::stringstream& cur, const std::string& name) override;
    void GenerateProcedureHeading(const Procedure& ctx, std::stringstream& cur, const std::string& name) override;
    void GenerateProcedureBody(const Procedure& ctx, std::stringstream& cur, const std::string& name) override;

    void GenerateStatement(const StatementSequence& statements, std::stringstream& cur) override;
    void GenerateAssigment(const Assigment& statement, std::stringstream& cur) override;
    void GenerateProcedureCall(const ProcedureCall& statement, std::stringstream& cur) override;
    void GenerateIf(const IfStatement& statement, std::stringstream& cur) override;
    void GenerateCase(const CaseStatement& statement, std::stringstream& cur) override;
    void GenerateWhile(const WhileStatement& statement, std::stringstream& cur) override;
    void GenerateRepeat(const RepeatStatement& statement, std::stringstream& cur) override;
    void GenerateFor(const ForStatement& statement, std::stringstream& cur) override;
    void GenerateDesignator(const Designator& designator, std::stringstream& cur) override;
    void GenerateRecordSelector(const RecordSelector& selector, std::stringstream& cur) override;
    void GenerateIndexSelector(const IndexSelector& selector, std::stringstream& cur) override;
    void GenerateAssertSelector(const AssertSelector& selector, std::stringstream& cur) override;
    void GeneratePointerSelector(const PointerSelector& selector, std::stringstream& cur) override;
    void GenerateExpression(const Expression& expression, std::stringstream& cur) override;
    void GenerateSimpleExpression(const SimpleExpression& expression, std::stringstream& cur) override;
    void GenerateTerm(const Term& term, std::stringstream& cur) override;
    void GenerateFactor(const Factor& factor, std::stringstream& cur) override;
    void GenerateSet(const Set& st, std::stringstream& cur) override;
    void GenerateDesignatorWrapper(const DesignatorWrapper& st, std::stringstream& cur) override;

private:
    void GenerateInitialisation(const DeclarationSequence& declaration, std::stringstream& cur);
    void GenerateArrayBaseRef(const TypeArrayContext& arr, std::stringstream& cur, const std::string& name/*, std::vector<size_t>& lengths*/, std::vector<Expression*>& lengths);
    void InitArray(const TypeArrayContext& arr, std::stringstream& cur, const std::string& name, const std::string& base_name, std::vector<size_t>& indexes);
    void InitRecord(const TypeRecordContext& record, std::stringstream& cur, const std::string& name);
    void GenerateNew(const Expression& exp, std::stringstream& cur);
    void GenerateTabs(std::stringstream& cur);
private:
    const size_t tabsz = 4;
    size_t tabcnt = 0;
    size_t markId = 0;
    size_t caseId = 0;
    bool isHDecl = false;
    std::unordered_map<std::string, bool> isRecordDeclarated;
    std::unordered_map<std::string, bool> isParticalDeclaratedPointer;
    std::unordered_set<std::string> tmpRecords;
    std::unordered_map<VarContext*, std::string> changedVarNames;
    std::unordered_set<std::string> declaratedPointerRecords; // Записи, которые уже были определены в указателях
    std::unordered_set<std::string> import;
};

#endif // GENERATOR_C_H
