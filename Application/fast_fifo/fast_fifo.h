/** ========================================================================= *
 *
 * @brief Fast & tiny FIFO library.
 *
 *  ========================================================================= */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Macros =================================================================== */
/* Enums ==================================================================== */
typedef enum {
	E_OK = 0,
	E_EMPTY,
	E_NOMEM
} fifo_error_t;

/* Types ==================================================================== */

/**
 * @brief The Fast FIFO control block.
 *
 * This structure utilizes Fast FIFO circular buffer, which means that data that
 * is written to the buffer will wrap around to the beginning of the buffer
 * once the end of the buffer is reached.
 */
typedef struct
{
  uint8_t *       p_buf;         /**< Pointer to the buffer memory. */
  volatile size_t read_pos;      /**< The read position in the buffer. */
  volatile size_t write_pos;     /**< The write position in the buffer. */
  size_t          buf_size_mask; /**< Mask used to calculate the buffer size. */
} fast_fifo_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */

/**
 * @brief Function for initializing the FIFO.
 *
 * @note The FIFO buffer provided must be a power of two in size, it gives
 *       fast access to data over circular buffer.
 *
 * @note The size of buffer may not be utilized entirely due to design, that
 *       means that if you provided 256 bytes buffer, there will be only 255
 *       bytes available for IO, since we should maintain wr and rd indexes
 *       separately in case of max utilization.
 *
 * @param[out] p_fifo   FIFO object.
 * @param[in]  p_buf    FIFO buffer for storing data.
 * @param[in]  buf_size Size of the FIFO buffer provided.
 *
 * @retval     E_OK    If initialization was successful.
 * @retval     E_NULL  If a NULL pointer is provided as buffer.
 * @retval     E_INVAL If size of buffer provided is not a power of two.
 */
fifo_error_t
fast_fifo_init(fast_fifo_t * p_fifo, uint8_t * p_buf, size_t buf_size);

/**
 * @brief Deinitializes the FIFO.
 *
 * @param[in/out] p_fifo   FIFO object.
 *
 * @retval     E_OK      If deinitialization was successful.
 * @retval     E_NULL    If a NULL pointer is provided as buffer.
 */
fifo_error_t fast_fifo_deinit(fast_fifo_t * p_fifo);

/**
 * @brief Returns the amount of available bytes in the FIFO.
 *
 * @note Function may assert if input parameter is NULL.
 *
 * @param[in]  p_fifo   Pointer to the FIFO.
 *
 * @return  Number of bytes available in the FIFO.
 */
size_t fast_fifo_get_available(fast_fifo_t * p_fifo);

/**
 * @brief Returns the amount of free space in the FIFO.
 *
 * @note Function may assert if input parameter is NULL.
 *
 * @param[in]  p_fifo   Pointer to the FIFO.
 *
 * @return Number of bytes free in the FIFO.
 */
size_t fast_fifo_get_free(fast_fifo_t * p_fifo);

/**
 * @brief Puts the byte to the FIFO.
 *
 * @note Function may assert if input parameter is NULL.
 *
 * @param[in]  p_fifo   Pointer to the FIFO.
 * @param[in]  byte     Byte to add to the FIFO.
 *
 * @retval E_OK     If the byte has been successfully added to the FIFO.
 * @retval E_NOMEM  If the FIFO is full.
 */
fifo_error_t fast_fifo_put(fast_fifo_t * p_fifo, uint8_t byte);

/**
 * @brief Gets the next byte from the FIFO.
 *
 * @note Function may assert if p_fifo parameter is NULL.
 *
 * @param[in]  p_fifo   Pointer to the FIFO.
 * @param[out] p_byte   Pointer to the location where the byte will be stored.
 *                      If NULL, the byte will be eliminated from the FIFO.
 *
 * @retval E_OK     If the byte was returned.
 * @retval E_EMPTY  If there are no more bytes in the queue.
 */
fifo_error_t fast_fifo_get(fast_fifo_t * p_fifo, uint8_t * const p_byte);

/**
 * @brief Peeks the element at the given index if it exists.
 *
 * @note Function may assert if any of input parameters is NULL.
 *
 * @param[in]  p_fifo   Pointer to the FIFO.
 * @param[in]  index    Index of the element to peek. 0 is the first element.
 * @param[out] p_byte   Byte fetched from the FIFO.
 *
 * @retval E_OK     If an byte was returned.
 * @retval E_EMPTY  No more bytes, or the index was too large.
 */
fifo_error_t
fast_fifo_peek(fast_fifo_t * p_fifo, size_t index, uint8_t * p_byte);

/**
 * @brief Resets the data in the FIFO.
 *
 * @note Function may assert if input parameter is NULL.
 *
 * @param[in]  p_fifo   Pointer to the FIFO.
 *
 * @retval  E_OK  If the FIFO was successfully reset.
 */
fifo_error_t fast_fifo_reset(fast_fifo_t * p_fifo);

/**
 * @brief Reads requested amount of bytes from the FIFO.
 *
 * @note Function may assert if any of the parameters are NULL.
 *
 * @param[in]  p_fifo Pointer to the FIFO.
 * @param[out] p_dst  Memory pointer to store the bytes from the FIFO.
 *
 * @param[in/out] p_size   Pointer to the maximum number of bytes to read.
 *                         Will be overwritten with the actual number of
 *                         bytes read.
 *
 * @retval E_OK    Data is stored in the byte array, the number of bytes read
 *                 is stored in p_size.
 * @retval E_EMPTY If the FIFO is empty.
 */
fifo_error_t
fast_fifo_read(fast_fifo_t * p_fifo, uint8_t * const p_dst, size_t * p_size);

/**
 * @brief Writes requested amount of bytes to the FIFO.
 *
 * @note Function may assert if any of the parameters are NULL.
 *
 * @param[in]  p_fifo Pointer to the FIFO.
 * @param[out] p_src  Source pointer to copy the bytes to the FIFO.
 * @param[in] amount  The number of bytes to copy.
 *
 * @retval E_OK    The bytes are copied to the FIFO.
 * @retval E_NOMEM No available space in the FIFO.
 */
fifo_error_t fast_fifo_write(
      fast_fifo_t * p_fifo, const uint8_t * const p_src, size_t amount);

#ifdef __cplusplus
}
#endif

/** @} */
