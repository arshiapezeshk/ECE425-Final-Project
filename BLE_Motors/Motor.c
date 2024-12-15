#include "Motor.h"

void Motor_Init(uint16_t period_constant, uint16_t duty_cycle)
{	
	// Return from the function if the specified duty_cycle is greater than
	// or equal to the given period. The duty cycle cannot exceed 99%
	if (duty_cycle >= period_constant) return;
	
	// Enable the clock to PWM Module 1 by setting the
	// R1 bit (Bit 1) in the RCGCPWM register
	SYSCTL->RCGCPWM |= 0x02;
	
	// Enable the clock to GPIO Port A by setting the
	// R0 bit (Bit 0) in the RCGCGPIO register
	SYSCTL->RCGCGPIO |= 0x01;
	
	// Configure the PA6 and PA7 pin to use the alternate function (M1PWM2 and M1PWM3)
	// by setting Bits 6 and 7 in the AFSEL register
	GPIOA->AFSEL |= 0x40;
	GPIOA->AFSEL |= 0x80;
	GPIOA->AFSEL &= ~0x3C;
	GPIOA->DIR |= 0x3C;
	
	// Clear the PMC6 and PMC7 field (Bits 31 to 24) in the PCTL register
	GPIOA->PCTL &= ~0x0F000000;
	GPIOA->PCTL &= ~0xF0000000;
	
	// Configure the PF2 pin to operate as a Module 1 PWM6 pin (M1PWM6)
	// by writing 0x5 to the PMC2 field (Bits 11 to 8) in the PCTL register
	// The 0x5 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOA->PCTL |= 0x05000000;
	GPIOA->PCTL |= 0x50000000;
	
	// Enable the digital functionality for the PA6 and PA7 pin
	// by setting Bits 6 and 7 in the DEN register
	GPIOA->DEN |= 0x40;	
	GPIOA->DEN |= 0x80;
	GPIOA->DEN |= 0x3C;
	GPIOA->DATA &= ~0x3C;
	
	// Disable the Module 1 PWM Generator 1 block (PWM1_1) before
	// configuration by clearing the ENABLE bit (Bit 0) in the PWM1CTL register
	PWM1->_1_CTL &= ~0x01;
	
	// Configure the counter for the PWM1_1 block to
	// use Count-Down mode by clearing the MODE bit (Bit 1)
	// in the PWM1CTL register. The counter will count from the load value
	// to 0, and the wrap back to the load value
	PWM1->_1_CTL &= ~0x02;
	
	// Set the ACTCMPAD field (bits 7 to 6) to 0x3 in the PWM3GENA register
	// to drive the PWM signal high when the counter matches
	// the comparator (i.e. the value in PWM3CMPA) while counting down
	PWM1->_1_GENA |= 0x0C0;
	PWM1->_1_GENB |= 0xC00;
	
	// Set the ACTLOAD field (Bits 3 to 2) to 0x2 in the PWM3GENA register
	// to drive the PWM signal low when the counter matches the value
	// in the PWM3LOAD register
	PWM1->_1_GENA |= 0x08;
	PWM1->_1_GENB |= 0x08;
	
	// Set the period by writing to the LOAD field (Bits 15 to 0)
	// in the PWM3LOAD register. This determines the number of clock
	// cycles needed to count down to zero
	PWM1->_1_LOAD = (period_constant - 1);

	// Set the duty cycle by writing to the COMPA field (Bits 15 to 0)
	// in the PWM3CMPA register. When the counter matches the value in this register,
	// the PWM signal will be driven high
	PWM1->_1_CMPA = (duty_cycle - 1);
	PWM1->_1_CMPB = (duty_cycle - 1);	
	
	// Enable the PWM1_3 block after configuration by setting the
	// ENABLE bit (Bit 0) in the PWM3CTL register
	PWM1->_1_CTL |= 0x01;

	// Enable the PWM1_3 signal to be passed to the PF2 pin (M1PWM6)
	// by setting the PWM6EN bit (Bit 6) in the PWMENABLE register
	PWM1->ENABLE |= 0x04;
	PWM1->ENABLE |= 0x08;

}

void PWM1_1A_Update_Duty_Cycle(uint16_t duty_cycle)
{
	// Set the duty cycle by writing to the COMPA field (Bits 15 to 0)
	// in the PWM3CMPA register. When the counter matches the value in this register,
	// the PWM signal will be driven high
	PWM1->_1_CMPA = (duty_cycle - 1);
}

void PWM1_1B_Update_Duty_Cycle(uint16_t duty_cycle)
{
	// Set the duty cycle by writing to the COMPA field (Bits 15 to 0)
	// in the PWM3CMPA register. When the counter matches the value in this register,
	// the PWM signal will be driven high
	PWM1->_1_CMPB = (duty_cycle - 1);
}

void Motor_Forward(uint16_t duty_cycle)
{
	PWM1->_1_CMPA = (duty_cycle - 1);
	PWM1->_1_CMPB = (duty_cycle - 1);
	GPIOA->DATA |= 0x14;
	GPIOA->DATA &= ~0x28;
}

void Motor_Backward(uint16_t duty_cycle)
{
	PWM1->_1_CMPA = (duty_cycle - 1);
	PWM1->_1_CMPB = (duty_cycle - 1);
	GPIOA->DATA |= 0x28;
	GPIOA->DATA &= ~0x14;
}

void Motor_Right_Turn(uint16_t duty_cycle)
{
	PWM1->_1_CMPA = (duty_cycle - 1);
	PWM1->_1_CMPB = (duty_cycle - 1);
	GPIOA->DATA |= 0x24;
	GPIOA->DATA &= ~0x18;
}

void Motor_Left_Turn(uint16_t duty_cycle)
{
	PWM1->_1_CMPA = (duty_cycle - 1);
	PWM1->_1_CMPB = (duty_cycle - 1);
	GPIOA->DATA |= 0x18;
	GPIOA->DATA &= ~0x24;
}

void Motor_Stop()
{
	PWM1->_1_CMPA = (0);
	PWM1->_1_CMPB = (0);
	GPIOA->DATA &= ~0x3C;
}