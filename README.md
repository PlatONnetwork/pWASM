# pWASM

PlatON flavored WebAssembly

# platon-contract

Platon-contract includes C, C++ base libraries, and custom C++ library files.

## Compile

```
cmake .. -DWASM_ROOT={} -DCLANG_ROOT={} -DPLATON_TOOL_ROOT={}
Windows need to add -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND"
Compile requires a specified path
1. WASM_ROOT Specify the platon-s2wasm platon-wast2wasm path
2. CLANG_ROOT Specify clang llvm-link llc to specify
3. PLATON_TOOL_ROOT Specified platon-abigen designation
```

## User contract writing

First use the script/autoproject.sh script to generate a new project directory in the user directory. Simultaneously generate a simple cmake file, with the original project file.
Such as executing the following command

```
autoproject.sh [home_dir] [target]
```

You will see the new `target` directory

```
└── user
    ├── CMakeLists.txt
    └── [target]
        ├── CMakeLists.txt
        └── [target].cpp
```

You can write a new contract file in the new directory and re-execute the make command to compile.
