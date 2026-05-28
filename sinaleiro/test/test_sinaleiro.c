#include <stdbool.h>
#include "../unity/unity.h"
#include "../src/sinaleiro2.h"

/*=========================================================
    Helpers
=========================================================*/

static void runTicks(unsigned int n)
{
    for(unsigned int i = 0; i < n; i++)
    {
        trafficControllerTick();
    }
}


int waitForState(int expectedState, unsigned int maxTicks)
{
    for(unsigned int i = 0; i < maxTicks; i++)
    {
        trafficControllerTick();

        if(getState() == expectedState)
            return 1;
    }

    return 0;
}

/*=============================================================================
                                SETUP
=============================================================================*/

void setUp(void)
{
    resetSystem();
}

void tearDown(void)
{
}


/*=============================================================================
                                Testes função vehicleLightToString()
=============================================================================*/
// C1 — color = 0
void test_vehicleLight_color_0_should_return_RED(void)
{
    TEST_ASSERT_EQUAL_STRING("RED", vehicleLightToString(LIGHT_RED));
}

// C2 — color = 1
void test_vehicleLight_color_1_should_return_YELLOW(void)
{
    TEST_ASSERT_EQUAL_STRING("YELLOW", vehicleLightToString(LIGHT_YELLOW));
}

// C3 — color = 2
void test_vehicleLight_color_2_should_return_GREEN(void)
{
    TEST_ASSERT_EQUAL_STRING("GREEN", vehicleLightToString(LIGHT_GREEN));
}

// C4 — valor inválido
void test_vehicleLight_invalid_color_should_return_UNKNOWN(void)
{
    TEST_ASSERT_EQUAL_STRING("UNKNOWN", vehicleLightToString(3));
}

/*=============================================================================
                                Testes função pedestrianLightToString()
=============================================================================*/
// C1 — color = 0 (RED)
void test_pedestrianLight_color_0_should_return_RED(void)
{
    TEST_ASSERT_EQUAL_STRING("RED", pedestrianLightToString(PED_RED));
}

// C2 — color = 1 (GREEN)
void test_pedestrianLight_color_1_should_return_GREEN(void)
{
    TEST_ASSERT_EQUAL_STRING("GREEN", pedestrianLightToString(PED_GREEN));
}

// C3 — valor inválido
void test_pedestrianLight_invalid_color_should_return_UNKNOWN(void)
{
    TEST_ASSERT_EQUAL_STRING("UNKNOWN", pedestrianLightToString(2));
}

/*=============================================================================
                                Testes função updateOutputs()
=============================================================================*/
// C1 — STATE_INIT
void test_updateOutputs_STATE_INIT(void)
{
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadA()); // RED
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadB()); // RED
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian()); // RED
}

// C2 — STATE_A_GREEN
void test_updateOutputs_STATE_A_GREEN(void)
{
    unsigned int targetTicks = TIME_ALL_RED;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(LIGHT_GREEN, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadB());
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian());
}

// C3 — STATE_A_YELLOW
void test_updateOutputs_STATE_A_YELLOW(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(LIGHT_YELLOW, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadB());
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian());
}

// C4 — STATE_B_GREEN
void test_updateOutputs_STATE_B_GREEN(void)
{
    unsigned int targetTicks =
        TIME_ALL_RED +
        TIME_GREEN_A +
        TIME_YELLOW_A +
        TIME_ALL_RED;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_GREEN, getRoadB());
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian());
}

// C5 — STATE_B_YELLOW
void test_updateOutputs_STATE_B_YELLOW(void)
{
    unsigned int targetTicks =
        TIME_ALL_RED +
        TIME_GREEN_A +
        TIME_YELLOW_A +
        TIME_ALL_RED +
        TIME_GREEN_B;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_YELLOW, getRoadB());
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian());
}

// C6 — ALL RED TO B
void test_updateOutputs_ALL_RED_TO_B(void)
{
    unsigned int targetTicks =
        TIME_ALL_RED +
        TIME_GREEN_A +
        TIME_YELLOW_A;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadB());
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian());
}

// C6 — ALL RED TO A
void test_updateOutputs_ALL_RED_TO_A(void)
{
    unsigned int targetTicks =
        TIME_ALL_RED +
        TIME_GREEN_A +
        TIME_YELLOW_A +
        TIME_ALL_RED +
        TIME_GREEN_B +
        TIME_YELLOW_B;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadB());
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian());
}

// C7 — PEDESTRIAN
void test_updateOutputs_PEDESTRIAN(void)
{
    pedestrianButtonPressed();

    unsigned int targetTicks =
        TIME_ALL_RED +
        TIME_GREEN_A +
        TIME_YELLOW_A +
        TIME_ALL_RED;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadB());
    TEST_ASSERT_EQUAL(PED_GREEN, getPedestrian());
}

// C8 — EMERGENCY
void test_updateOutputs_EMERGENCY(void)
{
    emergencyButtonPressed();

    unsigned int targetTicks =
        TIME_ALL_RED +
        TIME_GREEN_A +
        TIME_YELLOW_A +
        TIME_ALL_RED;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadB());
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian());
}

// C9 — INVALID
void test_updateOutputs_INVALID(void)
{
    changeState(99);

    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadB());
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian());
}

/*=============================================================================
                                Testes função pedestrianButtonPressed()
=============================================================================*/
// C1 — Botão pressionado → deve eventualmente entrar em STATE_PEDESTRIAN
void test_pedestrian_request_should_eventually_enter_pedestrian(void)
{
    pedestrianButtonPressed();

    TEST_ASSERT_TRUE(waitForState(STATE_PEDESTRIAN, EVENTUALLY_TIMEOUT));
}

/*=============================================================================
                                Testes função emergencyButtonPressed()
=============================================================================*/
// C1 — Botão de emergência pressionado → deve eventualmente entrar em STATE_EMERGENCY
void test_emergency_request_should_eventually_enter_emergency(void)
{
    emergencyButtonPressed();

    TEST_ASSERT_TRUE(waitForState(STATE_EMERGENCY, EVENTUALLY_TIMEOUT));
}


/*=============================================================================
                                Testes função emergencyButtonReleased()
=============================================================================*/
// C1 — Emergência liberada → deve eventualmente retornar para STATE_INIT
void test_emergency_release_should_eventually_return_to_init(void)
{
    emergencyButtonPressed();

    TEST_ASSERT_TRUE(waitForState(STATE_EMERGENCY, EVENTUALLY_TIMEOUT));

    emergencyButtonReleased();

    TEST_ASSERT_TRUE(waitForState(STATE_INIT, EVENTUALLY_TIMEOUT));
}

/*=============================================================================
                                Testes função changeState()
=============================================================================*/
// C1 — Transição para estado válido → deve atualizar o estado
void test_change_state_with_valid_state_should_update_state(void)
{
    changeState(STATE_A_GREEN);

    TEST_ASSERT_EQUAL(STATE_A_GREEN, getState());
}


// C2 — Transição para estado inválido → deve manter condição segura
void test_change_state_with_invalid_state_should_keep_safe_condition(void)
{
    changeState(9);

    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadB());
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian());
}




// /*=============================================================================
//                                 Testes função trafficControllerTick()
// =============================================================================*/

//     C1 — STATE_INIT sem timeout
void test_C1_init_should_remain_init(void)
{   
    unsigned int targetTicks = TIME_ALL_RED - 1U;
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_INIT, getState());
}

//     C2 — STATE_INIT timeout
void test_C2_init_should_go_to_A_GREEN(void)
{
    unsigned int targetTicks = TIME_ALL_RED;
    
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_A_GREEN, getState());
}

//     C3 — A_GREEN sem timeout
void test_C3_A_green_should_remain(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A - 1U;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_A_GREEN, getState());
}

//     C4 — A_GREEN timeout
void test_C4_A_green_should_go_to_yellow(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A;
    
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_A_YELLOW, getState());
}

//     C5 — A_YELLOW sem timeout
void test_C5_A_yellow_should_remain(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A - 1U;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_A_YELLOW, getState());
}

//     C6 — A_YELLOW timeout
void test_C6_A_yellow_should_go_to_all_red_to_b(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A;
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_ALL_RED_TO_B, getState());
}

//     C7 — ALL_RED_TO_B sem timeout
void test_C7_all_red_to_b_should_remain(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED - 1U;
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_ALL_RED_TO_B, getState());
}

//     C8 — Emergência no ALL_RED_TO_B
void test_C8_emergency_should_go_to_emergency(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A;
    runTicks(targetTicks);

    emergencyButtonPressed();

    runTicks(TIME_ALL_RED);

    TEST_ASSERT_EQUAL(STATE_EMERGENCY, getState());
}

//     C9 — Pedestre no ALL_RED_TO_B
void test_C9_pedestrian_should_go_to_pedestrian(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A;
    runTicks(targetTicks);

    pedestrianButtonPressed();

    runTicks(TIME_ALL_RED);

    TEST_ASSERT_EQUAL(STATE_PEDESTRIAN, getState());
}

//     C10 — fluxo normal para B_GREEN
void test_C10_should_go_to_b_green(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_B_GREEN, getState());
}

//     C11 — B_GREEN sem timeout
void test_C11_b_green_should_remain(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED + TIME_GREEN_B - 1U;
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_B_GREEN, getState());
}

//     C12 — B_GREEN timeout
void test_C12_b_green_should_go_to_yellow(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED + TIME_GREEN_B;
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_B_YELLOW, getState());
}

//     C13 — B_YELLOW sem timeout
void test_C13_b_yellow_should_remain(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED + TIME_GREEN_B + TIME_YELLOW_B - 1U;
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_B_YELLOW, getState());
}

//     C14 — B_YELLOW timeout
void test_C14_b_yellow_should_go_to_all_red_to_a(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED + TIME_GREEN_B + TIME_YELLOW_B;
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_ALL_RED_TO_A, getState());
}

//     C15 — ALL_RED_TO_A sem timeout
void test_C15_all_red_to_a_should_remain(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED + TIME_GREEN_B + TIME_YELLOW_B + TIME_ALL_RED - 1U;
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_ALL_RED_TO_A, getState());
}

//     C16 — emergência no ALL_RED_TO_A
void test_C16_emergency_should_go_to_emergency(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED + TIME_GREEN_B + TIME_YELLOW_B;
    runTicks(targetTicks);

    emergencyButtonPressed();

    runTicks(TIME_ALL_RED);

    TEST_ASSERT_EQUAL(STATE_EMERGENCY, getState());
}

//     C17 — pedestre no ALL_RED_TO_A
void test_C17_pedestrian_should_go_to_pedestrian(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED +
                               TIME_GREEN_B + TIME_YELLOW_B;
    runTicks(targetTicks);

    pedestrianButtonPressed();

    runTicks(TIME_ALL_RED);

    TEST_ASSERT_EQUAL(STATE_PEDESTRIAN, getState());
}

//     C18 — volta para A_GREEN
void test_C18_should_go_to_A_green(void)
{
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED +
                               TIME_GREEN_B + TIME_YELLOW_B + TIME_ALL_RED;
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_A_GREEN, getState());
}

//     C19 — PEDESTRIAN sem timeout
void test_C19_pedestrian_should_remain(void)
{
    pedestrianButtonPressed();

    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED + TIME_PEDESTRIAN - 1U;

    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_PEDESTRIAN, getState());
}

//     C20 — PEDESTRIAN timeout
void test_C20_pedestrian_should_go_to_A_green(void)
{
    pedestrianButtonPressed();

    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED + TIME_PEDESTRIAN;
    runTicks(targetTicks);

    TEST_ASSERT_EQUAL(STATE_A_GREEN, getState());
}

//     C21 — EMERGENCY sem transição
void test_C21_emergency_should_stay(void)
{
    emergencyButtonPressed();
    unsigned int targetTicks = TIME_ALL_RED + TIME_GREEN_A + TIME_YELLOW_A + TIME_ALL_RED;
    runTicks(targetTicks);
    TEST_ASSERT_EQUAL(STATE_EMERGENCY, getState());
    runTicks(EVENTUALLY_TIMEOUT);
    TEST_ASSERT_EQUAL(STATE_EMERGENCY, getState());

}

//     C22 — estado inválido (forçado via changeState indireto)
void test_C22_invalid_should_go_emergency(void)
{

    changeState(99);
    
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadA());
    TEST_ASSERT_EQUAL(LIGHT_RED, getRoadB());
    TEST_ASSERT_EQUAL(PED_RED, getPedestrian());
}


/*=============================================================================
                                MAIN TEST
=============================================================================*/
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_vehicleLight_color_0_should_return_RED);
    RUN_TEST(test_vehicleLight_color_1_should_return_YELLOW);
    RUN_TEST(test_vehicleLight_color_2_should_return_GREEN);
    RUN_TEST(test_vehicleLight_invalid_color_should_return_UNKNOWN);

    RUN_TEST(test_pedestrianLight_color_0_should_return_RED);
    RUN_TEST(test_pedestrianLight_color_1_should_return_GREEN);
    RUN_TEST(test_pedestrianLight_invalid_color_should_return_UNKNOWN);

    RUN_TEST(test_updateOutputs_STATE_INIT);
    RUN_TEST(test_updateOutputs_STATE_A_GREEN);
    RUN_TEST(test_updateOutputs_STATE_A_YELLOW);
    RUN_TEST(test_updateOutputs_STATE_B_GREEN);
    RUN_TEST(test_updateOutputs_STATE_B_YELLOW);
    RUN_TEST(test_updateOutputs_ALL_RED_TO_B);
    RUN_TEST(test_updateOutputs_ALL_RED_TO_A);
    RUN_TEST(test_updateOutputs_PEDESTRIAN);
    RUN_TEST(test_updateOutputs_EMERGENCY);
    RUN_TEST(test_updateOutputs_INVALID);

    RUN_TEST(test_pedestrian_request_should_eventually_enter_pedestrian);

    RUN_TEST(test_emergency_request_should_eventually_enter_emergency);

    RUN_TEST(test_emergency_release_should_eventually_return_to_init);

    RUN_TEST(test_change_state_with_valid_state_should_update_state);
    RUN_TEST(test_change_state_with_invalid_state_should_keep_safe_condition);

    RUN_TEST(test_C1_init_should_remain_init);
    RUN_TEST(test_C2_init_should_go_to_A_GREEN);
    RUN_TEST(test_C3_A_green_should_remain);
    RUN_TEST(test_C4_A_green_should_go_to_yellow);
    RUN_TEST(test_C5_A_yellow_should_remain);
    RUN_TEST(test_C6_A_yellow_should_go_to_all_red_to_b);
    RUN_TEST(test_C7_all_red_to_b_should_remain);
    RUN_TEST(test_C8_emergency_should_go_to_emergency);
    RUN_TEST(test_C9_pedestrian_should_go_to_pedestrian);
    RUN_TEST(test_C10_should_go_to_b_green);
    RUN_TEST(test_C11_b_green_should_remain);
    RUN_TEST(test_C12_b_green_should_go_to_yellow);
    RUN_TEST(test_C13_b_yellow_should_remain);
    RUN_TEST(test_C14_b_yellow_should_go_to_all_red_to_a);
    RUN_TEST(test_C15_all_red_to_a_should_remain);
    RUN_TEST(test_C16_emergency_should_go_to_emergency);
    RUN_TEST(test_C17_pedestrian_should_go_to_pedestrian);
    RUN_TEST(test_C18_should_go_to_A_green);
    RUN_TEST(test_C19_pedestrian_should_remain);
    RUN_TEST(test_C20_pedestrian_should_go_to_A_green);
    RUN_TEST(test_C21_emergency_should_stay);
    RUN_TEST(test_C22_invalid_should_go_emergency);

    printf("Programa executado!\n");

    printf("Pressione ENTER para sair...");
    getchar();

    return UNITY_END();
}