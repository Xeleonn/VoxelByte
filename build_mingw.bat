@REM Build for MINGW64 or 32 from MSYS2.
@set OUT_DIR=Output
@set OUT_EXE=VoxelByte
@set INCLUDES=-I include/
@set SOURCES=include/imgui/imgui.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_impl_glfw.cpp include/imgui/imgui_impl_opengl3.cpp include/imgui/imgui_tables.cpp include/imgui/imgui_widgets.cpp include/glad/glad.c VoxelByte/main.cpp VoxelByte/clock.cpp VoxelByte/voxel.cpp VoxelByte/player.cpp VoxelByte/window.cpp VoxelByte/gui.cpp VoxelByte/logger.cpp VoxelByte/globals.cpp
@set LIBS=-lopengl32 -lgdi32 -lglu32 -ldwmapi -L binaries -lglfw3
mkdir %OUT_DIR%
g++ -DUNICODE %INCLUDES% %SOURCES% -g -o %OUT_DIR%/%OUT_EXE%.exe -mwindows -mconsole --static %LIBS%
pause