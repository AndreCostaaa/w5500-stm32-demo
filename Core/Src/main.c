/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "main.h"
#include "stm32l4xx_hal_spi.h"
#include "stm32l4xx_hal_tim.h"
#include "stm32l4xx_hal_uart.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "dhcp.h"

#define DHCP
#define DHCP_BUFFER_SIZE 2048
#define TCP_BUFFER_SIZE	 2048
#define DHCP_SOCKET	 0
#define TCP_SOCKET	 (DHCP_SOCKET + 1)

SPI_HandleTypeDef w5500_spi;
UART_HandleTypeDef console_uart;
TIM_HandleTypeDef htim16;
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM16_Init(void);
#ifdef DHCP
static uint8_t dhcp_buffer[DHCP_BUFFER_SIZE];
#endif
static uint8_t tcp_buffer[TCP_BUFFER_SIZE];
static uint8_t mac_address[] = { 0x12, 0xde, 0xc8, 0xcb, 0x05, 0xa1 };
static uint8_t dst_ip[] = { 192, 168, 1, 246 };
static const uint16_t dst_port = 4000;

static wiz_NetTimeout wiznet_timeout = { .retry_cnt = 3, .time_100us = 2000 };

//Used by _write syscall (printf)
int __io_putchar(int byte)
{
	return HAL_UART_Transmit(&console_uart, (uint8_t *)&byte, 1, 100);
}

void crit_section_enter(void)
{
	__set_PRIMASK(1);
}

void crit_section_leave(void)
{
	__set_PRIMASK(0);
}

void spi_cs_select(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
}
void spi_cs_deselect(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}
void spi_read(uint8_t *buf, uint16_t size)
{
	HAL_StatusTypeDef ret = HAL_SPI_Receive(&w5500_spi, buf, size, 100);
	if (ret != HAL_OK) {
		printf("Failed to read\r\n");
		//Handle error
	}
}
void spi_write(uint8_t *buf, uint16_t size)
{
	HAL_StatusTypeDef ret = HAL_SPI_Transmit(&w5500_spi, buf, size, 100);
	if (ret != HAL_OK) {
		printf("Failed to read\r\n");
		//Handle error
	}
}
uint8_t spi_read_byte(void)
{
	uint8_t data;

	spi_read(&data, 1);
	return data;
}
void spi_write_byte(uint8_t data)
{
	spi_write(&data, 1);
}

void register_callbacks(void)
{
	//critical section
	reg_wizchip_cris_cbfunc(crit_section_enter, crit_section_leave);
	//chip select
	reg_wizchip_cs_cbfunc(spi_cs_select, spi_cs_deselect);
	//spi
	reg_wizchip_spi_cbfunc(spi_read_byte, spi_write_byte);
	//spi burst
	reg_wizchip_spiburst_cbfunc(spi_read, spi_write);
}

void get_chip_id(char *buffer)
{
	ctlwizchip(CW_GET_ID, (void *)buffer);
}
void get_network_info(wiz_NetInfo *info)
{
	ctlnetwork(CN_GET_NETINFO, info);
}
void get_timeout_info(wiz_NetTimeout *timeout)
{
	ctlnetwork(CN_GET_TIMEOUT, timeout);
}

void print_network_info(const char *chip_id, const wiz_NetInfo *info,
			const wiz_NetTimeout *timeout)
{
	printf("TIMEOUT: %d, %d\r\n", timeout->retry_cnt, timeout->time_100us);

	printf("\r\n=== %s NET CONF ===\r\n", chip_id);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", info->mac[0],
	       info->mac[1], info->mac[2], info->mac[3], info->mac[4],
	       info->mac[5]);
	printf("IP: %d.%d.%d.%d\r\n", info->ip[0], info->ip[1], info->ip[2],
	       info->ip[3]);
	printf("GATEWAY: %d.%d.%d.%d\r\n", info->gw[0], info->gw[1],
	       info->gw[2], info->gw[3]);
	printf("MASK: %d.%d.%d.%d\r\n", info->sn[0], info->sn[1], info->sn[2],
	       info->sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", info->dns[0], info->dns[1], info->dns[2],
	       info->dns[3]);
	printf("======================\r\n");
}
void network_init(wiz_NetInfo *info, wiz_NetTimeout *timeout)
{
	ctlnetwork(CN_SET_TIMEOUT, (void *)timeout);
	ctlnetwork(CN_SET_NETINFO, (void *)info);
}

void on_dhcp_ip_assign(void)
{
	wiz_NetInfo info;
	char chip_id[6];
	getIPfromDHCP(info.ip);
	getGWfromDHCP(info.gw);
	getSNfromDHCP(info.sn);
	getDNSfromDHCP(info.dns);
	info.dhcp = NETINFO_DHCP;

	network_init(&info, &wiznet_timeout);

	get_chip_id(chip_id);
	print_network_info(chip_id, &info, &wiznet_timeout);
	printf("DHCP LEASED TIME : %u Sec.\r\n", getDHCPLeasetime());
}
void on_dhcp_ip_renewed(void)
{
	printf("DHCP IP RENEWED\r\n");
	on_dhcp_ip_assign();
}
void on_dhcp_ip_conflict(void)
{
	printf("DHCP IP CONFLICT\r\n");
	Error_Handler();
}
void handle_dhcp(void)
{
	static uint8_t old_dhcp_ret = -1;

	uint8_t dhcp_ret = DHCP_run();
	switch (dhcp_ret) {
	case DHCP_FAILED: ///< Processing Fail
		if (dhcp_ret != old_dhcp_ret) {
			printf("DHCP FAILED\r\n");
		}
		break;
	case DHCP_RUNNING: ///< Processing DHCP protocol
		if (dhcp_ret != old_dhcp_ret) {
			printf("DHCP RUNNING\r\n");
		}
		break;
	case DHCP_IP_ASSIGN: ///< First Occupy IP from DHPC server      (if cbfunc == null, act as default default_ip_assign)
		if (dhcp_ret != old_dhcp_ret) {
			printf("DHCP IP ASSIGN\r\n");
		}
		break;
	case DHCP_IP_CHANGED: ///< Change IP address by new ip from DHCP (if cbfunc == null, act as default default_ip_update)
		if (dhcp_ret != old_dhcp_ret) {
			printf("DHCP IP CHANGED\r\n");
		}
		break;
	case DHCP_IP_LEASED: ///< Stand by
		if (dhcp_ret != old_dhcp_ret) {
			printf("DHCP IP LEASED\r\n");
		}
		break;
	case DHCP_STOPPED: ///< Stop processing DHCP protocol
		if (dhcp_ret != old_dhcp_ret) {
			printf("DHCP IP STOPPED\r\n");
		}
		break;
	default:
		break;
	}
	old_dhcp_ret = dhcp_ret;
}
void handle_tcp_connection(void)
{
	int32_t size, size_tx_sent = 0;
	static uint16_t current_port = 50000;
	switch (getSn_SR(TCP_SOCKET)) {
	case SOCK_ESTABLISHED:

		if (getSn_IR(TCP_SOCKET) & Sn_IR_CON) { // Interrupt flag
			setSn_IR(TCP_SOCKET, Sn_IR_CON); // clear interrupt flag
		}
		if ((size = getSn_RX_RSR(TCP_SOCKET)) <= 0) {
			break;
		}
		if (size > TCP_BUFFER_SIZE) {
			size = TCP_BUFFER_SIZE;
		}
		size = recv(TCP_SOCKET, tcp_buffer, size);
		if (size <= 0) {
			printf("Received failed %d\r\n", size);
			break;
		}
		for (int32_t i = 0; i < size; ++i) {
			tcp_buffer[i]++;
		}
		while (size_tx_sent < size) {
			size_tx_sent += send(TCP_SOCKET,
					     tcp_buffer + size_tx_sent, size);
			if (size_tx_sent < 0) {
				printf("Send Error %d. Closing socket\r\n",
				       size_tx_sent);
				close(TCP_SOCKET);
				return;
			}
		}
		break;
	case SOCK_CLOSE_WAIT:
		if (disconnect(TCP_SOCKET) != SOCK_OK) {
			break;
		}
		printf("Socket closed\r\n");
		break;
	case SOCK_INIT:
		printf("Connecting to %d.%d.%d.%d:%d\r\n", dst_ip[0], dst_ip[1],
		       dst_ip[2], dst_ip[3], dst_port);
		if (connect(TCP_SOCKET, dst_ip, dst_port) != SOCK_OK) {
			return;
		}
		printf("Connected !\r\n");
		break;
	case SOCK_CLOSED:
		close(TCP_SOCKET);
		if (socket(TCP_SOCKET, Sn_MR_TCP, current_port++, 0x00) !=
		    TCP_SOCKET) {
		}
		if (current_port == 0xffff) {
			current_port = 50000;
		}
		break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim16) {
		DHCP_time_handler();
	}
}
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 },
				  { 2, 2, 2, 2, 2, 2, 2, 2 } };
	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_SPI1_Init();
	MX_TIM16_Init();

	register_callbacks();
	if (ctlwizchip(0, (void *)memsize) == -1) {
		printf("WIZCHIP Initialized fail.\r\n");
		Error_Handler();
	}

#ifdef DHCP
	setSHAR(mac_address);
	reg_dhcp_cbfunc(on_dhcp_ip_assign, on_dhcp_ip_renewed,
			on_dhcp_ip_conflict);
	DHCP_init(DHCP_SOCKET, dhcp_buffer);
#else
	wiz_NetInfo info = { .gw = { 192, 168, 1, 1 },
			     .ip = { 192, 168, 1, 200 },
			     .sn = { 255, 255, 255, 0 },
			     .dhcp = NETINFO_STATIC };
	memcpy(info.mac, mac_address, sizeof(mac_address));
	wiz_NetTimeout timeout;
	char chip_id[6];
	network_init(&info, &wiznet_timeout);
	get_network_info(&info);
	get_timeout_info(&timeout);
	get_chip_id(chip_id);
	print_network_info(chip_id, &info, &timeout);
#endif
	printf("Hello World!\r\n");
	while (1) {
#ifdef DHCP
		handle_dhcp();
#endif
		handle_tcp_connection();
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
  */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) !=
	    HAL_OK) {
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
  */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
				      RCC_CLOCKTYPE_SYSCLK |
				      RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) !=
	    HAL_OK) {
		Error_Handler();
	}
}
/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{
	htim16.Instance = TIM16;
	htim16.Init.Prescaler = 8000 - 1;
	htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim16.Init.Period = 10000 - 1;
	htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim16.Init.RepetitionCounter = 0;
	htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim16) != HAL_OK) {
		Error_Handler();
	}
	HAL_TIM_Base_Start_IT(&htim16);
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{
	/* SPI1 parameter configuration*/
	w5500_spi.Instance = SPI1;
	w5500_spi.Init.Mode = SPI_MODE_MASTER;
	w5500_spi.Init.Direction = SPI_DIRECTION_2LINES;
	w5500_spi.Init.DataSize = SPI_DATASIZE_8BIT;
	w5500_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
	w5500_spi.Init.CLKPhase = SPI_PHASE_1EDGE;
	w5500_spi.Init.NSS = SPI_NSS_SOFT;
	w5500_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	w5500_spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	w5500_spi.Init.TIMode = SPI_TIMODE_DISABLE;
	w5500_spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	w5500_spi.Init.CRCPolynomial = 7;
	w5500_spi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	w5500_spi.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	if (HAL_SPI_Init(&w5500_spi) != HAL_OK) {
		Error_Handler();
	}
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
	console_uart.Instance = USART2;
	console_uart.Init.BaudRate = 115200;
	console_uart.Init.WordLength = UART_WORDLENGTH_8B;
	console_uart.Init.StopBits = UART_STOPBITS_1;
	console_uart.Init.Parity = UART_PARITY_NONE;
	console_uart.Init.Mode = UART_MODE_TX_RX;
	console_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	console_uart.Init.OverSampling = UART_OVERSAMPLING_16;
	console_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	console_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&console_uart) != HAL_OK) {
		Error_Handler();
	}
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : PB6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}
