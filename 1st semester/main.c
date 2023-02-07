#include <stdio.h>
#include "NUC100Series.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable I2C0 module clock */
    CLK_EnableModuleClock(I2C0_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP = SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

    /* Set GPA multi-function pins for I2C0 SDA and SCL */
    SYS->GPA_MFP = SYS_GPA_MFP_PA8_I2C0_SDA | SYS_GPA_MFP_PA9_I2C0_SCL;
}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

void I2C0_Init(void)
{
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 150000);

    /* Get I2C0 Bus Clock */
    printf("I2C clock %d Hz\n", I2C_GetBusClockFreq(I2C0));
}

void I2C0_Close(void)
{
    /* Disable I2C0 and close I2C0 clock */
    I2C_Close(I2C0);
    CLK_DisableModuleClock(I2C0_MODULE);
}

uint8_t I2C_Read_Chip_ID()
{	
		uint8_t byteData;
		printf("Start of I2C Read Chip ID function\n");
		//send i2c start
		I2C_Trigger(I2C0,1, 0, 1, 0);	//set start
		I2C_WAIT_READY(I2C0);	//poll si flag
		
		//send to Write port
		I2C0->I2CDAT = 0x40; //0x20->0x40 address need to be <<1
		I2C_Trigger(I2C0,0, 0, 1, 0);	//clr si
		I2C_WAIT_READY(I2C0); //poll si flag

		//send INDEX
		I2C0->I2CDAT = 0x2E;
		I2C_Trigger(I2C0,0, 0, 1, 0);	//clr si
		I2C_WAIT_READY(I2C0);	//poll si flag

		
		//send i2c start
		I2C_Trigger(I2C0,1, 0, 1, 0);	//set start
		I2C_WAIT_READY(I2C0);	//poll si flag		

		
		//send to Read port
		I2C0->I2CDAT = (0x41);
		I2C_Trigger(I2C0,0, 0, 1, 0);	//clr si
		I2C_WAIT_READY(I2C0);	//poll si flag		
		
		
		//receive data
		I2C0->I2CDAT = 0xFF;
		I2C_Trigger(I2C0,0, 0, 1, 0);   //clr si	
		I2C_WAIT_READY(I2C0);	//poll si flag
		byteData = I2C0->I2CDAT;
		I2C_Trigger(I2C0,0, 1, 1, 0);   //clr si and set stop
		
		
		if(byteData != 0x40){
				printf("Error reading chip ID\n");
		}
		printf("chip ID : %X\n", byteData);
		printf("Success!! End of I2C Read Chip ID function\n");
		return byteData;
}
uint8_t I2C_Write(uint8_t reg_addr , uint8_t value)//INDEX:register address , value:what value we want to write
{	
		uint8_t byteData;
		printf("Start of I2C Write function\n");
	
		//send i2c start
		I2C_Trigger(I2C0,1, 0, 1, 0);	
		I2C_WAIT_READY(I2C0);	
		
		//send to Write port
		I2C0->I2CDAT = 0x40; //0x20->0x40 address need to be <<1
		I2C_Trigger(I2C0,0, 0, 1, 0);	
		I2C_WAIT_READY(I2C0); 
		
		//send INDEX
		I2C0->I2CDAT = reg_addr;
		I2C_Trigger(I2C0,0, 0, 1, 0);
		I2C_WAIT_READY(I2C0);	
		
		//send data to gport0
		I2C0->I2CDAT = value;	
		I2C_Trigger(I2C0,0, 0, 1, 0);    	
		I2C_WAIT_READY(I2C0);	
		
		//send data to gport1
		I2C0->I2CDAT = value;	
		I2C_Trigger(I2C0,0, 0, 1, 0);   	
		I2C_WAIT_READY(I2C0);

		//send data to gport2
		I2C0->I2CDAT = value;	
		I2C_Trigger(I2C0,0, 0, 1, 0);   
		I2C_WAIT_READY(I2C0);	
		
		//send data to gport3
		I2C0->I2CDAT = value;	
		I2C_Trigger(I2C0,0, 0, 1, 0);    	
		I2C_WAIT_READY(I2C0);	
		
		//send data to gport4
		I2C0->I2CDAT = value;	
		I2C_Trigger(I2C0,0, 0, 1, 0);   
		I2C_WAIT_READY(I2C0);	
		
		//send data to gport5
		I2C0->I2CDAT = value;	
		I2C_Trigger(I2C0,0, 0, 1, 0);    	
		I2C_WAIT_READY(I2C0);	
		
		//stop bit
		I2C_Trigger(I2C0,0, 1, 1, 0);   
		
		printf("End of I2C Write function\n");
}
int32_t main(void)
{
		uint8_t reg_addr , value;
	
    SYS_UnlockReg();

    SYS_Init();

    UART0_Init();

    SYS_LockReg();

    printf("\n");
    printf("+--------------------------------------------------------+\n");
    printf("| NUC100 I2C Driver Sample Code(Master) for access Slave |\n");
    printf("|                                                        |\n");
    printf("| I2C Master (I2C0) <---> I2C Slave(I2C0)                |\n");
    printf("+--------------------------------------------------------+\n");

    printf("Configure I2C0 as a master.\n");
    printf("The I/O connection for I2C0:\n");
    printf("I2C0_SDA(PA.8), I2C0_SCL(PA.9)\n");

    /* Init I2C0 */
    I2C0_Init();

    printf("\n");
    printf("Check I2C Slave(I2C0) is running first!\n");
    printf("Press any key to continue.\n");
    getchar();
    printf("\n");
		
		I2C_Read_Chip_ID();
    printf("Reading Chip ID test OK.\n");
		
		while(1){
		printf("input register address:");
		scanf("%d",&reg_addr);
		printf("\ninput value:");
		scanf("%d",&value);
		I2C_Write(reg_addr , value);
		}
			

    I2C0_Close();

    while(1);
}



