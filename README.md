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
