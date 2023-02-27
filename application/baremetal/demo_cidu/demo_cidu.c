#include <stdio.h>
#include "nuclei_sdk_soc.h"
#include "nuclei_sdk_hal.h"

#if !defined(__CIDU_PRESENT) || (__CIDU_PRESENT != 1)
/* __CIDU_PRESENT should be defined in <Device>.h */
#error "__CIDU_PRESENT is not defined or equal to 1, please check!"
#endif

#if !defined(SMP_CPU_CNT)
#error "SMP_CPU_CNT macro is not defined, please set SMP_CPU_CNT to integer value > 1"
#endif

/* Comment it if you want broadcast mode */
#define ENABLE_FIRST_COME_FIRST_CLAIM_MODE

#define CORE_ID(n)                (n)
#define UART0_SEMAPHORE           1
/* Support up to 16 Cores in one cluster, applicable to SMP_CPU_CNT <= 16 */
#define BROADCAST_TO_ALL_CORES    0xFFFF
#define INTLEVEL                  1
#define INTPRIORITY               0

volatile uint32_t boothart_ready = 0;

int boot_hart_main(unsigned long hartid);
int other_harts_main(unsigned long hartid);
int main(void);

/* Reimplementation of smp_main for multi-harts */
int smp_main(void)
{
    return main();
}

void eclic_uart0_int_handler()
{
    int32_t status = -1;
    unsigned long hartid = __RV_CSR_READ(CSR_MHARTID);

    /* Protect the uart0, in case that other core access */
    CIDU_AcquireSemaphore_Block(UART0_SEMAPHORE, hartid);
    printf("Core %d enters uart0_receive_handler\n", hartid);
    /* Job finished, release the semaphore */
    CIDU_ReleaseSemaphore(UART0_SEMAPHORE);

#if defined(ENABLE_FIRST_COME_FIRST_CLAIM_MODE)
    status = CIDU_SetFirstClaimMode(ECLIC_IRQn_MAP_TO_SOC_EXTERNAL(UART0_IRQn), hartid);
    if (0 != status) {
        return;
    }
#endif

    /* Protect the uart0, in case that other core access */
    CIDU_AcquireSemaphore_Block(UART0_SEMAPHORE, hartid);

    printf("Core %d wants to process rx input\n", hartid);
    status = uart_get_status(SOC_DEBUG_UART);
    if (status & UART_IP_RXIP_MASK) {
        unsigned char c_get;
        // Clear rx pending
        uart_clear_status(SOC_DEBUG_UART, UART_IP_RXIP_MASK);
        c_get = getchar();
        printf("Core %d processed input:%c\n", hartid, c_get);
    }
    /* Job finished, release the semaphore */
    CIDU_ReleaseSemaphore(UART0_SEMAPHORE);

#if defined(ENABLE_FIRST_COME_FIRST_CLAIM_MODE)
    CIDU_ResetFirstClaimMode(ECLIC_IRQn_MAP_TO_SOC_EXTERNAL(UART0_IRQn));
#endif
}

void eclic_inter_core_int_handler()
{
    uint32_t sender_core_id = 0;
    unsigned long hartid = __RV_CSR_READ(CSR_MHARTID);

    /* Query sender's ID */
    sender_core_id = CIDU_GetCoreIntSenderId(hartid);
    /* Protect the uart0, in case that other core access */
    CIDU_AcquireSemaphore_Block(UART0_SEMAPHORE, hartid);
    printf("Core %d has received interrupt from core %d\n", hartid, sender_core_id);
    /* Job finished, release the semaphore */
    CIDU_ReleaseSemaphore(UART0_SEMAPHORE);
    /* Job finished, reset the core interrupt status */
    CIDU_ClearCoreIntStatus(sender_core_id, hartid);
}

int main(void)
{
    int ret;
    unsigned long hartid = __RV_CSR_READ(CSR_MHARTID);

    if (hartid == BOOT_HARTID) { // boot hart
        /* CIDU_SetBroadcastMode(ECLIC_IRQn_MAP_TO_SOC_EXTERNAL(UART0_IRQn), CORE_RECEIVE_INTERRUPT_ENABLE(0)
                                | CORE_RECEIVE_INTERRUPT_ENABLE(1)); */
        CIDU_SetBroadcastMode(ECLIC_IRQn_MAP_TO_SOC_EXTERNAL(UART0_IRQn), BROADCAST_TO_ALL_CORES);

        /* Register uart0 interrupt receive message handler */
        ECLIC_Register_IRQ(UART0_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                            ECLIC_LEVEL_TRIGGER, INTLEVEL, INTPRIORITY, eclic_uart0_int_handler);
        /* Register inter core interrupt handler */
        ECLIC_Register_IRQ(InterCore_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                            ECLIC_LEVEL_TRIGGER, INTLEVEL, INTPRIORITY, eclic_inter_core_int_handler);

        // Enable interrupts in general.
        __enable_irq();
        // Enable uart0 receive interrupt
        uart_enable_rxint(SOC_DEBUG_UART);

        boothart_ready = 1;
        ret = boot_hart_main(hartid);
    } else { // other harts
        while (boothart_ready == 0);

        ret = other_harts_main(hartid);
    }
    return ret;
}

int boot_hart_main(unsigned long hartid)
{
    /* Core 0(boot hart) send interrup to last core */
    CIDU_SetInterCoreIntShadow(BOOT_HARTID, SMP_CPU_CNT - 1);
    /* Besides Core 2, Core 0 sends interrupt to core 1 too, if SMP_CPU_CNT >2 */
    CIDU_SetInterCoreIntShadow(BOOT_HARTID, CORE_ID(1));

    while(1);
}

int other_harts_main(unsigned long hartid)
{
    /* Register uart0 interrupt receive message handler */
    ECLIC_Register_IRQ(UART0_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                        ECLIC_LEVEL_TRIGGER, INTLEVEL, INTPRIORITY, eclic_uart0_int_handler);
    /* Register inter core interrupt handler */
    ECLIC_Register_IRQ(InterCore_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                        ECLIC_LEVEL_TRIGGER, INTLEVEL, INTPRIORITY, eclic_inter_core_int_handler);

    // Enable interrupts in general.
    __enable_irq();

    /* core n send interrupt to core n-1 */
    CIDU_SetInterCoreIntShadow(hartid, hartid - 1);

    while(1);
}
