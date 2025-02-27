#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "traffic_simulation.h"

// Global queues for lanes
Queue laneQueues[4];         // Queues for lanes A, B, C, D
int lanePriorities[4] = {0}; 

const SDL_Color VEHICLE_COLORS[] = {
    {0, 0, 255, 255}, 
    {255, 0, 0, 255}, 
    {0, 0, 128, 255}, 
    {255, 69, 0, 255} 
};

void initializeTrafficLights(TrafficLight *lights) {
    lights[0] = (TrafficLight){
        .state = RED,
        .timer = 0,
        .position = {INTERSECTION_X - LANE_WIDTH, INTERSECTION_Y - LANE_WIDTH - TRAFFIC_LIGHT_HEIGHT, TRAFFIC_LIGHT_WIDTH, TRAFFIC_LIGHT_HEIGHT},
        .direction = DIRECTION_NORTH};
    lights[1] = (TrafficLight){
        .state = RED,
        .timer = 0,
        .position = {INTERSECTION_X - LANE_WIDTH, INTERSECTION_Y + LANE_WIDTH, TRAFFIC_LIGHT_WIDTH, TRAFFIC_LIGHT_HEIGHT},
        .direction = DIRECTION_SOUTH};
    lights[2] = (TrafficLight){
        .state = GREEN,
        .timer = 0,
        .position = {INTERSECTION_X + LANE_WIDTH, INTERSECTION_Y - LANE_WIDTH, TRAFFIC_LIGHT_HEIGHT, TRAFFIC_LIGHT_WIDTH},
        .direction = DIRECTION_EAST};
    lights[3] = (TrafficLight){
        .state = GREEN,
        .timer = 0,
        .position = {INTERSECTION_X - LANE_WIDTH - TRAFFIC_LIGHT_HEIGHT, INTERSECTION_Y - LANE_WIDTH, TRAFFIC_LIGHT_HEIGHT, TRAFFIC_LIGHT_WIDTH},
        .direction = DIRECTION_WEST};
}

void updateTrafficLights(TrafficLight *lights) {
    Uint32 currentTicks = SDL_GetTicks();
    static Uint32 lastUpdateTicks = 0;

    if (currentTicks - lastUpdateTicks >= 5000) { // Change lights every 5 seconds
        lastUpdateTicks = currentTicks;

        // Check for high-priority lanes
        for (int i = 0; i < 4; i++) {
            if (laneQueues[i].size > 10) {
                lanePriorities[i] = 1; // Set high priority
            } else if (laneQueues[i].size < 5) {
                lanePriorities[i] = 0; // Reset to normal priority
            }
        }

        for (int i = 0; i < 4; i++) {
            if (lanePriorities[i] == 1) {
                lights[i].state = GREEN; // Give green light to high-priority lane
            } else {
                lights[i].state = (lights[i].state == RED) ? GREEN : RED; // Toggle lights
            }
        }
    }
}

Vehicle *createVehicle(Direction direction) {
    Vehicle *vehicle = (Vehicle *)malloc(sizeof(Vehicle));
    vehicle->direction = direction;

    // Set vehicle type with probabilities
    int typeRoll = rand() % 100;
    if (typeRoll < 5) {
        vehicle->type = AMBULANCE;
    } else if (typeRoll < 10) {
        vehicle->type = POLICE_CAR;
    } else if (typeRoll < 15) {
        vehicle->type = FIRE_TRUCK;
    } else {
        vehicle->type = REGULAR_CAR;
    }

    vehicle->active = true;
      // Set speed based on vehicle type
      switch (vehicle->type) {
        case AMBULANCE:
        case POLICE_CAR:
            vehicle->speed = 4.0f;
            break;
        case FIRE_TRUCK:
            vehicle->speed = 3.5f;
            break;
        default:
            vehicle->speed = 2.0f;
        }
    
        vehicle->state = STATE_MOVING;
        vehicle->turnAngle = 0.0f;
        vehicle->turnProgress = 0.0f;

       // 30% chance to turn
       int turnChance = rand() % 100;
       if (turnChance < 30) {
           vehicle->turnDirection = (turnChance < 15) ? TURN_LEFT : TURN_RIGHT;
       } else {
           vehicle->turnDirection = TURN_NONE;
       }
   
       // Set dimensions based on direction
       if (direction == DIRECTION_NORTH || direction == DIRECTION_SOUTH) {
           vehicle->rect.w = 20; // width
           vehicle->rect.h = 30; // height
       } else {
           vehicle->rect.w = 30; // width
           vehicle->rect.h = 20; // height
       }