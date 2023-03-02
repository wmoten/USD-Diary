# USD Scope Creator

This program creates a new USD file with a scope prim that references the original file.


## How to build

### C++
project compiles and executes using the following commands:

```
USD_INSTALL_ROOT=/wherever/you/installed/USD/to 
cmake .
make
./scope_reparent_run <usdFilePath> [scopeName]
```

`USD_INSTALL_ROOT` typically defaults to `/usr/local/USD`
on Linux but your location may vary.
See [USD's build documentation](https://github.com/PixarAnimationStudios/USD#3-run-the-script) for details.



## How to Run

To run the program, use the following command:

```./scope_reparent_run <usdFilePath> [scopeName]```

If no scope name is provided, the program will use the name of the original file (without the extension) as the scope name.

## Edge Cases Handled

- If the input file path is invalid, the program will print an error message and exit.
- If the specified scope name already exists in the original file, the program will print a warning message but will still create the new file with the same scope name.
- If the original file has no default prim set, the program will prompt the user to set one.
- If the original file has any prims with names in the `BLACKLIST_PRIM` vector, they will not be added as references to the new file.

## Edge Cases Not Handled

- The program does not handle cases where the input file is not a valid USD file.
- If the output file already exists, the program will overwrite it without warning.
- The program does not handle cases where the input file is in an unsupported format.
- The program does not handle cases where the user does not have permission to write to the output directory.
- The program does not handle cases where the user does not have permission to read the input file.
