default:
	/usr/bin/g++ `sdl2-config --cflags` -I imgui/backends -I imgui  -lGL -ldl main.cpp imgui/imgui*.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_sdl.cpp `sdl2-config --libs`
