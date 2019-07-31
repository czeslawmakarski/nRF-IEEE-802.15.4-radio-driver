/* Copyright (c) 2017 - 2018, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this
 *      list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef NRF_FEM_CONTROL_CONFIG_H_
#define NRF_FEM_CONTROL_CONFIG_H_

#include "nrf_ppi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration parameters for pins that enable/disable Power Amplifier (PA) or Low Noise Amplifier (LNA).
 */
typedef struct
{
  bool                         enable;         /* Enable toggling for this pin. */
  bool                         active_high;    /* If true, pin will be active high. Otherwise, pin will be active low. */
  uint8_t                      gpio_pin;       /* The GPIO pin number for the pin. */
  uint8_t                      gpiote_ch_id;   /* GPIOTE channel to be used for toggling pins. */
} nrf_fem_gpiote_pin_config_t;


/**
 * @brief Configuration parameters for the PA/LNA interface.
 */
typedef struct
{
  struct
  {
    uint32_t                      pa_time_gap_us;    /* The time between activating the PA pin and radio transmission is started. */
    uint32_t                      lna_time_gap_us;   /* The time between activating the LNA pin and radio reception is started. */
    int8_t                        pa_gain_db;        /* Configurable PA gain, ignored if the amplifier is not supporting this. */
    int8_t                        lna_gain_db;       /* Configurable LNA gain, ignored if the amplifier is not supporting this. */
  } fem_config;

  nrf_fem_gpiote_pin_config_t pa_pin_config;         /* Power Amplifier pin configuration. */
  nrf_fem_gpiote_pin_config_t lna_pin_config;        /* Low Noise Amplifier pin configuration. */

  nrf_ppi_channel_t ppi_ch_id_set;                   /* PPI channel to be used for setting pins. */
  nrf_ppi_channel_t ppi_ch_id_clr;                   /* PPI channel to be used for clearing pins. */
} nrf_fem_interface_config_t;

/**
 * @section Timings.
 */

/** Time in microseconds when PA GPIO is activated before the radio is ready for transmission. */
#define NRF_FEM_PA_TIME_IN_ADVANCE          23

/** Time in microseconds when LNA GPIO is activated before the radio is ready for reception. */
#define NRF_FEM_LNA_TIME_IN_ADVANCE         5

/** Radio ramp-up time in TX mode, in microseconds. */
#define NRF_FEM_RADIO_TX_STARTUP_LATENCY_US 40

/** Radio ramp-up time in RX mode, in microseconds. */
#define NRF_FEM_RADIO_RX_STARTUP_LATENCY_US 40

#ifdef __cplusplus
}
#endif

#endif /* NRF_FEM_CONTROL_CONFIG_H_ */
