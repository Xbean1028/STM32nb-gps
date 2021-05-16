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
 实验0：ALIENTEK STM32F103开发板工程模板
 注意，这是手册中的调试章节使用的main文件
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					//串口1,发送缓存区
nmea_msg gpsx; 											//GPS信息
__align(4) u8 dtbuf[50];   								//打印缓存器
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode字符串 
	  
//显示GPS定位信息 
//void Gps_Msg_Show(void)
//{
// 	float tp;		   
//	tp=gpsx.longitude;	   
//	sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//得到经度字符串 	   
//	tp=gpsx.latitude;	   
//	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//得到纬度字符串	 
//	tp=gpsx.altitude;	   
// 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//得到高度字符串
//	tp=gpsx.speed;	   
// 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//得到速度字符串	 				    
//	if(gpsx.fixmode<=3)														//定位状态
//	{  
//		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	 
//	}	 	   
//	sprintf((char *)dtbuf,"Valid satellite:%02d",gpsx.posslnum);	 		//用于定位的卫星数
//    
//	sprintf((char *)dtbuf,"Visible satellite:%02d",gpsx.svnum%100);	 		//可见卫星数
//		 
//	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//显示UTC日期
//	//printf("year2:%d\r\n",gpsx.utc.year);
//	//u3_printf(dtbuf);
//	    
//	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//显示UTC时间

//}
//显示GPS定位信息 
void Gps_Msg_Show(void)
{
	float tp;
	char temp[50];
	 tp=gpsx.longitude;	   
				sprintf((char *)temp,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//得到经度字符串 	   
			u3_printf(temp);//发送接收到的数据到串口1
				tp=gpsx.latitude;	   
				sprintf((char *)temp,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//得到纬度字符串	 
			u3_printf(temp);//发送接收到的数据到串口1
				tp=gpsx.altitude;	   
				sprintf((char *)temp,"Altitude:%.1fm     ",tp/=10);	    			//得到高度字符串
				u3_printf(temp);//发送接收到的数据到串口1
				tp=gpsx.speed;	   
				sprintf((char *)temp,"Speed:%.3fkm/h     ",tp/=1000);		    		//得到速度字符串	 
				u3_printf(temp);//发送接收到的数据到串口1				
				if(gpsx.fixmode<=3)														//定位状态
				{  
					sprintf((char *)temp,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
					u3_printf(temp);//发送接收到的数据到串口1					
				}	 	   
				sprintf((char *)temp,"Valid satellite:%02d",gpsx.posslnum);	 		//用于定位的卫星数
				u3_printf(temp);//发送接收到的数据到串口1
					
				sprintf((char *)temp,"Visible satellite:%02d",gpsx.svnum%100);	 		//可见卫星数
				u3_printf(temp);//发送接收到的数据到串口1
					 
				sprintf((char *)temp,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//显示UTC日期
				u3_printf(temp);//发送接收到的数据到串口1

				sprintf((char *)temp,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//显示UTC时间
				u3_printf(temp);//发送接收到的数据到串口1
				
				u3_printf("Over\n");
}

void AT_back(void)
{
	u8 i=0;
	delay_ms(200);
	if(USART2_RX_STA&0x8000)    //AT接收到数据并返回
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
	
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init();	    	 //延时函数初始化	  
	//uart_init(115200);	 //串口初始化为115200
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
		u3_printf(send1);//发送接收到的数据到串口1
	  u3_printf("over");//over测试


	 u2_printf(send2);
		//delay_ms(800);
	 
	 delay_ms(800);
		AT_back();
		u3_printf(send2);//发送接收到的数据到串口1
		u3_printf("over");//over测试
//	 u2_printf("AT+CSQ\r\n");
//	 u3_printf("AT+CSQ\r\n");
//	 delay_ms(100);
		

	 u2_printf(send3);
		delay_ms(500);
				AT_back();
	 u3_printf(send3);//发送接收到的数据到串口1
				u3_printf("over");//over测试
		//delay_ms(200);

	 
		u3_printf("start");//start测试;	
	 u2_printf(send4);
		delay_ms(500);
				AT_back();
	 u3_printf(send4);//发送接收到的数据到串口1
		u3_printf("over");//over测试
		//delay_ms(200);
	 
		u3_printf("start");//start测试
	 u2_printf(send5);
		delay_ms(500);	
				AT_back();
	 u3_printf(send5);//发送接收到的数据到串口1
		u3_printf("over");//over测试
		//delay_ms(200);
	 
		u3_printf("start");//start测试	
	 u2_printf(send6);
		delay_ms(500);
				AT_back();
	 u3_printf(send6);//发送接收到的数据到串口1
		u3_printf("over");//over测试
		//delay_ms(200);
	 
		u3_printf("start");//start测试
	 u2_printf(send7);
		delay_ms(500);
				AT_back();
	 u3_printf(send7);//发送接收到的数据到串口1
		u3_printf("over");//over测试
		//delay_ms(200);

		u3_printf("start");//start测试
	 u2_printf(send8);
		delay_ms(500);
				AT_back();
	 u3_printf(send8);//发送接收到的数据到串口1
		u3_printf("over");//over测试
		//delay_ms(200);


//	 
  while(1)
	{
		delay_ms(1);
		
		
		if(USART_RX_STA&0x8000)		//接收到一次数据了
		{
			rxlen=USART_RX_STA&0x7FFF;	//得到数据长度
			u3_printf("Start");
			
//			for(i=0;i<rxlen-1;i++)
//			{
//				USART3_TX_BUF[i]=USART_RX_BUF[i];	 
//				u3_printf("\r\nY1-%s\r",USART_RX_BUF[i]);				
//			}
 			USART_RX_STA=0;		   	//启动下一次接收
			USART3_TX_BUF[i]=0;			//自动添加结束符
			GPS_Analysis(&gpsx,(u8*)USART_RX_BUF);//分析字符串
			gpsnumber++;
			if(gpsnumber%5==0)
			{
				Gps_Msg_Show();				//显示信息	
			}
	
//			u3_printf("UTC Start");
//			u3_printf("\r\n%s\r\n",USART_RX_BUF);
			
			
 		}
		
		
			// u3_printf("1");
		//GPIO_SetBits(GPIOC, GPIO_Pin_13); 
		
//	  if(USART_RX_STA&0x8000)    //接收到数据并返回
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

//		if(USART2_RX_STA&0x8000)    //AT接收到数据并返回
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
		
		if(USART3_RX_STA&0x8000)    //接收到数据并返回
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

