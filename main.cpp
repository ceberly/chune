#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <mutex>

#include <SDL.h>
#include <SDL_opengl.h>

using std::cerr;
using std::endl;

typedef struct GlobalMutableState {
  int device_count;
} GlobalMutableState;

typedef std::mutex GlobalMutableStateMutex;

int main() {
  cerr << "setting up SDL2..." << endl;
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER |
        SDL_INIT_AUDIO) != 0) {
    cerr << "error loading SDL: " << SDL_GetError() << endl;
    return EXIT_FAILURE;
  }

  cerr << "setting up audio..." << endl;

  const int count = SDL_GetNumAudioDevices(0);
  cerr << "found " << count << " audio devices." << endl;

  for (int i = 0; i < count; ++i) {
    cerr << "\t" << "Audio device " << i << " " <<
      SDL_GetAudioDeviceName(i, 0) << endl;
  }

  const char *glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL |
      SDL_WINDOW_RESIZABLE |
      SDL_WINDOW_ALLOW_HIGHDPI);

  SDL_Window *window = SDL_CreateWindow("CHUNE 0.01", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1);

  cerr << "setting up imgui..." << endl;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO(); (void)io;

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  bool done = false;
  while (!done) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) done = true;
      if (event.type == SDL_WINDOWEVENT &&
          event.window.event == SDL_WINDOWEVENT_CLOSE &&
          event.window.windowID == SDL_GetWindowID(window)) done = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    //if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
    
    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("Hello, world!");
      ImGui::Text("This is some useful text.");
      //ImGui::Checkbox("Demo Window", &show_demo_window);

      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
      ImGui::ColorEdit3("clear color", (float*)&clear_color);

      if (ImGui::Button("Button")) counter++;

      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
          1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      ImGui::End();
    }

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
        clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
  }


  cerr << "cleaning up..." << endl;
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
