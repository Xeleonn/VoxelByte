@REM Build for MINGW64 or 32 from MSYS2.
@set OUT_DIR=Output
@set OUT_EXE=VoxelByte
@set INCLUDES=-I include/glad/ -I include/GLFW/
@set SOURCES=src/main.cpp include/glad/glad.c
@set LIBS=-lopengl32 -lgdi32 -lglu32 -ldwmapi -L binaries -lglfw3
mkdir %OUT_DIR%
g++ -DUNICODE %INCLUDES% %SOURCES% -g -o %OUT_DIR%/%OUT_EXE%.exe --static -mwindows %LIBS%
pause
