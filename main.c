/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "bsp.h"
#include "nrf_calendar.h"
#include "app_uart.h"

static bool run_time_updates = false;


void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


void uart_init()
{
    uint32_t err_code;
    const app_uart_comm_params_t comm_params =
      {
          RX_PIN_NUMBER,
          TX_PIN_NUMBER,
          RTS_PIN_NUMBER,
          CTS_PIN_NUMBER,
          APP_UART_FLOW_CONTROL_ENABLED,
          false,
          UART_BAUDRATE_BAUDRATE_Baud115200
      };

    APP_UART_FIFO_INIT(&comm_params,
                         16,
                         256,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOW,
                         err_code);
    APP_ERROR_CHECK(err_code);
}


int uart_get_parameter(char *query_message, int min_value, int max_value)
{
    uint8_t tmp_char, digit_index;
    int current_value;
    while(1)
    {
        current_value = 0;
        digit_index = 0;
        printf("%s: ", query_message);
        while(1)
        {
            while(app_uart_get(&tmp_char) != NRF_SUCCESS);
            if(tmp_char >= '0' && tmp_char <= '9' && digit_index < 9)
            {
                current_value = current_value * 10 + (tmp_char - '0');
                app_uart_put(tmp_char);
                digit_index++;
            }
            else if(tmp_char == 8 && digit_index > 0)
            {
                current_value /= 10;
                digit_index--;
                printf("\b \b");
            }
            else if(tmp_char == 13)
            {
                break;
            }
        }
        if(current_value >= min_value && current_value <= max_value) break;
        else
        {
            printf("\r\nInvalid value! \r\n");
            printf("Legal range is %i - %i\r\n", min_value, max_value);            
        }
    }
    printf("\r\n");
    return current_value;
}


void print_current_time()
{
    printf("Uncalibrated time:\t%s\r\n", nrf_cal_get_time_string(false));
    printf("Calibrated time:\t%s\r\n", nrf_cal_get_time_string(true));
}


void calendar_updated()
{
    if(run_time_updates)
    {
        print_current_time();
    }
}


int main(void)
{
    uint8_t uart_byte;
    uint32_t year, month, day, hour, minute, second;

    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);

    nrf_cal_init();
    nrf_cal_set_callback(calendar_updated, 4);

    uart_init();

    printf("\r\nCalendar demo\r\n\n");
    printf("s - Set time\r\n");
    printf("g - Get time\r\n");
    printf("r - Run continuous time updates\r\n\n");

    while (true)
    {
        if(app_uart_get(&uart_byte) == NRF_SUCCESS)
        {
            switch(uart_byte)
            {
                case 's':
                    run_time_updates = false;
                
                    year = (uint32_t)uart_get_parameter("Enter year", 1970, 2100);
                    month = (uint32_t)uart_get_parameter("Enter month", 0, 11);
                    day = (uint32_t)uart_get_parameter("Enter day", 1, 31);
                    hour = (uint32_t)uart_get_parameter("Enter hour", 0, 23);
                    minute = (uint32_t)uart_get_parameter("Enter minute", 0, 59);
                    second = (uint32_t)uart_get_parameter("Enter second", 0, 59);
                    
                    nrf_cal_set_time(year, month, day, hour, minute, second);
                    
                    printf("Time set: ");
                    printf("%s", nrf_cal_get_time_string(false));
                    printf("\r\n\n");
                    break;
                
                case 'g':
                    print_current_time();
                    break;
                
                case 'r':
                    run_time_updates = !run_time_updates;
                    printf(run_time_updates ? "Time updates on\r\n" : "Time updates off\r\n");
                    break;
            }
        }
    }
}
