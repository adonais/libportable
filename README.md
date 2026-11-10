## How to build msedge++ source code?

- C++ compiler  
Microsoft Visual Studio/Windows   
 
- Clang compiler  

- Cmake, Ninja build system  

## Build!
- Ensure that cmake and clang in the PATH environment variable (cmd shell)  

	set CC=clang-cl  
	set CXX=clang-cl  
	cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCHROME_PLUS_ARCH=x64  

## Features

- Delete version. dll hijacking code, Edge is not applicable and prone to false positive viruses
- Delete PAK file memory modification as not applicable on edge
- Delete some functions that I personally feel are meaningless, see msedge++(example).ini
- Rewrite the tabs feature because Chrome++'s tab functionality does not work on the edge
- Rewrite the code, the built-in example configuration file will not overwrite the user's configuration file
- Improve the code by ignoring the data_ir and cache_ir options in directories without write permission
- Static modification of resources.pak during injection, masking upgrade errors, and default browser options
- Support automatic updates for Edge/Chrome portable versions
- For detailed information and functions, please refer to msedge++(example).ini
