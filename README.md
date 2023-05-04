# PlantUML2Cpp

Welcome to PlantUML2Cpp, a parser and translator written in modern C++ (20) that takes your PlantUML files and turns them into boilerplate C++ code.

## How to use it

PlantUML2Cpp is a command line tool written in C++, using CMake as build tool.

### Building

Currently there are no real releases yet, so if you want to use this tool you have to build it yourself. You will need the following tools:

* A modern compiler: I'm developing using gcc12. I tried clang up to version 15 but it never worked well for me.
* Cmake >= 3.0.0 (I'm using 3.22.1, if you use anything below and it doesn't work please tell me)

#### Dependencies

Cmake will download all necessary dependencies for you using the CPM.cmake script.

These dependencies are:
* PEGParser
* fmt
* googletest for testing

#### Building on Debian derivates

On debian/ubuntu/all their derivates, you can do
```
sudo apt install gcc-12 g++-12 libstdc++-12-dev cmake
```
to install the toolchain used by this project.

After that, you should clone this repository, open a terminal in the new folder and do
```
mkdir build; cd build
cmake ..
make
```

#### Building on Windows

Sorry, I have never tried building this project on Windows. If you're using gcc with MinGW or similar, it should still work, but might need some extra work. I have no idea about Visual Studios compiler. Given my experience with gcc vs. clang I don't have the highest hopes that it will just work out of the box. If you tried, please let me know what you experienced.

### The command line tool

PlantUML2Cpp only takes one argument, the working directory. If that argument isn't given, the current directory is assumed to be the working directory.
In the working directory it looks for a folder named 'models'. All PlantUML files in this folder will be translated. Then it creates an 'include' and a 'source' directory and generates the code in those folders. As a safety measure it currently does _not_ overwrite existing files.

As the formating options of PlantUML2Cpp are limited, it is advisable to run a tool like clang-format on the generated files immediately.

#### Configuration

Currently, you can only configure the behavior of PlantUML using the Config.h file in the include folder. Later I will add the option to add a config file in the models-folder.

The config determines where PlantUML2Cpp will generate code files, some general formating options like indent, and how to map certain PlantUML structures to C++.

### Assumptions

For this tool to function properly, or just to make my life easier, I had to make some assumption and add some more constraints on top of the grammar of PlantUML. The following list is a (hopefully) complete list of those assumptions and what it means for your diagrams.

* Package and namespace names should not contain the namespace separator
    * In PlantUML you can call your package or even namespace "a.b.c" (with '.' as the namespace separator, which is the default) and it will be a package in the root namespace with the name "a.b.c".
    * Seriously?!
    * Anyway, if you still want this behavior just enclose those names in double-quotes
* Packages are small static libraries or C++20 modules (as soon as cmake has decent support for them)
    * If you want namespaces, use namespaces :)
* Only explicitly defined classes (, entities, etc.) will be generated
    * A diagram with `Class1 --|> Class2` alone will not generate any classes
    * If you put `class Class1` in front, `Class1` will be generated, inheriting from `Class2`, but `Class2` itself will not be generated
    * This is mostly to make explicit what classes you are actually interested in
    * Also, the order is important: first define your classes and then the relationships between them
* Don't do things that make absolutely no sense in C++
    * Like abstract variables!

Additions to the syntax:
* Const methods, parameters and variables:
    * If you add "const" before or after the typename the variable or parameter will be marked as const and generated accordingly
    * For methods add the const directly after the parameter list

## Coverage of the PlantUML language specification

Currently, only class diagrams are supported. The plan is to support more diagrams in the future to make it possible to generate an entire project at once.
To check what kind of syntax can be parsed, you can have a look in the test directory. Especially the ParserTest.cpp contains a lot of plantUML example code that can already be parsed.

### Class Diagram

| PlantUML syntax element | Can be parsed | Generates code |
|-------------------------|---------------|----------------|
| abstract (class) | yes | ? |
| annotation | yes | ? |
| class | yes | ? |
| entity | yes | ? |
| enum | yes | ? |
| interface | yes | ? |
| circle | yes | no |
| diamond | yes | no |
| to | be | extended |

## Roadmap

This is a loose list of features that range from 'essential' to 'might be nice'. If there is a feature you want, either on here or not, please let me know.

* declare small structs that are only needed by one client (variant or similar) in the file of the client
* Generate test frameworks and mocks
* Entity-Component-System: generate code for an ECS like entt
* Component diagrams: might be useful to generate makefiles or for C++20 modules
* Sequence diagrams: generate implementation of functions or comments inside them
* Activity diagrams: same idea as sequence diagrams
* State diagrams: Generate a finite state machine directly from your diagrams (maybe even with some formal verification?)
* BIG ONE: parse C++ files and append new parts and update existing parts only if they were changed

## How to contribute

Suggestions, pull requests or bug reports, any contribution is welcome!

To get a general overview in case you want to dive into the code, have a look into the models directory. It should always contain an up-to-date representation of the project. You can also look at the unit tests to get some idea of what is happening.

In general, PlantUML2Cpp takes a .puml-file in the models directory and creates an abstract syntax tree (AST) from it. If that succeeds and no errors were encountered, it passes the AST to various generators (currently implemented are class and variant generators with enum being worked on).
Those generators first translate the AST into a representation that makes sense for the use case using the visitor pattern. Then they can some post-processing on the gathered data and finally generate files. And then it continues with the next .puml-file.

## Motivation

There are several existing projects that turn (C++) code into PlantUML files, but as far as I could tell none that can do it the other way around.

Personally, I like my development to be model driven, not my models to be development driven. It is easier to gain and keep oversight of a software design using a visual model. But I also don't like fiddling around with visual editors where you can never really align anything and it always ends up looking like a mess but you can't tell if that's because you didn't arrange it properly or if the model is just too big. Therefore I really like the idea of PlantUML. Models are text, they are automatically turned into images and if they look too cluttered it is very likely time to split the model into two (or more) components.

## License

PlantUML2CPP is licensed under the MIT license. See LICENSE.md.