/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
#if 11
int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}
#else

enum
{
  IDLE_MODE = 'a',
  SLEEP_MODE = 'b',
  DEEP_SLEEP_MODE ='c',
}LOW_POWER_MODES;

uint8_t cmd = 0;
/*
void timeout (uint32_t status, uintptr_t context)
{
    LED_Toggle();   
}*/

void display_menu (void)
{
    SYS_CONSOLE_PRINT("\n\n\n\rSelect the low power mode to enter");
    SYS_CONSOLE_PRINT("\n\ra) Idle Mode");
    SYS_CONSOLE_PRINT("\n\rb) Sleep Mode"); 
    SYS_CONSOLE_PRINT("\n\rc) Deep Sleep Mode");
    SYS_CONSOLE_PRINT("\n\rEnter your choice");    
    //scanf("%c", &cmd);
    cmd =IDLE_MODE;
}
int main ( void )
{
    RCON_RESET_CAUSE resetCause;
    char typedChar[4];
    char savedChar[4]="init";
    
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    //PMUCLKCTRLbits.WLDOOFF = 1;
    //PMUCLKCTRLbits.WCMRET = 0;
    resetCause = RCON_ResetCauseGet();
    
    POWER_ReleaseGPIO();
    
    __builtin_enable_interrupts();

    /* Check if RESET was after deep sleep wakeup */
    if (((resetCause & RCON_RESET_CAUSE_DPSLP) == RCON_RESET_CAUSE_DPSLP))
    {
        RCON_ResetCauseClear(RCON_RESET_CAUSE_DPSLP);
        
        /* Recollect the saved characters */
        savedChar[0]= POWER_DSGPR_Read(POWER_DSGPR0);
        //savedChar[1]= POWER_DSGPR_Read(POWER_DSGPR1);
        //savedChar[2]= POWER_DSGPR_Read(POWER_DSGPR2);
        //savedChar[3]= POWER_DSGPR_Read(POWER_DSGPR3);

        SYS_CONSOLE_PRINT("\r\n\r\nDevice woke up after deep sleep mode due to INT0 interrupt reset.\r\n");
        SYS_CONSOLE_PRINT("Four letters which were typed before entering into deep sleep state are:\r\n");
        UART1_Write((uint8_t*)&savedChar[0],sizeof(savedChar));
    }
    
    SYS_CONSOLE_PRINT("\n\n\r----------------------------------------------");
    SYS_CONSOLE_PRINT("\n\r                 LOW power demo"               );
    SYS_CONSOLE_PRINT("\n\r----------------------------------------------"); 
    
    //TMR1_CallbackRegister(&timeout, (uintptr_t) NULL);
    //TMR1_Start();
    /* INT0 interrupt is used to wake up from Deep Sleep */
    EVIC_ExternalInterruptEnable(EXTERNAL_INT_0);
    /* CN interrupt is used to wake up from Idle or Sleep mode */
    GPIO_PinInterruptEnable(CN_INT_PIN);
    
    //POWER_LowPowerModeEnter(LOW_POWER_DEEP_SLEEP_MODE);
    
    display_menu();
#if 00
    while(1)
    {
               printf("\r\n#### Now entering deep sleep mode...\r\n");
                printf("\n\rPress Switch SW1 to wakeup the device         ");
               
                POWER_LowPowerModeEnter(LOW_POWER_DEEP_SLEEP_MODE);
    }
#endif    
#if 11    
    while(1)
    {
        SYS_CMD_Tasks();
        switch(cmd)
        {
            case IDLE_MODE:
            {
                SYS_CONSOLE_PRINT("\n\rEntering IDLE Mode");
                SYS_CONSOLE_PRINT("\n\rPress Switch SW1 to wakeup the device");
                //TMR1_Stop();
                
                POWER_LowPowerModeEnter(LOW_POWER_IDLE_MODE);
                __builtin_disable_interrupts();
                //__builtin_enable_interrupts();
                SYS_CONSOLE_PRINT("\n\rChange Notice Interrupt detected, exiting IDLE mode......");
                //TMR1_Start();
                while(1);
                display_menu();
                break;
            }
            case SLEEP_MODE:
            {
                SYS_CONSOLE_PRINT("\n\rEntering SLEEP Mode");
                SYS_CONSOLE_PRINT("\n\rPress Switch SW1 to wakeup the device         ");
                //TMR1_Stop();
                POWER_LowPowerModeEnter(LOW_POWER_SLEEP_MODE);
                SYS_CONSOLE_PRINT("\n\rChange Notice Interrupt detected, exiting SLEEP mode......");
                //TMR1_Start();
                display_menu();
                break;
            }
            case DEEP_SLEEP_MODE:
            {
                SYS_CONSOLE_PRINT("\r\n\r\n#### Type four letters which you want to save in deep sleep state ####\r\n");

                UART1_Read(&typedChar[0], 4);

                /* Save the typed characters in DSGPR registers which will retain during deep sleep */
                POWER_DSGPR_Write(POWER_DSGPR0, typedChar[0]);
                //POWER_DSGPR_Write(POWER_DSGPR1, typedChar[1]);
                //POWER_DSGPR_Write(POWER_DSGPR2, typedChar[2]);
                //POWER_DSGPR_Write(POWER_DSGPR3, typedChar[3]);

                SYS_CONSOLE_PRINT("Letters which were typed are:\r\n");
                UART1_Write((uint8_t*)&typedChar[0],sizeof(typedChar));

                SYS_CONSOLE_PRINT("\r\n#### Now entering deep sleep mode...\r\n");
                SYS_CONSOLE_PRINT("\n\rPress Switch SW1 to wakeup the device         ");
                //GPIO_RK4_Clear();
                //GPIO_RK5_Clear();
                POWER_LowPowerModeEnter(LOW_POWER_DEEP_SLEEP_MODE);
                break;
            }
            default:
            {
                SYS_CONSOLE_PRINT("\n\rInvalid choice");
                display_menu();
                break;
            }
        } 
    }
#endif
    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}



#endif

/*******************************************************************************
 End of File
*/

