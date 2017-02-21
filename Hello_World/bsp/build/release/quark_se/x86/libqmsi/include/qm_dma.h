/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __QM_DMA_H_
#define __QM_DMA_H_

#include "qm_common.h"
#include "qm_soc_regs.h"

/**
 * DMA Driver for Quark Microcontrollers.
 *
 * @defgroup groupDMA DMA
 * @{
 */

/**
 * DMA Handshake Polarity
 */
typedef enum {
	QM_DMA_HANDSHAKE_POLARITY_HIGH = 0x0, /**< Set HS polarity high. */
	QM_DMA_HANDSHAKE_POLARITY_LOW = 0x1   /**< Set HS polarity low. */
} qm_dma_handshake_polarity_t;

/**
 * DMA Burst Transfer Length
 */
typedef enum {
	QM_DMA_BURST_TRANS_LENGTH_1 = 0x0,  /**< Burst length 1 data item. */
	QM_DMA_BURST_TRANS_LENGTH_4 = 0x1,  /**< Burst length 4 data items. */
	QM_DMA_BURST_TRANS_LENGTH_8 = 0x2,  /**< Burst length 8 data items. */
	QM_DMA_BURST_TRANS_LENGTH_16 = 0x3, /**< Burst length 16 data items. */
	QM_DMA_BURST_TRANS_LENGTH_32 = 0x4, /**< Burst length 32 data items. */
	QM_DMA_BURST_TRANS_LENGTH_64 = 0x5, /**< Burst length 64 data items. */
	QM_DMA_BURST_TRANS_LENGTH_128 =
	    0x6,			    /**< Burst length 128 data items. */
	QM_DMA_BURST_TRANS_LENGTH_256 = 0x7 /**< Burst length 256 data items. */
} qm_dma_burst_length_t;

/**
 * DMA Transfer Width
 */
typedef enum {
	QM_DMA_TRANS_WIDTH_8 = 0x0,   /**< Transfer width of 8 bits. */
	QM_DMA_TRANS_WIDTH_16 = 0x1,  /**< Transfer width of 16 bits. */
	QM_DMA_TRANS_WIDTH_32 = 0x2,  /**< Transfer width of 32 bits. */
	QM_DMA_TRANS_WIDTH_64 = 0x3,  /**< Transfer width of 64 bits. */
	QM_DMA_TRANS_WIDTH_128 = 0x4, /**< Transfer width of 128 bits. */
	QM_DMA_TRANS_WIDTH_256 = 0x5  /**< Transfer width of 256 bits. */
} qm_dma_transfer_width_t;

/**
 * DMA channel direction.
 */
typedef enum {
	QM_DMA_MEMORY_TO_MEMORY = 0x0, /**< Memory to memory transfer. */
	QM_DMA_MEMORY_TO_PERIPHERAL =
	    0x1,			  /**< Memory to peripheral transfer. */
	QM_DMA_PERIPHERAL_TO_MEMORY = 0x2 /**< Peripheral to memory transfer. */
} qm_dma_channel_direction_t;

/**
 * DMA channel configuration structure
 */
typedef struct {
	/** DMA channel handshake interface ID */
	qm_dma_handshake_interface_t handshake_interface;

	/** DMA channel handshake polarity */
	qm_dma_handshake_polarity_t handshake_polarity;

	/** DMA channel direction */
	qm_dma_channel_direction_t channel_direction;

	/** DMA source transfer width */
	qm_dma_transfer_width_t source_transfer_width;

	/** DMA destination transfer width */
	qm_dma_transfer_width_t destination_transfer_width;

	/** DMA source burst length */
	qm_dma_burst_length_t source_burst_length;

	/** DMA destination burst length */
	qm_dma_burst_length_t destination_burst_length;

	/**
	 * Client callback for DMA transfer ISR
	 *
	 * @param[in] callback_context DMA client context.
	 * @param[in] len              Data length transferred.
	 * @param[in] error            Error code.
	 */
	void (*client_callback)(void *callback_context, uint32_t len,
				int error_code);

	/** DMA client context passed to the callbacks */
	void *callback_context;
} qm_dma_channel_config_t;

/**
 * DMA transfer configuration structure
 */
typedef struct {
	uint32_t block_size;      /**< DMA block size, Min = 1, Max = 4095. */
	uint32_t *source_address; /**< DMA source transfer address. */
	uint32_t *destination_address; /**< DMA destination transfer address. */

} qm_dma_transfer_t;

/**
 * Initialise the DMA controller.
 *
 * The DMA controller and channels are first disabled.
 * All DMA controller interrupts are masked
 * using the controllers interrupt masking registers. The system
 * DMA interrupts are then unmasked. Finally the DMA controller
 * is enabled. This function must only be called once as it
 * resets the DMA controller and interrupt masking.
 *
 * @param[in] dma DMA instance.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int qm_dma_init(const qm_dma_t dma);

/**
 * Setup a DMA channel configuration.
 *
 * Configures the channel source width, burst size, channel direction,
 * handshaking interface and registers the client callback and callback
 * context. qm_dma_init() must first be called before configuring
 * a channel. This function only needs to be called once unless
 * a channel is being repurposed.
 *
 * @param[in] dma            DMA instance.
 * @param[in] channel_id     The channel to start.
 * @param[in] channel_config The DMA channel configuration as
 *                           defined by the DMA client. This must not be NULL.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int qm_dma_channel_set_config(const qm_dma_t dma,
			      const qm_dma_channel_id_t channel_id,
			      qm_dma_channel_config_t *const channel_config);

/**
 * Setup a DMA channel transfer.
 *
 * Configure the source address,destination addresses and block size.
 * qm_dma_channel_set_config() must first be called before
 * configuring a transfer. qm_dma_transfer_set_config() must
 * be called before starting every transfer, even if the
 * addresses and block size remain unchanged.
 *
 * @param[in] dma             DMA instance.
 * @param[in] channel_id      The channel to start.
 * @param[in] transfer_config The transfer DMA configuration
 *                            as defined by the dma client.
 *                            This must not be NULL.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int qm_dma_transfer_set_config(const qm_dma_t dma,
			       const qm_dma_channel_id_t channel_id,
			       qm_dma_transfer_t *const transfer_config);

/**
 * Start a DMA transfer.
 *
 * qm_dma_transfer_set_config() mustfirst be called
 * before starting a transfer.
 *
 * @param[in] dma        DMA instance.
 * @param[in] channel_id The channel to start.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
*/
int qm_dma_transfer_start(const qm_dma_t dma,
			  const qm_dma_channel_id_t channel_id);

/**
 * Terminate a DMA transfer.
 *
 * This function is only called if a transfer needs to be terminated manually.
 * This may be require if an expected transfer complete callback
 * has not been received. Terminating the transfer will
 * trigger the transfer complete callback. The length
 * returned by the callback is the transfer length at the
 * time that the transfer was terminated.
 *
 * @param[in] dma        DMA instance.
 * @param[in] channel_id The channel to stop.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
*/
int qm_dma_transfer_terminate(const qm_dma_t dma,
			      const qm_dma_channel_id_t channel_id);

/**
 * Setup and start memory to memory transfer.
 *
 * This function will setup a memory to memory transfer by
 * calling qm_dma_transfer_setup() and will then start the
 * transfer by calling qm_dma_transfer_start(). This is
 * done for consistency across user applications.
 *
 * @param[in] dma             DMA instance.
 * @param[in] channel_id      The channel to start.
 * @param[in] transfer_config The transfer DMA configuration
 *                            as defined by the dma client.
 *                            This must not be NULL.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
*/
int qm_dma_transfer_mem_to_mem(const qm_dma_t dma,
			       const qm_dma_channel_id_t channel_id,
			       qm_dma_transfer_t *const transfer_config);

/**
 * @}
 */

#endif /* __QM_DMA_H_ */
