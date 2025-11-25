#pragma once

struct SDL_Window;
struct SDL_Renderer;

void Runtime_Set_Sdl_Window(SDL_Window* window);
void Runtime_Set_Sdl_Renderer(SDL_Renderer* renderer);
SDL_Window* Runtime_Get_Sdl_Window();
SDL_Renderer* Runtime_Get_Sdl_Renderer();
