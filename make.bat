cls

del *.exe

g++ -mwindows -m64 -static -Wall -Wextra SelectFolderApp.cpp SelectFolder.cpp Registry.cpp -o SelectFolderApp.exe
