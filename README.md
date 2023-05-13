# o7p
Компилятор языка программирования Оберон-7, реализованный без сканера. На выходе формируется высокоуровневая семантическая модель, используемая в дальнейшем для генерации кода на языке C.

## Инструкция к запуску

Для сборки необходимо в директории `o7p` выполнить команду `make`. После этого в директории `../bin` появится исполняемый файл `o7p`.

У исполняемого файла есть следующие опции, которые можно задать:

- `-h [--help]` - вывести информацию о доступных опциях
- `-w [--workspace-dir] dir` - путь до директории `workspace`, в которой в директории `o7p` находятся модули на Oberon-7. По умолчанию `../o7p/workspace`
- `-f [--file-name] oberon_file.o7` - путь к файлу с модулем относительно `workspace/o7`. Расширение ".o7" может быть опущено
- `-d [--debug]` - вывести debug информацию. Включает в себя построенную объектную модель и сгенерированный код

## Пример запуска

`./o7p -f ut/success/assigment`

После данного запуска в `workspace` создадутся директории `h/ut/success`, `c/ut/success`, `main-c/ut/success`, `prj/ut/success/assigment`, `out/ut/success`.

В `h/ut/success` будет создан файл `assigment.h`, в `c/ut/success` будет создан `assigment.c`, в `main-c/ut/success` будет создан `main-assigment.c`, содержащий функцию `main()`, запускающую инициализацию модуля, в `prj/ut/success/assigment` будет создан `CMakeLists.txt`, описывающий проект из исполняемого файла `main-assigment.c` и библиотек `baselib` и `assigment`. 

Для того, чтобы собрать проект на С достаточно из директории `prj/ut/success/assigment` запустить команду `cmake ./ && make` и в директории `out/ut/success` появится исполняемый файл `assigment`
