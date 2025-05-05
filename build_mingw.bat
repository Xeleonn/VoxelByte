@REM Build for MINGW64 or 32 from MSYS2.
@set OUT_DIR=Output
@set OUT_EXE=VoxelByte
@set INCLUDES=-I include/glad/ -I include/GLFW/ -I imgui/
@set SOURCES=imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp include/glad/glad.c src/main.cpp
@set LIBS=-lopengl32 -lgdi32 -lglu32 -ldwmapi -L binaries -lglfw3
mkdir %OUT_DIR%
g++ -DUNICODE %INCLUDES% %SOURCES% -g -o %OUT_DIR%/%OUT_EXE%.exe --static -mwindows %LIBS%
pause
