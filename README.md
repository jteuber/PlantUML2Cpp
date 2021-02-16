## Assumptions

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
