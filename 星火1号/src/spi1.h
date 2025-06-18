/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-04-26     廖钟涛       the first version
 */
#ifndef SRC_SPI1_H_
#define SRC_SPI1_H_

#define SPI_CS_PIN  GET_PIN(E, 11)

extern char tx_buf[400];

uint8_t spi1_init(void);
uint8_t spi1_write(uint8_t *buf, uint16_t len);
uint8_t spi1_read(uint8_t *buf, uint16_t len);

#endif /* SRC_SPI1_H_ */
