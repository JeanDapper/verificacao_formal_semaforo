#ifndef SINALEIRO2_H
#define SINALEIRO2_H

#include <stdint.h>
#include <stdbool.h>

/*=============================================================================
                                CONFIGURAÇÕES
=============================================================================*/

#define TIME_GREEN_A          10U
#define TIME_YELLOW_A          3U

#define TIME_GREEN_B          10U
#define TIME_YELLOW_B          3U

#define TIME_ALL_RED           2U
#define TIME_PEDESTRIAN        8U

#define FSM_CYCLE_TIME \
    (TIME_ALL_RED + \
     TIME_GREEN_A + \
     TIME_YELLOW_A + \
     TIME_ALL_RED + \
     TIME_GREEN_B + \
     TIME_YELLOW_B + \
     TIME_ALL_RED)

#define EVENTUALLY_TIMEOUT (FSM_CYCLE_TIME * 2U)
/*=============================================================================
                                ENUMS
=============================================================================*/

typedef enum
{
    LIGHT_RED = 0U,
    LIGHT_YELLOW,
    LIGHT_GREEN

} TrafficLightColor_t;

typedef enum
{
    PED_RED = 0U,
    PED_GREEN

} PedestrianLightColor_t;

typedef enum
{
    STATE_INIT = 0U,

    STATE_A_GREEN,
    STATE_A_YELLOW,

    STATE_ALL_RED_TO_B,

    STATE_B_GREEN,
    STATE_B_YELLOW,

    STATE_ALL_RED_TO_A,

    STATE_PEDESTRIAN,

    STATE_EMERGENCY

} SystemState_t;

/*=============================================================================
                        INTERFACE PARA TESTES
=============================================================================*/

// Estado do sistema
int getState(void);

// Estados das luzes
int getRoadA(void);
int getRoadB(void);
int getPedestrian(void);

// Reset do sistema
void resetSystem(void);

/*=============================================================================
                            EVENTOS
=============================================================================*/

void pedestrianButtonPressed(void);

void emergencyButtonPressed(void);

void emergencyButtonReleased(void);

/*=============================================================================
                            FSM
=============================================================================*/
void updateOutputs(void);

void trafficControllerTick(void);

void changeState(SystemState_t newState);
/*=============================================================================
                    FUNÇÕES AUXILIARES (TESTÁVEIS)
=============================================================================*/

const char* vehicleLightToString(TrafficLightColor_t color);

const char* pedestrianLightToString(PedestrianLightColor_t color);

#endif // SINALEIRO2_H