#include "stm32f10x.h"                  // Device header
static uint8_t led_state = 0; // 保存LED状态（供LED_Get读取）
void LED_Init(void) 
	{ 
		GPIO_InitTypeDef GPIO_InitStructure; 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOA, GPIO_Pin_8); // 初始熄灭
		}
		void LED_Set(uint8_t state)
			{ 
				led_state = state; 
				if (state) GPIO_SetBits(GPIOA, GPIO_Pin_8); // 高电平亮 
				else GPIO_ResetBits(GPIOA, GPIO_Pin_8);
 }
			
 uint8_t LED_Get(void) 
{
return led_state;
}
