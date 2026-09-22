#include "stm32f10x.h"                  // Device header
static uint8_t beep_state = 0; // 保存BEEP状态（供BEEP_Get读取）
void BEEP_Init(void) 
	{ 
		GPIO_InitTypeDef GPIO_InitStructure; 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_SetBits(GPIOB, GPIO_Pin_0); // 初始熄灭
		}
		void BEEP_Set(uint8_t state)
			{ 
				beep_state = state; 
				if (state) GPIO_ResetBits(GPIOB, GPIO_Pin_0); // 高电平亮 
				else GPIO_SetBits(GPIOB, GPIO_Pin_0);
 }
			
 uint8_t BEEP_Get(void) 
{
return beep_state;
}
