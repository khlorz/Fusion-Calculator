/****************************************************
* Fusion Calculator's main loop
****************************************************/

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include <utility>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <cstring>

#include "aol/aol.h"

static void InitSDL(SDL_Window*& window, SDL_Renderer*& renderer);
static void InitImGui(SDL_Window* window, SDL_Renderer* renderer);

int main(int, char**)
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    try
    {
        InitSDL(window, renderer);
        InitImGui(window, renderer);

        ImGuiIO& io = ImGui::GetIO();
        bool show_demo_window = true;
        ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);

        bool done = false;
        while (!done)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL3_ProcessEvent(&event);
                if (event.type == SDL_EVENT_QUIT)
                    done = true;
                if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                    done = true;
            }

            if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
            {
                SDL_Delay(10);
                continue;
            }

            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            ImGui::ShowDemoWindow(&show_demo_window);

            ImGui::Render();
            SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
            SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            SDL_RenderClear(renderer);
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
            SDL_RenderPresent(renderer);
        }
    }
    catch (const AoL::RuntimeException& e)
    {
        std::cerr << e.What() << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "Something unknown happened\n";
    }

    if (ImGui::GetCurrentContext() != nullptr)
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
    if (window)
    {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();

    return 0;
}

void InitSDL(SDL_Window*& window, SDL_Renderer*& renderer)
{
    constexpr const char* window_name = "Fusion Calculator";
    constexpr int window_width = 1280;
    constexpr int window_height = 720;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::string ex_what = AoL::StrConcat("Error: SDL_Init(): ", SDL_GetError());
        throw AoL::RuntimeException{ std::move(ex_what) };
    }

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    window = SDL_CreateWindow(window_name, (int)(window_width * main_scale), (int)(window_height * main_scale), window_flags);
    if (window == nullptr)
    {
        std::string ex_what = AoL::StrConcat("Error: SDL_Init(): ", SDL_GetError());
        throw AoL::RuntimeException{ std::move(ex_what) };
    }

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr)
    {
        std::string ex_what = AoL::StrConcat("Error: SDL_Init(): ", SDL_GetError());
        throw AoL::RuntimeException{ std::move(ex_what) };
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);
}

void InitImGui(SDL_Window* window, SDL_Renderer* renderer)
{
    if (ImGui::CreateContext() == nullptr)
    {
        throw AoL::RuntimeException("ImGui not initialized!");
    }
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

