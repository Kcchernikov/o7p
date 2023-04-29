#include <boost/program_options.hpp>
#include <string>
#include <vector>
//#include <algorithm>
//#include <iterator>
#include <iostream>
#include <fstream>

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
    std::string packageDir;     // каталог пакета содержащего компилируемую сущность
    std::string fileName;       // компилируемая сущность
    std::string outDir;         // каталог для сохранения выходного файла
    std::string outFile;        // имя выходного файла
    bool isDebug = false;

    CompileOpts opts;
    try {
        options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Help screen")
            ("file,f", value(&fileName), "File with module")
            ("output-dir,od", value(&outDir), "Output dir")
            ("output-file,o", value(&outFile), "Output file's name")
            ("debug,d", "Is debug mode");

        // Ограничение позиционных аргументов (возможны без опции) одним
        positional_options_description pos_desc;
        //pos_desc.add("phone", -1);
        pos_desc.add("file", 1);
        pos_desc.add("output-dir", 2);
        pos_desc.add("output-file", 3);
        pos_desc.add("debug", 4);

        command_line_parser parser{argc, argv};
        parser.options(desc).positional(pos_desc).allow_unregistered();
        parsed_options parsed_options = parser.run();

        variables_map vm;
        store(parsed_options, vm);
        notify(vm);

        // CompileOpts
        opts.isDebug = isDebug;
        opts.outDir = outDir;
        opts.outFile = outFile;
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
        if (vm.count("file")) {
            if (!vm.count("output-file")) {
                std::size_t found = fileName.find_last_of("/\\");
                opts.outFile = fileName.substr(found+1);
                if (opts.outFile.size() > 3 && std::string(opts.outFile.end() - 3, opts.outFile.end()) == ".o7") {
                    opts.outFile.resize(opts.outFile.size() - 3);
                }
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
    std::string compiledFile{fileName};
    
    // Компилируемый модуль читается из файла в строку
    std::ifstream moduleStream(fileName);
    if(moduleStream.fail()) {
        std::cout << "Module" << fileName << " isn't accesible" << std::endl;
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
