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
	  
//��ʾGPS��λ��Ϣ 
//void Gps_Msg_Show(void)
//{
// 	float tp;		   
//	tp=gpsx.longitude;	   
//	sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//�õ������ַ��� 	   
//	tp=gpsx.latitude;	   
//	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���	 
//	tp=gpsx.altitude;	   
// 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//�õ��߶��ַ���
//	tp=gpsx.speed;	   
// 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//�õ��ٶ��ַ���	 				    
//	if(gpsx.fixmode<=3)														//��λ״̬
//	{  
//		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	 
//	}	 	   
//	sprintf((char *)dtbuf,"Valid satellite:%02d",gpsx.posslnum);	 		//���ڶ�λ��������
//    
//	sprintf((char *)dtbuf,"Visible satellite:%02d",gpsx.svnum%100);	 		//�ɼ�������
//		 
//	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//��ʾUTC����
//	//printf("year2:%d\r\n",gpsx.utc.year);
//	//u3_printf(dtbuf);
//	    
//	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//��ʾUTCʱ��

//}
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
	u8 i=0;
	delay_ms(200);
	if(USART2_RX_STA&0x8000)    //AT���յ����ݲ�����
		{				
			USART2_RX_STA&=0x7FFF;
			i=0;
			while(USART2_RX_STA--)
			{
				USART_SendData(USART3, USART2_RX_BUF[i++]);         
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
			}
			//printf("*\n");
			u3_printf("\n");
			USART2_RX_STA=0;
		}
}


int main(void)
 {		
 	u8 i=0;
	 u8 key=0XFF;
	u8 upload=0;	
	 u16 rxlen;
	u16 lenx;
	 u16 gpsnumber=0;
	 //float tp;	
	 char temp[60];
	 char temp3[60];
	 char send1[10] = "AT\r\n";
	 char send2[10] = "AT+CSQ\r\n";
	 char send3[70] = "AT+MQTTCFG=\"39.106.166.6\",1883,\"mqttjs_id007\",60,\"admin\",\"123456\",1\r\n";
	 char send4[20] = "AT+MQTTCFG?\r\n";
	 char send5[30] = "AT+MQTTOPEN=1,1,0,0,0,\"\",\"\"\r\n";
	 char send6[20] = "AT+MQTTSTAT?\r\n";
	 char send7[50] = "AT+MQTTPUB=\"test_mqtt\",2,1,0,0,\"update message\"\r\n";
	 char send8[60] = "AT+MQTTPUB=\"test_mqtt\",2,1,0,0,\"update message222\"\r\n";
	 char send9[10] = "AT\r\n";
	 char send10[10] = "AT\r\n";
	
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    	 //��ʱ������ʼ��	  
	//uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	 uart_init(9600);
	 usart3_init(9600);
	 usart2_init(9600);
	 printf("1-u1\n");
	 u3_printf("1+u3\n");
	 //u2_printf("123-u2");
	 LED_Init(); 
	 //delay_ms(800);
	 LED0=0; 
	 u3_printf(send2);
	 

	 u2_printf(send1);
	 
	 delay_ms(800);
		AT_back();
		u3_printf(send1);//���ͽ��յ������ݵ�����1
	  u3_printf("over");//over����


	 u2_printf(send2);
		//delay_ms(800);
	 
	 delay_ms(800);
		AT_back();
		u3_printf(send2);//���ͽ��յ������ݵ�����1
		u3_printf("over");//over����
//	 u2_printf("AT+CSQ\r\n");
//	 u3_printf("AT+CSQ\r\n");
//	 delay_ms(100);
		

	 u2_printf(send3);
		delay_ms(500);
				AT_back();
	 u3_printf(send3);//���ͽ��յ������ݵ�����1
				u3_printf("over");//over����
		//delay_ms(200);

	 
		u3_printf("start");//start����;	
	 u2_printf(send4);
		delay_ms(500);
				AT_back();
	 u3_printf(send4);//���ͽ��յ������ݵ�����1
		u3_printf("over");//over����
		//delay_ms(200);
	 
		u3_printf("start");//start����
	 u2_printf(send5);
		delay_ms(500);	
				AT_back();
	 u3_printf(send5);//���ͽ��յ������ݵ�����1
		u3_printf("over");//over����
		//delay_ms(200);
	 
		u3_printf("start");//start����	
	 u2_printf(send6);
		delay_ms(500);
				AT_back();
	 u3_printf(send6);//���ͽ��յ������ݵ�����1
		u3_printf("over");//over����
		//delay_ms(200);
	 
		u3_printf("start");//start����
	 u2_printf(send7);
		delay_ms(500);
				AT_back();
	 u3_printf(send7);//���ͽ��յ������ݵ�����1
		u3_printf("over");//over����
		//delay_ms(200);

		u3_printf("start");//start����
	 u2_printf(send8);
		delay_ms(500);
				AT_back();
	 u3_printf(send8);//���ͽ��յ������ݵ�����1
		u3_printf("over");//over����
		//delay_ms(200);


//	 
  while(1)
	{
		delay_ms(1);
		
		
		if(USART_RX_STA&0x8000)		//���յ�һ��������
		{
			rxlen=USART_RX_STA&0x7FFF;	//�õ����ݳ���
			u3_printf("Start");
			
//			for(i=0;i<rxlen-1;i++)
//			{
//				USART3_TX_BUF[i]=USART_RX_BUF[i];	 
//				u3_printf("\r\nY1-%s\r",USART_RX_BUF[i]);				
//			}
 			USART_RX_STA=0;		   	//������һ�ν���
			USART3_TX_BUF[i]=0;			//�Զ���ӽ�����
			GPS_Analysis(&gpsx,(u8*)USART_RX_BUF);//�����ַ���
			gpsnumber++;
			if(gpsnumber%5==0)
			{
				Gps_Msg_Show();				//��ʾ��Ϣ	
			}
	
//			u3_printf("UTC Start");
//			u3_printf("\r\n%s\r\n",USART_RX_BUF);
			
			
 		}
		
		
			// u3_printf("1");
		//GPIO_SetBits(GPIOC, GPIO_Pin_13); 
		
//	  if(USART_RX_STA&0x8000)    //���յ����ݲ�����
//		{				
//			USART_RX_STA&=0x7FFF;
//			i=0;
//			if(USART_RX_BUF[3]=='R')
//			{
//				u8 temp1 = USART_RX_STA;
//				while(temp1--)
//				{
//					//USART_SendData(USART1, USART_RX_BUF[i++]);         
//					//while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
//					USART_SendData(USART3, USART_RX_BUF[i++]);         
//					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
//				}
//			}
//			else
//			{
//				u8 temp1 = USART_RX_STA;
//				while(temp1--)
//				{
//					i++; 
//				}
//			}
//			
//			printf("*\n");
//			USART_RX_STA=0;
//		}

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
		
		//delay_ms(3000);
		if((lenx%300)==0)LED0=!LED0; 	    				 
		lenx++;	
//		sprintf((char *)temp,"lenx:%03d",lenx);
//		u3_printf(temp);
	}	 
} 

