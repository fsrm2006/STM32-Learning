#ifndef __OLED_H
#define __OLED_H

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowDecimalNum(uint8_t Line, uint8_t Column, float Number, uint8_t Round);
void OLED_CleanPartial(uint8_t Line, uint8_t Column, uint8_t Length);
void OLED_ShowNums(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
uint8_t Get_NumLength(int32_t num);

#endif
