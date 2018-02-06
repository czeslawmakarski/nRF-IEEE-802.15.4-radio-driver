/* Copyright (c) 2018, Nordic Semiconductor ASA
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

/**
 * @file
 *   This file implements hooks for the 802.15.4 driver FSM.
 *
 * Hooks are used by optional driver features to modify way in which notifications are propagated
 * through the driver.
 *
 */

#include "nrf_drv_radio802154_fsm_hooks.h"

#include <stdbool.h>

#include "mac_features/nrf_drv_radio802154_ack_timeout.h"
#include "mac_features/nrf_drv_radio802154_csma_ca.h"
#include "nrf_drv_radio802154_config.h"
#include "nrf_drv_radio802154_types.h"


typedef bool (* abort_hook)(nrf_drv_radio802154_term_t term_lvl);
typedef void (* transmitted_hook)(const uint8_t * p_frame);
typedef bool (* tx_failed_hook)(const uint8_t * p_frame, nrf_drv_radio802154_tx_error_t error);
typedef bool (* tx_started_hook)(const uint8_t * p_frame);

/* Since some compilers do not allow empty initializers for arrays with unspecified bounds,
 * NULL pointer is appended to below arrays if the compiler used is not GCC. It is intentionally
 * unused, but it prevents the arrays from being empty. GCC manages to optimize empty arrays away,
 * so such a solution is unnecessary. */

static const abort_hook m_abort_hooks[] =
{
#if NRF_DRV_RADIO802154_CSMA_CA_ENABLED
    nrf_drv_radio802154_csma_ca_abort,
#endif

#if NRF_DRV_RADIO802154_ACK_TIMEOUT_ENABLED
    nrf_drv_radio802154_ack_timeout_abort,
#endif

#ifndef __GNUC__
    NULL,
#endif
};

static const transmitted_hook m_transmitted_hooks[] =
{
#if NRF_DRV_RADIO802154_ACK_TIMEOUT_ENABLED
    nrf_drv_radio802154_ack_timeout_transmitted_hook,
#endif

#ifndef __GNUC__
    NULL,
#endif
};

static const tx_failed_hook m_tx_failed_hooks[] =
{
#if NRF_DRV_RADIO802154_CSMA_CA_ENABLED
    nrf_drv_radio802154_csma_ca_tx_failed_hook,
#endif

#if NRF_DRV_RADIO802154_ACK_TIMEOUT_ENABLED
    nrf_drv_radio802154_ack_timeout_tx_failed_hook,
#endif

#ifndef __GNUC__
    NULL,
#endif
};

static const tx_started_hook m_tx_started_hooks[] =
{
#if NRF_DRV_RADIO802154_CSMA_CA_ENABLED
    nrf_drv_radio802154_csma_ca_tx_started_hook,
#endif

#if NRF_DRV_RADIO802154_ACK_TIMEOUT_ENABLED
    nrf_drv_radio802154_ack_timeout_tx_started_hook,
#endif

#ifndef __GNUC__
    NULL,
#endif
};

bool nrf_drv_radio802154_fsm_hooks_terminate(nrf_drv_radio802154_term_t term_lvl)
{
    bool result = true;

    for (uint32_t i = 0; i < sizeof(m_abort_hooks) / sizeof(m_abort_hooks[0]); i++)
    {
        if (m_abort_hooks[i] == NULL)
        {
            break;
        }

        result = m_abort_hooks[i](term_lvl);

        if (!result)
        {
            break;
        }
    }

    return result;
}

void nrf_drv_radio802154_fsm_hooks_transmitted(const uint8_t * p_frame)
{
    for (uint32_t i = 0; i < sizeof(m_transmitted_hooks) / sizeof(m_transmitted_hooks[0]); i++)
    {
        if (m_transmitted_hooks[i] == NULL)
        {
            break;
        }

        m_transmitted_hooks[i](p_frame);
    }
}

bool nrf_drv_radio802154_fsm_hooks_tx_failed(const uint8_t                * p_frame,
                                             nrf_drv_radio802154_tx_error_t error)
{
    bool result = true;

    for (uint32_t i = 0; i < sizeof(m_tx_failed_hooks) / sizeof(m_tx_failed_hooks[0]); i++)
    {
        if (m_tx_failed_hooks[i] == NULL)
        {
            break;
        }

        result = m_tx_failed_hooks[i](p_frame, error);

        if (!result)
        {
            break;
        }
    }

    return result;
}

bool nrf_drv_radio802154_fsm_hooks_tx_started(const uint8_t * p_frame)
{
    bool result = true;

    for (uint32_t i = 0; i < sizeof(m_tx_started_hooks) / sizeof(m_tx_started_hooks[0]); i++)
    {
        if (m_tx_started_hooks[i] == NULL)
        {
            break;
        }

        result = m_tx_started_hooks[i](p_frame);

        if (!result)
        {
            break;
        }
    }

    return result;
}