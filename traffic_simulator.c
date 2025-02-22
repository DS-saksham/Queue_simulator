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

typedef struct Vehicle {
    int id;
    char road;
    int lane;
    SDL_Rect rect;
    int speed;
    int direction;  // 0: down, 1: right, 2: up, 3: left
} Vehicle;

typedef struct {
    char road;
    int lane;
    int priority;
} LanePriority;

typedef struct {
    LanePriority* data;
    int size;
    int capacity;
} PriorityQueue;

typedef struct {
    int x, y;
} TrafficLight;

Vehicle vehicles[MAX_VEHICLES];
const int ROAD_X_START = (SCREEN_WIDTH - 500) / 2;
const int ROAD_Y_START = (SCREEN_HEIGHT - 500) / 2;

SDL_Texture *carTexture;
TrafficLight trafficLights[8]; // 2 lights per corner, 4 corners
Uint32 lastBlink = 0;
bool isLightRed = true;
Uint32 lastSpawnTime = 0;

// Function Prototype of all the functions used in the project
void initializePriorityQueue(PriorityQueue* pq, int maxSize);
void heapifyUp(PriorityQueue* pq, int index);
void heapifyDown(PriorityQueue* pq, int index);
void enqueuePriority(PriorityQueue* pq, LanePriority item);
LanePriority dequeuePriority(PriorityQueue* pq);
bool isEmptyPriority(PriorityQueue* pq);
void updatePriority(PriorityQueue* pq, char road, int lane, int newPriority);
void generateVehicle();
void updateVehicles(PriorityQueue* pq);
void renderVehicles(SDL_Renderer *renderer);
void renderLane(SDL_Renderer *renderer);
void renderTrafficLights(SDL_Renderer *renderer);

// The priority queue was inalilized
void initializePriorityQueue(PriorityQueue* pq, int maxSize) {
    pq->data = (LanePriority*)malloc(sizeof(LanePriority) * maxSize);
    pq->size = 0;
    pq->capacity = maxSize;
}

void heapifyDown(PriorityQueue* pq, int index) {
    int smallest = index;
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;

    if (leftChild < pq->size && pq->data[leftChild].priority < pq->data[smallest].priority)
        smallest = leftChild;

    if (rightChild < pq->size && pq->data[rightChild].priority < pq->data[smallest].priority)
        smallest = rightChild;

    if (smallest != index) {
        LanePriority temp = pq->data[index];
        pq->data[index] = pq->data[smallest];
        pq->data[smallest] = temp;
        heapifyDown(pq, smallest);
    }
}

void enqueuePriority(PriorityQueue* pq, LanePriority item) {
    if (pq->size == pq->capacity) return;
    pq->data[pq->size] = item;
    heapifyUp(pq, pq->size);
    pq->size++;
}