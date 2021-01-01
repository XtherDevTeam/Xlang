# Chinese Doc
-   # 这是个什么
    A:这是个编译性的语言，有配套的VM（还没写），目前进度：Lexer到AST的生成

    # 运行过程

    ```c++
    // Defined variables: code -> 从文件读取的源代码 , filename -> 输出文件名
    Lexer lexer(code);
    AST root(lexer);
    string asmcode = root.dumpasm();
    CompileObj c = Compiler::Compile(asmcode);
    c.dumpToFile(filename);
    ```

