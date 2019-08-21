#include "stm32f4xx_conf.h"
 
static unsigned int LEDState = 0;
static void USART_SendString(USART_TypeDef* USARTx, char* s);
 
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        if((char)USART_ReceiveData(USART1) == '1')
            LEDState = 2;
 
        if((char)USART_ReceiveData(USART1) == '0')
            LEDState = 1;
    }
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}
 
int main(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStrutcure;
    NVIC_InitTypeDef    NVIC_InitStructure;
 
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_180MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_180MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
 
    USART_InitStrutcure.USART_BaudRate              = 115200;
    USART_InitStrutcure.USART_WordLength            = USART_WordLength_8b;
    USART_InitStrutcure.USART_Parity                = USART_Parity_No;
    USART_InitStrutcure.USART_StopBits              = USART_StopBits_1;
    USART_InitStrutcure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
    USART_InitStrutcure.USART_Mode                  = USART_Mode_Tx | USART_Mode_Rx;
 
    USART_Init(USART1, &USART_InitStrutcure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
 
    NVIC_InitStructure.NVIC_IRQChannel          = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd       = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPriority  = 0;
    NVIC_Init(&NVIC_InitStructure);
 
    while(1)
    {
        if(LEDState == 2) {
            GPIO_SetBits(GPIOC, GPIO_Pin_8);
            USART_SendString(USART1, "LED On\r\n");
            LEDState = 0;
        }
        if(LEDState == 1) {
            GPIO_ResetBits(GPIOC, GPIO_Pin_8);
            USART_SendString(USART1, "LED Off\r\n");
            LEDState = 0;
        }
    }
    return (0);
}
 
static void USART_SendString(USART_TypeDef* USARTx, char* s)
{
    while(*s)
    {
        while(!USART_GetFlagStatus(USARTx, USART_FLAG_TC));
        USART_SendData(USARTx, *s);
        s++;
    }
}
