/* Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
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
 * @brief Protocol interface for Power Amplifier (PA) and Low Noise Amplifier (LNA) module.
 *
 * This module enables toggling of GPIO pins before and after radio transmission and reception in
 * order to control a Power Amplifier and/or Low Noise Amplifier.
 *
 * PA and LNA device specific configuration parameters must first be provided to this module by the application.
 * The protocol must then provide PA and LNA protocol configuration parameters before the functionality can be used by the protocol.
 *
 * When the PA/LNA module is configured, the stack can call the provided enable functions before radio activity
 * to enable the PA or LNA timer configurations for the upcoming radio activity.
 * The default behavior is automatic deactivation of PA/LNA on the radio DISABLED event.
 * This can be disabled, so that a manual deactivation can be performed instead.
 */

#ifndef NRF_FEM_PROTOCOL_API_H__
#define NRF_FEM_PROTOCOL_API_H__

#include "nrf_fem_control_config.h"
#include "nrf_fem_protocol_legacy_api.h"

#include "nrf_ppi.h"
#include "nrf_timer.h"

typedef enum
{
    NRF_802154_FAL_PA  = 1 << 0,
    NRF_802154_FAL_LNA = 1 << 1,
    NRF_802154_FAL_ALL = NRF_802154_FAL_PA | NRF_802154_FAL_LNA
} nrf_fal_functionality_t;

/**
 * @brief PA and LNA activation event types.
 */
typedef enum
{
    NRF_802154_FAL_EVENT_TYPE_TIMER,
    NRF_802154_FAL_EVENT_TYPE_GENERIC,
    NRF_802154_FAL_EVENT_TYPE_PPI,
} nrf_802154_fal_event_type_t;

/**
 * @brief Frontend Abstraction Layer event.
 *
 * The event can be a Timer Compare event or a Any event.
 * Register value is only used for Timer Compare event and should contain timer value to set the compare relative to.
 */
typedef struct
{
    union
    {
        struct
        {
            NRF_TIMER_Type * p_timer_instance;     /* Pointer to a 1 us resolution timer instance. */
            uint32_t         counter_value;        /* Timer value when radio activity starts. */
            uint8_t          compare_channel_mask; /* Mask of compare channels which may be used by the FEM to schedule its own tasks. */
        } timer;
        struct
        {
            uint32_t register_address; /* Address of event register. */
        } generic;
        struct
        {
            uint8_t ch_id;                    /* Number of the PPI which was provided. */
        } ppi;
    }                           event;
    bool                        override_ppi; /* False to ignore PPI channel below and use the one set by application. True to use the PPI channel below. */
    uint8_t                     ppi_ch_id;    /* PPI channel to be used for this event. */
    nrf_802154_fal_event_type_t type;         /* Type of event source. */
} nrf_802154_fal_event_t;

#if ENABLE_FEM

/**
 * @brief Set up PA using the provided events for the upcoming radio transmission.
 *
 * Multiple configurations may be provided by repeating calls to this function (i.e you can set the activate and deactivate event in multiple calls,
 * and the configuration is preserved between calls).
 *
 * If a timer event is provided, i.e. NRF_802154_PA_LNA_EVENT_TYPE_TIMER, the PA will be configured to (de)activate at the application-configured time gap
 * before the timer instance reaches the given register_value. The time gap is set via @ref nrf_fem_interface_configure.
 *
 * If a radio event is provided, i.e. NRF_802154_PA_LNA_EVENT_TYPE_EVENT, the PA will be configured to (de)activate when some event occurs.
 *
 * The function sets up the PPIs and GPIOTE channel to activate PA for the upcoming radio transmission.
 * PA pin will be active until deactivated, either by configured deactivation event or using @ref nrf_802154_fal_deactivate_now.
 *
 * @param[in] p_activate_event   Pointer to the activation event structure.
 * @param[in] p_deactivate_event Pointer to the deactivation event structure.
 *
 * @pre To activate PA it is required that nrf_fem_interface_configure() has been called first.
 *
 * @note If a timer event is provided, the caller of this function is responsible for starting the timer and its shorts.
 *
 * @note If a timer event is provided, the caller of this function is responsible for stopping the timer no earlier than the provided compare channel expires.
 *
 * @retval   ::NRF_SUCCESS               PA activate setup is successful.
 * @retval   ::NRF_ERROR_FORBIDDEN       PA is currently disabled.
 * @retval   ::NRF_ERROR_INVALID_STATE   PA activate setup could not be performed due to invalid or missing configuration parameters
 *                                       in p_activate_event or/and p_deactivate_event.
 */
int32_t nrf_802154_fal_pa_configuration_set(const nrf_802154_fal_event_t * const p_activate_event,
                                            const nrf_802154_fal_event_t * const p_deactivate_event);

/**
 * @brief Functions clears up the configuration provided by the @ref nrf_802154_fal_pa_configuration_set function.
 *
 * @param[in] p_activate_event   Pointer to the activation event structure.
 * @param[in] p_deactivate_event Pointer to the deactivation event structure.
 *
 * @retval   ::NRF_SUCCESS               PA activate setup purge is successful.
 * @retval   ::NRF_ERROR_INVALID_STATE   PA activate setup purge could not be performed due to invalid or missing configuration parameters
 *                                       in p_activate_event or/and p_deactivate_event.
 */
int32_t nrf_802154_fal_pa_configuration_clear(const nrf_802154_fal_event_t * const p_activate_event,
                                              const nrf_802154_fal_event_t * const p_deactivate_event);

/**
 * @brief Set up LNA using the provided event for the upcoming radio reception.
 *
 * Multiple configurations may be provided by repeating calls to this function (i.e you can set the activate and deactivate event in multiple calls,
 * and the configuration is preserved between calls).
 *
 * If a timer event is provided, i.e. NRF_802154_PA_LNA_EVENT_TYPE_TIMER, the LNA will be configured to (de)activate at the application-configured time gap
 * before the timer instance reaches the given register_value. The time gap is set via @ref nrf_fem_interface_configure.
 *
 * If a radio event is provided, i.e. NRF_802154_PA_LNA_EVENT_TYPE_RADIO, the LNA will be configured to (de)activate when the radio event occurs.
 *
 * The function sets up the PPIs and GPIOTE channel to activate LNA for the upcoming radio reception.
 * LNA pin will be active until deactivated, either by configured deactivation event or using @ref pa_lna_deactivate_now.
 *
 * @param[in] p_activate_event   Pointer to the activation event structure.
 * @param[in] p_deactivate_event Pointer to the deactivation event structure.
 *
 * @pre To activate LNA it is required that nrf_fem_interface_configure() has been called first.
 *
 * @note If a timer event is provided, the caller of this function is responsible for starting the timer and its shorts.
 *
 * @note If a timer event is provided, the caller of this function is responsible for stopping the timer no earlier than the provided compare channel expires.
 *
 * @retval   ::NRF_SUCCESS               LNA activate setup is successful.
 * @retval   ::NRF_ERROR_FORBIDDEN       LNA is currently disabled.
 * @retval   ::NRF_ERROR_INVALID_STATE   LNA activate setup could not be performed due to invalid or missing configuration parameters
 *                                       in p_activate_event or/and p_deactivate_event.
 */
int32_t nrf_802154_fal_lna_configuration_set(const nrf_802154_fal_event_t * const p_activate_event,
                                             const nrf_802154_fal_event_t * const p_deactivate_event);

/**
 * @brief Functions clears up the configuration provided by the @ref nrf_802154_fal_lna_configuration_set function.
 *
 * @param[in] p_activate_event   Pointer to the activation event structure.
 * @param[in] p_deactivate_event Pointer to the deactivation event structure.
 *
 * @retval   ::NRF_SUCCESS               LNA activate setup purge is successful.
 * @retval   ::NRF_ERROR_INVALID_STATE   LNA activate setup purge could not be performed due to invalid or missing configuration parameters
 *                                       in p_activate_event or/and p_deactivate_event.
 */
int32_t nrf_802154_fal_lna_configuration_clear(
    const nrf_802154_fal_event_t * const p_activate_event,
    const nrf_802154_fal_event_t * const p_deactivate_event);

/**
 * @brief Deactivate PA/LNA now.
 * This function will immediately deactivate the PA/LNA pins.
 */
void nrf_802154_fal_deactivate_now(nrf_fal_functionality_t type);

/**
 * @brief Clean up the configured PA/LNA timer/radio instance, PPI and GPIOTE resources.
 * The function resets the hardware that is set up for PA/LNA activation. PA and LNA module control configuration parameters are not deleted.
 * The function is intended to be called after the radio disable signal.
 */
void nrf_802154_fal_cleanup(void);

/**
 * @brief Check if PA signaling is configured and enabled, and get the configured gain in dB.
 *
 * @param[out] p_gain The configured gain in dB if PA is configured and enabled.
                      If there is no PA present or the PA does not affect the signal gain returns 0 dB.
 *
 */
void nrf_802154_fal_pa_is_configured(int8_t * const p_gain);

#else // ENABLE_FEM

static inline int32_t nrf_802154_fal_pa_configuration_set(
    const nrf_802154_fal_event_t * const p_activate_event,
    const nrf_802154_fal_event_t * const p_deactivate_event)
{
    (void)p_activate_event;
    (void)p_deactivate_event;
    return NRF_ERROR_FORBIDDEN;
}

static inline int32_t nrf_802154_fal_pa_configuration_clear(
    const nrf_802154_fal_event_t * const p_activate_event,
    const nrf_802154_fal_event_t * const p_deactivate_event)
{
    (void)p_activate_event;
    (void)p_deactivate_event;
    return NRF_ERROR_FORBIDDEN;
}

static inline int32_t nrf_802154_fal_lna_configuration_set(
    const nrf_802154_fal_event_t * const p_activate_event,
    const nrf_802154_fal_event_t * const p_deactivate_event)
{
    (void)p_activate_event;
    (void)p_deactivate_event;
    return NRF_ERROR_FORBIDDEN;
}

static inline int32_t nrf_802154_fal_lna_configuration_clear(
    const nrf_802154_fal_event_t * const p_activate_event,
    const nrf_802154_fal_event_t * const p_deactivate_event)
{
    (void)p_activate_event;
    (void)p_deactivate_event;
    return NRF_ERROR_FORBIDDEN;
}

static inline void nrf_802154_fal_deactivate_now(nrf_fal_functionality_t type)
{
    (void)type;
}

static inline void nrf_802154_fal_cleanup(void)
{

}

static inline void nrf_802154_fal_pa_is_configured(int8_t * const p_gain)
{
    *p_gain = 0;
}

#endif // ENABLE_FEM

#endif // NRF_FEM_PROTOCOL_API_H__

/**
   @}
   @}
 */
