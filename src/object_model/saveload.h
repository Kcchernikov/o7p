#ifndef SAVELOAD_H
#define SAVELOAD_H

#include "declaration.h"
#include "procedure.h"

#include <filesystem>

VarContext* ImportVar(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def);

TypeRecordContext* ImportRecord(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def);

TypePointerContext* ImportPointer(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def);

TypeArrayContext* ImportArray(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def);

ProcContext* ImportProcedure(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def);

Context* ImportType(DeclarationSequence* declaration, DeclarationSequence* base, std::stringstream& def, std::string name = "");

DeclarationSequence* ImportModule(std::string def, DeclarationSequence* base);

DeclarationSequence* ImportModuleFromFile(std::filesystem::path path, DeclarationSequence* base);

void ExportPointer(DeclarationSequence* declaration, TypePointerContext* p, std::stringstream& def, bool firstExp = false);

void ExportRecord(DeclarationSequence* declaration, TypeRecordContext* rec, std::stringstream& def, bool firstExp = false);

void ExportVar(DeclarationSequence* declaration, std::string name, VarContext* var, std::stringstream& def);

void ExportArray(DeclarationSequence* declaration, TypeArrayContext* arr, std::stringstream& def, bool firstExp = false);

void ExportProcedure(DeclarationSequence* declaration, ProcContext* proc, std::stringstream& def, bool firstExp = false);

void ExportType(DeclarationSequence* declaration, Context* ctx, std::stringstream& def, bool firstExp = false);

std::string ExportModule(DeclarationSequence* declaration);

void ExportModuleToFile(DeclarationSequence* declaration, std::filesystem::path path);

#endif // SAVELOAD_H
