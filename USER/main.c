#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "usart3.h"
#include "usart2.h"
#include "led.h"
#include "gps.h"
#include "string.h"	
#include "stdio.h"
/************************************************
 ʵ��0��ALIENTEK STM32F103�����幤��ģ��
 ע�⣬�����ֲ��еĵ����½�ʹ�õ�main�ļ�
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					//����1,���ͻ�����
nmea_msg gpsx; 											//GPS��Ϣ
__align(4) u8 dtbuf[50];   								//��ӡ������
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode�ַ��� 
u8 atflag;


void Bean_Send_data(USART_TypeDef * USARTx,u8 *s)
{
	//USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//���ж�
	while(*s!='\0')
	{ 
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC )==RESET);	
		USART_SendData(USARTx,*s);
		s++;
	}
//	 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	 USART_Cmd(USART1, ENABLE);

}

void Bean_Send_data_char(USART_TypeDef * USARTx,char *s)
{
	//USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//���ж�
	while(*s!='\0')
	{ 
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC )==RESET);	
		USART_SendData(USARTx,*s);
		s++;
	}
//	 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	 USART_Cmd(USART1, ENABLE);

}
	  
//��ʾGPS��λ��Ϣ 
void Gps_Msg_Show(void)
{
	float tp;
	char temp[50];
	 tp=gpsx.longitude;	   
				sprintf((char *)temp,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//�õ������ַ��� 	   
			u3_printf(temp);//���ͽ��յ������ݵ�����1
				tp=gpsx.latitude;	   
				sprintf((char *)temp,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���	 
			u3_printf(temp);//���ͽ��յ������ݵ�����1
				tp=gpsx.altitude;	   
				sprintf((char *)temp,"Altitude:%.1fm     ",tp/=10);	    			//�õ��߶��ַ���
				u3_printf(temp);//���ͽ��յ������ݵ�����1
				tp=gpsx.speed;	   
				sprintf((char *)temp,"Speed:%.3fkm/h     ",tp/=1000);		    		//�õ��ٶ��ַ���	 
				u3_printf(temp);//���ͽ��յ������ݵ�����1				
				if(gpsx.fixmode<=3)														//��λ״̬
				{  
					sprintf((char *)temp,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
					u3_printf(temp);//���ͽ��յ������ݵ�����1					
				}	 	   
				sprintf((char *)temp,"Valid satellite:%02d",gpsx.posslnum);	 		//���ڶ�λ��������
				u3_printf(temp);//���ͽ��յ������ݵ�����1
					
				sprintf((char *)temp,"Visible satellite:%02d",gpsx.svnum%100);	 		//�ɼ�������
				u3_printf(temp);//���ͽ��յ������ݵ�����1
					 
				sprintf((char *)temp,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//��ʾUTC����
				u3_printf(temp);//���ͽ��յ������ݵ�����1

				sprintf((char *)temp,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//��ʾUTCʱ��
				u3_printf(temp);//���ͽ��յ������ݵ�����1
				
				u3_printf("Over\n");
}

void AT_back(void)
{
	u16 ii=0;
	u16 inrxlen;
	delay_ms(200);
	if(USART_RX_STA&0x8000)    //AT���յ����ݲ�����
		{				
			
			//inrxlen=USART_RX_STA&0x7FFF;	//�õ����ݳ���
			USART_RX_STA&=0x7FFF;
			ii=0;
			while(USART_RX_STA--)
			{
				//USART_RX_STA--;
				USART_SendData(USART3, USART_RX_BUF[ii++]);         
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
			}
			//printf("*\n");
			u3_printf("\n");
			USART_RX_STA=0;
		}
		USART_RX_STA=0;
		delay_ms(2);
}

void Gps_Msg_Send(void)
{
	//dev1+37.5318905+122.08075116666666+2021-05-08 12:32:44
	char devid[10] = "dev1";
	float wei,jing,time;
	char temp[100];
	char msgstring[100];
	char sendstring[150];
	 jing=gpsx.longitude;
	 wei=gpsx.latitude;  
	
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//���ж�
	AT_back();
	 sprintf((char *)temp,"%s+%.5f+%.5f",devid,wei/=100000,jing/=100000);	//�õ���γ���ַ��� 	   
			//u3_printf(temp);//���ͽ��յ������ݵ�����1
	 sprintf((char *)msgstring,"%s+%04d-%02d-%02d %02d:%02d:%02d",temp,gpsx.utc.year,gpsx.utc.month,gpsx.utc.date,(gpsx.utc.hour+8)%24,gpsx.utc.min,gpsx.utc.sec);	//�õ��ַ��� 	   
			//u3_printf(msgstring);//���ͽ��յ������ݵ�����1
	 
	 sprintf((char *)sendstring,"AT+MQTTPUB=\"test_mqtt\",2,1,0,0,\"%s\"\r\n",msgstring);	//�õ���γ���ַ��� 	
	  u3_printf(sendstring);//���ͽ��յ������ݵ�����1
	 //u2_printf(sendstring);
	 Bean_Send_data_char(USART1,sendstring);
	 //u3_printf("send message\n");
		delay_ms(200);
//	 u3_printf("send message back\n");
	 AT_back();
	 
//	 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//	 USART_Cmd(USART2, ENABLE);
//	 
}

void AT_init(void)
{
	u8 send1[10] = "AT\r\n";
	 u8 send2[10] = "AT+CSQ\r\n";
	 u8 send3[80] = "AT+MQTTCFG=\"39.106.166.6\",1883,\"mqttjs_id007\",60,\"admin\",\"123456\",1\r\n";
	 u8 send4[20] = "AT+MQTTCFG?\r\n";
	 u8 send5[30] = "AT+MQTTOPEN=1,1,0,0,0,\"\",\"\"\r\n";
	 u8 send6[20] = "AT+MQTTSTAT?\r\n";
	 u8 send7[60] = "AT+MQTTPUB=\"test_mqtt\",2,1,0,0,\"update message\"\r\n";
	 u8 send8[60] = "AT+MQTTPUB=\"test_mqtt\",2,1,0,0,\"update message222\"\r\n";
	
	LED0=0; 
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//���ж�
	//u2_printf(send1);
	Bean_Send_data(USART1,send1);
	 delay_ms(200);
		AT_back();
		//u3_printf(send1);//���ͽ��յ������ݵ�����1
	 Bean_Send_data(USART3,send1);
		//delay_ms(100);

	 //u2_printf(send2);
	Bean_Send_data(USART1,send2);
	 delay_ms(200);
		AT_back();
		//u3_printf(send2);//���ͽ��յ������ݵ�����1
		Bean_Send_data(USART3,send2);
		//delay_ms(100);

	 //u2_printf(send3);
	 Bean_Send_data(USART1,send3);
		delay_ms(300);
				AT_back();
	 //u3_printf(send3);//���ͽ��յ������ݵ�����1
	 Bean_Send_data(USART3,send3);
	 //delay_ms(100);
	 
	 //u2_printf(send4);
	 Bean_Send_data(USART1,send4);
		delay_ms(300);
				AT_back();
	 //u3_printf(send4);//���ͽ��յ������ݵ�����1
	 Bean_Send_data(USART3,send4);
	 //delay_ms(100);
	 
	 //u2_printf(send5);
	 Bean_Send_data(USART1,send5);
		delay_ms(300);	
				AT_back();
	 //u3_printf(send5);//���ͽ��յ������ݵ�����1
	 Bean_Send_data(USART3,send5);
	 //delay_ms(100);
	 
	 //u2_printf(send6);
	 Bean_Send_data(USART1,send6);
		delay_ms(300);
				AT_back();
	 //u3_printf(send6);//���ͽ��յ������ݵ�����1
	 Bean_Send_data(USART3,send6);
	 //delay_ms(100);

	 //u2_printf(send7);
	 Bean_Send_data(USART1,send7);
		delay_ms(300);
				AT_back();
	 //u3_printf(send7);//���ͽ��յ������ݵ�����1
	 Bean_Send_data(USART3,send7);
//	 delay_ms(200);
//	 AT_back();

	 LED0=1; 
////��������ж�
	 USART_Cmd(USART2, ENABLE);
	 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

int main(void)
 {		
 	u16 i=0;
	 u16 rxlen;
	 u16 lenx=0;
	 u16 gpsnumber=0;
	 //float tp;	
	 //atflag = 0;
	 u8  TEMP_BUF[USART_REC_LEN];
   char temp[100];	 
	 u8 newtest[20]="newAT_init_end\n";
	 
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    	 //��ʱ������ʼ��	  
	//uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	 uart_init(9600);
	 usart3_init(9600);
	 usart2_init(9600);
	 //printf("1-u1\n");
	 u3_printf("1-u3\n");
	 u2_printf("123-u2");
	 LED_Init(); 
	 delay_ms(1);
	 LED0=1; 
	 Bean_Send_data(USART3,newtest);
	 AT_back();
	 AT_init();
	 Bean_Send_data(USART3,"AT_init_end\n");
	 LED0=0; 


  while(1)
	{
		delay_ms(10);
		//USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//��AT�ж�
		if((lenx%50)==0)LED0=!LED0; 	    				 
		lenx++;	
		
		if(USART2_RX_STA&0x8000)		//���յ�һ��������
		{
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//���ж�
			
			//rxlen=USART2_RX_STA&0x7FFF;	//�õ����ݳ���
			Bean_Send_data(USART3,"Open\n");
			//u3_printf("Start");
			
			gpsnumber++;	
			
			if(gpsnumber%20==0)
			{
				
				Bean_Send_data(USART3,"Five\n");
				GPS_Analysis(&gpsx,(u8*)USART2_RX_BUF);//�����ַ���
				//Gps_Msg_Show();				//��ʾ��Ϣ	
				Gps_Msg_Send();
				
			}
			USART2_RX_STA=0;		   	//������һ�ν���
			Bean_Send_data(USART3,"Next\n");
			USART_Cmd(USART2, ENABLE);
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			//Bean_Send_data(USART3,"End\n");
  
		}
		
		if(USART3_RX_STA&0x8000)    //���յ����ݲ�����
		{				
			USART3_RX_STA&=0x7FFF;
			i=0;
			while(USART3_RX_STA--)
			{
				USART_SendData(USART3, USART3_RX_BUF[i++]);         
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
			}
			//printf("*\n");
			u3_printf("3*\n");
			USART3_RX_STA=0;
		}

		if(USART_RX_STA&0x8000)    //AT���յ����ݲ�����
		{				
			
			//rxlen=USART_RX_STA&0x7FFF;	//�õ����ݳ���
			USART_RX_STA&=0x7FFF;
			i=0;
			while(USART_RX_STA--)
			{
				//USART_RX_STA--;
				USART_SendData(USART3, USART_RX_BUF[i++]);         
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
			}
			//printf("*\n");
			u3_printf("\n");
			USART_RX_STA=0;
		}
		
//		if(USART2_RX_STA&0x8000)    //AT���յ����ݲ�����
//		{				
//			USART2_RX_STA&=0x7FFF;
//			i=0;
//			while(USART2_RX_STA--)
//			{
//				USART_SendData(USART2, USART2_RX_BUF[i++]);         
//				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
//			}
//			//printf("*\n");
//			u3_printf("2*\n");
//			USART2_RX_STA=0;
//		}
		
		
		//delay_ms(10);	
//		sprintf((char *)temp,"lenx:%03d",lenx);
//		u3_printf(temp);
	}	
} 
