#include <stdint.h>
#include <nuclei_sdk_hal.h>

int metal_tty_putc(int c)
{
    uart_write(SOC_DEBUG_UART, (uint8_t)c);
    return 0;
}

__WEAK void __libc_fini_array(void)
{

}

__WEAK void __libc_init_array(void)
{

}
