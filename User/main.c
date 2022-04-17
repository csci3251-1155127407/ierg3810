#include <stm32f10x.h>
#include <stdbool.h>
#include <IERG3810_LED.h>
#include <IERG3810_KEY.h>
#include <IERG3810_buzzer.h>
#include <IERG3810_clock.h>
#include <IERG3810_USART.h>
#include <IERG3810_LCD.h>
#include <IERG3810_util.h>
#include <IERG3810_interrupt.h>
#include <IERG3810_timer.h>
#include <stdlib.h>

void TIM3_IRQHandler(void) {
	IERG3810_TIM_clear_interrupt(3);
}

void TIM4_IRQHandler(void) {
	IERG3810_TIM_clear_interrupt(4);
}

int main() {
	IERG3810_KEY_init();
	IERG3810_LED_init();
	IERG3810_buzzer_init();
	IERG3810_clock_init();
	IERG3810_USART_init();
	IERG3810_LCD_init();
	IERG3810_interrupt_init();
	IERG3810_TIM3_init(7199, 4199);
	IERG3810_TIM4_init(7199, 4199);
	IERG3810_SysTick_init(10);
	IERG3810_delay(100000);
	IERG3810_LCD_draw_rect(0, 0, H, V, 0x0000);
	srand(3810);
	
	IERG3810_LCD_draw_rect(0, 0, H, V, IERG3810_to_color(174, 240, 243));
	while(true) {
		char res = IERG3810_PS2_get();
		if (res == '4') {
			
		} else if (res == '8') {
			
		} else if (res == '6') {
			
		}
	}
}

