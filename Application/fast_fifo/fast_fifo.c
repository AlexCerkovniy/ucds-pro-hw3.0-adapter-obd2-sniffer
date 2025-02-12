/* Includes ================================================================= */
#include "fast_fifo.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
#define UTIL_MIN(__a, __b) (((__a) < (__b)) ? (__a) : (__b))

/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions  ======================================================= */

/**
 * @brief Returns the number of bytes available in the ring buffer.
 */
static size_t ringbuf_get_used(fast_fifo_t * p_fifo)
{
  size_t r_position = p_fifo->read_pos;
  size_t w_position = p_fifo->write_pos;
  return (p_fifo->buf_size_mask & (w_position - r_position));
}

/**
 * @brief Puts single byte to the ring buffer.
 */
static void ringbuf_put(fast_fifo_t * p_fifo, uint8_t byte)
{
  p_fifo->p_buf[(p_fifo->write_pos & p_fifo->buf_size_mask)] = byte;
  p_fifo->write_pos++;
}

/**
 * @brief Fetches single byte from the ring buffer.
 */
static void
ringbuf_peek(fast_fifo_t * p_fifo, size_t index, uint8_t * p_byte)
{
  *p_byte = p_fifo->p_buf[(p_fifo->read_pos + index) & p_fifo->buf_size_mask];
}

/**
 * @brief Returns single byte from the ring buffer and then removes it.
 */
static void ringbuf_get(fast_fifo_t * p_fifo, uint8_t * p_byte)
{
  ringbuf_peek(p_fifo, 0, p_byte);
  p_fifo->read_pos++;
}

/* Shared functions ========================================================= */
size_t fast_fifo_get_available(fast_fifo_t * p_fifo)
{
  size_t available_count = 0;
  available_count = ringbuf_get_used(p_fifo);
  return available_count;
}

size_t fast_fifo_get_free(fast_fifo_t * p_fifo)
{
  size_t free_count = 0;
  free_count = p_fifo->buf_size_mask - ringbuf_get_used(p_fifo);
  return free_count;
}

fifo_error_t fast_fifo_deinit(fast_fifo_t * p_fifo)
{
  /* Deinitialize the critical section. */
  p_fifo->read_pos  = 0;
  p_fifo->write_pos = 0;
  return E_OK;
}

fifo_error_t
fast_fifo_init(fast_fifo_t * p_fifo, uint8_t * p_buf, size_t buf_size)
{

  /* Buffer size must be a power of two since we use the fast mask method for
   * calculating the indexes in circular buffer.
   */
  p_fifo->p_buf         = p_buf;
  p_fifo->buf_size_mask = buf_size - 1;
  p_fifo->read_pos      = 0;
  p_fifo->write_pos     = 0;

  return E_OK;
}

fifo_error_t fast_fifo_put(fast_fifo_t * p_fifo, uint8_t byte)
{
  fifo_error_t ret = E_OK;

  if (ringbuf_get_used(p_fifo) < p_fifo->buf_size_mask)
  {
    ringbuf_put(p_fifo, byte);
  }
  else
  {
	ret = E_NOMEM;
  }

  return ret;
}

fifo_error_t fast_fifo_get(fast_fifo_t * p_fifo, uint8_t * const p_byte)
{

  fifo_error_t ret = E_EMPTY;

    if (ringbuf_get_used(p_fifo) != 0)
    {
      /* Get byte, or remove it from the FIFO. */
      if (p_byte)
      {
        ringbuf_get(p_fifo, p_byte);
      }
      else
      {
        p_fifo->read_pos++;
      }
      ret = E_OK;
    }

  return ret;
}

fifo_error_t fast_fifo_peek(fast_fifo_t * p_fifo, size_t index, uint8_t * p_byte)
{
  fifo_error_t ret = E_EMPTY;

  if (ringbuf_get_used(p_fifo) > index)
  {
    ringbuf_peek(p_fifo, index, p_byte);
    ret = E_OK;
  }

  return ret;
}

fifo_error_t fast_fifo_reset(fast_fifo_t * p_fifo)
{
  p_fifo->read_pos = p_fifo->write_pos;

  return E_OK;
}

fifo_error_t
fast_fifo_read(fast_fifo_t * p_fifo, uint8_t * const p_dst, size_t * p_size)
{
  fifo_error_t ret = E_EMPTY;

    size_t must_copy = ringbuf_get_used(p_fifo);
    must_copy        = UTIL_MIN(must_copy, (*p_size));
    (*p_size)        = must_copy;

    if (must_copy > 0)
    {
      if (p_dst)
      {
        /* Copy the data to the buffer. */
        for (size_t i = 0; i < must_copy; ++i)
        {
          ringbuf_get(p_fifo, &p_dst[i]);
        }
      }
      else
      {
        /* Just skip the data. */
        p_fifo->read_pos += must_copy;
      }
      ret = E_OK;
    }

  return ret;
}

fifo_error_t fast_fifo_write(
      fast_fifo_t * p_fifo, const uint8_t * const p_src, size_t amount)
{

  fifo_error_t ret = E_NOMEM;

    size_t available = p_fifo->buf_size_mask - ringbuf_get_used(p_fifo);

    /* The requested size must be less than the available size. */
    if (amount <= available)
    {
      /* Copy data to the FIFO. */
      for (size_t i = 0; i < amount; ++i)
      {
        ringbuf_put(p_fifo, p_src[i]);
      }
      ret = E_OK;
    }

  return ret;
}
