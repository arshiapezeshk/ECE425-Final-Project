/*
 * @file main.c
 *
 * @brief Main source code for the UART_BLE program.
 *
 * This file contains the main entry point and function definitions for the UART_BLE program.
 * 
 * The following modules are used:
 *  - UART0: Used to print characters on the serial terminal
 *  - UART1: Used to establish communication between the Tiva TM4C123G LaunchPad
 *           and an Adafruit BLE UART module
 *
 * The Adafruit BLE UART module uses the following pinout:
 *  - BLE UART MOD (Pin 1)  <-->  Tiva LaunchPad Pin GND
 *  - BLE UART CTS (Pin 2)	<-->  Tiva LaunchPad Pin GND
 *  - BLE UART TXO (Pin 3) 	<-->  Tiva LaunchPad Pin PB0 (U1RX)
 *  - BLE UART RXI (Pin 4)  <-->  Tiva LaunchPad Pin PB1 (U1TX)
 *  - BLE UART VIN (Pin 5)  <-->  Tiva LaunchPad VCC (3.3V)
 *  - BLE UART RTS (Pin 6)  <-->  Not Connected
 *  - BLE UART GND (Pin 7)  <-->  Tiva LaunchPad GND
 *  - BLE UART DFU (Pin 8)  <-->  Not Connected
 *
 * @note For more information regarding the UART module, refer to the
 * Universal Asynchronous Receivers / Transmitters (UARTs) section
 * of the TM4C123GH6PM Microcontroller Datasheet.
 * - Link: https://www.ti.com/lit/gpn/TM4C123GH6PM
 *
 * @note For more information regarding the Adafruit BLE UART module, refer
 * to the following link.
 * - Link: https://www.adafruit.com/product/2479
 *
 * @author Arshia Pezeshk
 */
 
#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"
#include "UART0.h"
#include "UART_BLE.h"
#include "GPIO.h"
#include "PWM_Clock.h"
#include "PWM1_3.h"
#include "Motor.h"

#define BUFFER_SIZE   128

// Initialize an array to store the characters received from the Adafruit BLE UART module
char UART_BLE_Buffer[BUFFER_SIZE];

void Process_UART_BLE_Data(char UART_BLE_Buffer[]);

// Initialize a global variable to store the PWM duty cycle value for the RGB LED
static uint16_t RGB_LED_duty_cycle = 0;

// Initialize a global variable to store the PWM duty cycle value for the motors
static uint16_t motor_duty_cycle = 0;

int main(void)
{		
	// Initialize the SysTick timer used to provide blocking delay functions
	SysTick_Delay_Init();
	
	// Initialize the RGB LED on the TM4C123G LaunchPad
	RGB_LED_Init();
	
	// Initialize the PWM clock to use the PWM clock divisor as the source
	// and update the PWM clock frequency to 50 MHz / 16 = 3.125 MHz
	PWM_Clock_Init();
	
	// Initialize the Module 1 PWM Generator 3 (PWM1_3) to generate a PWM signal
	// using the PF1, PF2, and PF3 pins that have a frequency of 50 Hz with 5% duty cycle
	// Period constant = ((50 MHz / 16) / 50 Hz) = 62500
	// Duty cycle = (62500 * 0.05) = 3125
	PWM1_3_Init(62500, 0);
	
	// Initialize the Motors
	Motor_Init(208, 0);
	
	// Initialize the UART0 module which will be used to print characters on the serial terminal
	UART0_Init();
	
	// Initialize the UART1 module which will be used to communicate with the Adafruit BLE UART module
	UART_BLE_Init();
	
	// Provide a short delay after initialization and reset the Adafruit BLE UART module
	SysTick_Delay1ms(1000);
	
	// Send a message to the Adafruit BLE UART module to check if the connection is stable
	UART_BLE_Output_String("UART BLE Active\n");
	SysTick_Delay1ms(1000);
	
	while(1)
	{
		int string_size = UART_BLE_Input_String(UART_BLE_Buffer);
		
		// THE CODE AFTER THIS COMMENT IS PURELY USED FOR DEBUGGING PURPOSES AND HAS NO AFFECT ON TACTICAL FUCNTIONALITY
		UART0_Output_String("String Size: ");
		UART0_Output_Unsigned_Decimal(string_size);
		UART0_Output_Newline();
		
		UART0_Output_String("UART BLE Data: ");
		
		for (int i = 0; i < string_size; i++)
		{
			if (UART_BLE_Buffer[i] == 0)
			{
				UART0_Output_Character('A');
			}
			else
			{
				UART0_Output_Character(UART_BLE_Buffer[i]);
			}
		}
		
		UART0_Output_Newline();
		// THE CODE BEFORE THIS COMMENT IS PURELY USED FOR DEBUGGING PURPOSES AND HAS NO AFFECT ON TACTICAL FUCNTIONALITY
		Process_UART_BLE_Data(UART_BLE_Buffer);
		
		UART0_Output_Newline();
	}
}

void Process_UART_BLE_Data(char UART_BLE_Buffer[])
{
	if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B51"))
	{
			PWM1_3B_Update_Duty_Cycle(RGB_LED_duty_cycle);	
			Motor_Forward(motor_duty_cycle);
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B50"))
	{
		PWM1_3B_Update_Duty_Cycle(0);
		Motor_Stop();	
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B61"))
	{
		PWM1_2B_Update_Duty_Cycle(RGB_LED_duty_cycle);
		Motor_Backward(motor_duty_cycle);
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B60"))
	{
		PWM1_2B_Update_Duty_Cycle(0);
		Motor_Stop();
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B71"))
	{
		PWM1_3A_Update_Duty_Cycle(RGB_LED_duty_cycle);
		Motor_Left_Turn(motor_duty_cycle);
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B70"))
	{
		PWM1_3A_Update_Duty_Cycle(0);
		Motor_Stop();
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B81"))
	{
		PWM1_3A_Update_Duty_Cycle(RGB_LED_duty_cycle);
		PWM1_3B_Update_Duty_Cycle(RGB_LED_duty_cycle);
		PWM1_2B_Update_Duty_Cycle(RGB_LED_duty_cycle);
		Motor_Right_Turn(motor_duty_cycle);
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B80"))
	{
		PWM1_3A_Update_Duty_Cycle(0);
		PWM1_3B_Update_Duty_Cycle(0);
		PWM1_2B_Update_Duty_Cycle(0);
		Motor_Stop();
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B11"))
	{
		RGB_LED_duty_cycle = 50000;
		motor_duty_cycle = 200;
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B21"))
	{
		RGB_LED_duty_cycle = 5000;
		motor_duty_cycle = 150;
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B31"))
	{
		RGB_LED_duty_cycle = 500;
		motor_duty_cycle = 100;
	}
	
	else if (Check_UART_BLE_Data(UART_BLE_Buffer, "!B41"))
	{
		RGB_LED_duty_cycle = 50;
		motor_duty_cycle = 0;
	}	
	
	else
	{
		UART0_Output_String("UART BLE Command Not Found");
		UART0_Output_Newline();
	}
}
