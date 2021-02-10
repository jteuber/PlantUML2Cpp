## Assumptions

For this tool to function properly, or just to make my life easier, I had to make some assumption and add some more constraints on top of the grammar of PlantUML. The following list is a (hopefully) complete list of those assumptions and what it means for your diagrams.

* Package and namespace names should not contain the namespace separator
    * In PlantUML you can call your package or even namespace "a.b.c" (with '.' as the namespace separator, which is the default) and it will be a package in the root namespace with the name "a.b.c".
    * Seriously?!
    * Anyway, if you still want this behavior just enclose those names in double-quotes
* Packages are small static libraries/modules
    * If you want namespaces, use namespaces :)
