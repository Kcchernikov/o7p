#include <boost/program_options.hpp>
#include <string>
#include <vector>
//#include <algorithm>
//#include <iterator>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "../compiler/parser.h"

using namespace boost::program_options;

// Начальные установки параметров компилятора и его запуск
void Compile(const char* str, const CompileOpts& opts);

void to_cout(const std::vector<std::string> &v)
{
  std::copy(v.begin(), v.end(),
    std::ostream_iterator<std::string>{std::cout, "\n"});
}

int main(int argc, const char *argv[])
{
    // Распознавание опций командной строки, обеспечивающих выбор
    // каталога проекта и компилируемой сущности (артефакта)
    std::string workspaceDir = "../o7p/workspace";      // катало workspace
    std::string fileName;                               // компилируемая сущность
    bool isDebug = false;

    CompileOpts opts;
    try {
        options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Help screen")
            ("workspace-dir,w", value(&workspaceDir), "Workspace dir, default \"../o7p/workspace\"")
            ("file-name,f", value(&fileName), "Module file path starting in workspace/o7, allowed without '.o7'")
            ("debug,d", "Is debug mode");

        // Ограничение позиционных аргументов (возможны без опции) одним
        positional_options_description pos_desc;
        //pos_desc.add("phone", -1);
        pos_desc.add("workspace-dir", 1);
        pos_desc.add("file-name", 2);
        pos_desc.add("debug", 4);

        command_line_parser parser{argc, argv};
        parser.options(desc).positional(pos_desc).allow_unregistered();
        parsed_options parsed_options = parser.run();

        variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        // CompileOpts
        opts.isDebug = isDebug;
        opts.workspaceDir = workspaceDir;
        opts.fileName = fileName;
        // Вывод подсказки
        if (vm.count("help")) {
            if(argc > 2) {
                std::cout << "Use only one option --help or -h for help\n";
                return 0;
            }
            std::cout << desc << '\n';
            return 0;
        }
    
        // Вывод непозиционных параметров (в нашем случае он пока один)
        if (vm.count("file-name")) {
            if (opts.fileName.size() > 3 && std::string(opts.fileName.end() - 3, opts.fileName.end()) == ".o7") {
                opts.fileName.resize(opts.fileName.size() - 3);
            }
        }
        if (vm.count("debug")) {
            opts.isDebug = true;
        }
    }
    catch (const error &ex) {
        std::cerr << ex.what() << '\n';
    }
    
    // После получения имен открытие и чтение файла с модулем
    // с последующей передачей их на обработку.
    // Пока файл с модулем открывается напрямую.
    //std::string exportName{packageDir + "/export"};
    //std::string artefactName{packageDir + "/smile/" + entityName};
    //std::string rigName{packageDir + "/rig/" + entityName};
    
    // Компилируемый модуль читается из файла в строку
    std::filesystem::path fullPath = opts.workspaceDir;
    std::ifstream moduleStream(fullPath / "o7" / (opts.fileName + ".o7"));
    if(moduleStream.fail()) {
        std::cout << "Module " << opts.fileName << " isn't accesible" << std::endl;
        std::cout << "Full path: " << fullPath / "o7" / (opts.fileName + ".o7");
        return 1;
    }
    std::stringstream ss;
    ss << moduleStream.rdbuf();
    std::string o7module(ss.str()); // Модуль Оберона 7
    /// Тестовый вывод прочитанного артефакта
    // std::cout << o7module << std::endl;

    Compile(o7module.c_str(), opts);
    
    return 0;
}
