#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 2000
#define SCREEN_HEIGHT 1700
#define VEHICLE_WIDTH 50
#define VEHICLE_HEIGHT 100
#define MAX_VEHICLES 200
#define MAX_LANES 12
#define LIGHT_SIZE 50
#define LIGHT_OFFSET 10
#define VEHICLE_SPAWN_INTERVAL 1000 // Time between vehicle spawns in milliseconds