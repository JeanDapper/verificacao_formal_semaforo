/*
===============================================================================
Sistema Embarcado de Controle Inteligente de Cruzamento Urbano
-------------------------------------------------------------------------------
Descrição:
    Controle de cruzamento entre Rua A e Rua B contendo:
        - Controle de veículos
        - Controle de pedestres
        - Modo emergência/manutenção
        - Máquina de estados temporizada
        - Fairness entre pedestres e veículos

Requisitos atendidos:
    RF-01 até RF-23

RF-17 atualizado:
    "O sistema deve concluir a transição segura do estado atual antes de
    entrar em modo emergência/manutenção."

Características importantes:
    - Nunca existem dois verdes simultâneos
    - Toda transição GREEN -> RED passa por YELLOW
    - Emergência só entra em estado seguro
    - Pedestre só pode ser atendido após:
        1 ciclo completo de A
        1 ciclo completo de B
===============================================================================
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "sinaleiro2.h"



/*=============================================================================
                                ESTRUTURAS
=============================================================================*/

typedef struct
{
    TrafficLightColor_t roadA;
    TrafficLightColor_t roadB;

    PedestrianLightColor_t pedestrian;

} TrafficLights_t;

/*=============================================================================
                            VARIÁVEIS GLOBAIS
=============================================================================*/

static uint32_t gTick = 0U;

static TrafficLights_t gLights;

static SystemState_t gState = STATE_INIT;

static uint32_t gStateTimer = 0U;

/*=============================================================================
                        PEDRESTRE
=============================================================================*/

static bool gPedestrianRequest = false;

/*
    RF-23:
    Após um ciclo de pedestre,
    deve ocorrer:
        - 1 ciclo completo de A
        - 1 ciclo completo de B

    antes de novo atendimento.
*/

static bool gPedestrianLock = false;

static bool gCycleACompleted = false;
static bool gCycleBCompleted = false;

/*=============================================================================
                        EMERGÊNCIA
=============================================================================*/

static bool gEmergencyPending = false;
static bool gEmergencyActive  = false;

/*=============================================================================
                        INTERFACE PARA TESTES
=============================================================================*/

int getState(void) { return gState; }
int getRoadA(void) { return gLights.roadA; }
int getRoadB(void) { return gLights.roadB; }
int getPedestrian(void) { return gLights.pedestrian; }

void resetSystem(void)
{
    gTick = 0;
    gState = STATE_INIT;
    gStateTimer = 0;

    gPedestrianRequest = false;
    gPedestrianLock = false;
    gCycleACompleted = false;
    gCycleBCompleted = false;

    gEmergencyPending = false;
    gEmergencyActive = false;

    updateOutputs();
}

/*=============================================================================
                            AUXILIARES
=============================================================================*/

const char* vehicleLightToString(
    TrafficLightColor_t color)
{
    switch(color)
    {
        case LIGHT_RED:
            return "RED";

        case LIGHT_YELLOW:
            return "YELLOW";

        case LIGHT_GREEN:
            return "GREEN";

        default:
            return "UNKNOWN";
    }
}

const char* pedestrianLightToString(
    PedestrianLightColor_t color)
{
    switch(color)
    {
        case PED_RED:
            return "RED";

        case PED_GREEN:
            return "GREEN";

        default:
            return "UNKNOWN";
    }
}

/*=============================================================================
                            OUTPUTS
=============================================================================*/

void updateOutputs(void)
{
    switch(gState)
    {
        case STATE_INIT:

            gLights.roadA      = LIGHT_RED;
            gLights.roadB      = LIGHT_RED;
            gLights.pedestrian = PED_RED;

            break;

        case STATE_A_GREEN:

            gLights.roadA      = LIGHT_GREEN;
            gLights.roadB      = LIGHT_RED;
            gLights.pedestrian = PED_RED;

            break;

        case STATE_A_YELLOW:

            gLights.roadA      = LIGHT_YELLOW;
            gLights.roadB      = LIGHT_RED;
            gLights.pedestrian = PED_RED;

            break;

        case STATE_B_GREEN:

            gLights.roadA      = LIGHT_RED;
            gLights.roadB      = LIGHT_GREEN;
            gLights.pedestrian = PED_RED;

            break;

        case STATE_B_YELLOW:

            gLights.roadA      = LIGHT_RED;
            gLights.roadB      = LIGHT_YELLOW;
            gLights.pedestrian = PED_RED;

            break;

        case STATE_ALL_RED_TO_A:
        case STATE_ALL_RED_TO_B:

            gLights.roadA      = LIGHT_RED;
            gLights.roadB      = LIGHT_RED;
            gLights.pedestrian = PED_RED;

            break;

        case STATE_PEDESTRIAN:

            gLights.roadA      = LIGHT_RED;
            gLights.roadB      = LIGHT_RED;
            gLights.pedestrian = PED_GREEN;

            break;

        case STATE_EMERGENCY:

            gLights.roadA      = LIGHT_RED;
            gLights.roadB      = LIGHT_RED;
            gLights.pedestrian = PED_RED;

            break;

        default:

            gLights.roadA      = LIGHT_RED;
            gLights.roadB      = LIGHT_RED;
            gLights.pedestrian = PED_RED;

            break;
    }
}

/*=============================================================================
                            DEBUG
=============================================================================*/
#ifndef UNIT_TEST
static void printSystemState(void)
{
    printf(
        "T=%3u | ST=%2d | A=%-6s | B=%-6s | PED=%-5s\n",

        gTick,
        gState,

        vehicleLightToString(gLights.roadA),
        vehicleLightToString(gLights.roadB),

        pedestrianLightToString(gLights.pedestrian)
    );
}
#endif
/*=============================================================================
                            EVENTOS
=============================================================================*/

void pedestrianButtonPressed(void)
{
    printf("[EVENT] Pedestrian button pressed\n");

    gPedestrianRequest = true;
}

void emergencyButtonPressed(void)
{
    printf("[EVENT] Emergency requested\n");

    gEmergencyPending = true;
}

void emergencyButtonReleased(void)
{
    printf("[EVENT] Emergency released\n");

    gEmergencyPending = false;
    gEmergencyActive  = false;

    gState = STATE_INIT;
    gStateTimer = 0U;

    updateOutputs();
}

/*=============================================================================
                            FSM AUX
=============================================================================*/

void changeState(SystemState_t newState)
{
    gState = newState;

    gStateTimer = 0U;

    updateOutputs();
}

/*=============================================================================
                            FSM
=============================================================================*/

void trafficControllerTick(void)
{
    gStateTimer++;

    switch(gState)
    {
        /*=========================================================
                                INIT
        =========================================================*/

        case STATE_INIT:

            if(gStateTimer >= TIME_ALL_RED)
            {
                changeState(STATE_A_GREEN);
            }

            break;

        /*=========================================================
                                A GREEN
        =========================================================*/

        case STATE_A_GREEN:

            if(gStateTimer >= TIME_GREEN_A)
            {
                changeState(STATE_A_YELLOW);
            }

            break;

        /*=========================================================
                                A YELLOW
        =========================================================*/

        case STATE_A_YELLOW:

            if(gStateTimer >= TIME_YELLOW_A)
            {
                changeState(STATE_ALL_RED_TO_B);
            }

            break;

        /*=========================================================
                                ALL RED TO B
        =========================================================*/

        case STATE_ALL_RED_TO_B:

            if(gStateTimer >= TIME_ALL_RED)
            {
                /*
                    Final do ciclo A
                */

                gCycleACompleted = true;

                /*
                    RF-17:
                    Emergência só entra
                    em estado seguro.
                */

                if(gEmergencyPending == true)
                {
                    gEmergencyActive = true;

                    changeState(STATE_EMERGENCY);
                }
                else
                {
                    /*
                        RF-23:
                        Pedestre somente se lock liberado.
                    */

                    if((gPedestrianRequest == true) &&
                       (gPedestrianLock == false))
                    {
                        gPedestrianRequest = false;

                        /*
                            Após pedestre:
                            locka novos atendimentos.
                        */

                        gPedestrianLock = true;

                        gCycleACompleted = false;
                        gCycleBCompleted = false;

                        changeState(STATE_PEDESTRIAN);
                    }
                    else
                    {
                        changeState(STATE_B_GREEN);
                    }
                }
            }

            break;

        /*=========================================================
                                B GREEN
        =========================================================*/

        case STATE_B_GREEN:

            if(gStateTimer >= TIME_GREEN_B)
            {
                changeState(STATE_B_YELLOW);
            }

            break;

        /*=========================================================
                                B YELLOW
        =========================================================*/

        case STATE_B_YELLOW:

            if(gStateTimer >= TIME_YELLOW_B)
            {
                changeState(STATE_ALL_RED_TO_A);
            }

            break;

        /*=========================================================
                                ALL RED TO A
        =========================================================*/

        case STATE_ALL_RED_TO_A:

            if(gStateTimer >= TIME_ALL_RED)
            {
                /*
                    Final do ciclo B
                */

                gCycleBCompleted = true;

                /*
                    RF-23:
                    Libera novos pedestres
                    apenas após:
                        1 ciclo A
                        1 ciclo B
                */

                if((gCycleACompleted == true) &&
                   (gCycleBCompleted == true))
                {
                    gPedestrianLock = false;
                }

                /*
                    Emergência
                */

                if(gEmergencyPending == true)
                {
                    gEmergencyActive = true;

                    changeState(STATE_EMERGENCY);
                }
                else
                {
                    if((gPedestrianRequest == true) &&
                       (gPedestrianLock == false))
                    {
                        gPedestrianRequest = false;

                        gPedestrianLock = true;

                        gCycleACompleted = false;
                        gCycleBCompleted = false;

                        changeState(STATE_PEDESTRIAN);
                    }
                    else
                    {
                        changeState(STATE_A_GREEN);
                    }
                }
            }

            break;

        /*=========================================================
                                PEDESTRIAN
        =========================================================*/

        case STATE_PEDESTRIAN:

            if(gStateTimer >= TIME_PEDESTRIAN)
            {
                changeState(STATE_A_GREEN);
            }

            break;

        /*=========================================================
                                EMERGENCY
        =========================================================*/

        case STATE_EMERGENCY:

            /*
                RF-19:
                Nenhuma transição automática.
            */

            break;

        /*=========================================================
                                DEFAULT
        =========================================================*/

        default:

            changeState(STATE_EMERGENCY);

            break;
    }
}

/*=============================================================================
                                    MAIN
=============================================================================*/
#ifndef UNIT_TEST

int main(void)
{
    changeState(STATE_INIT);

    while(1U)
    {
        /*=========================================================
                                EVENTOS
        =========================================================*/

        if(gTick == 15U)
        {
            pedestrianButtonPressed();
        }

        /*
            Nova solicitação de pedestre.
            NÃO deve ser atendida imediatamente
            por causa do RF-23.
        */

        if(gTick == 20U)
        {
            pedestrianButtonPressed();
        }

        if(gTick == 40U)
        {
            emergencyButtonPressed();
        }

        if(gTick == 70U)
        {
            emergencyButtonReleased();
        }

        /*=========================================================
                                FSM
        =========================================================*/

        trafficControllerTick();

        /*=========================================================
                                DEBUG
        =========================================================*/

        printSystemState();

        /*=========================================================
                                TICK
        =========================================================*/

        gTick++;

        /*
            Simulação limitada.
        */

        if(gTick > 100U)
        {
            break;
        }
    }

    return 0U;
}
#endif