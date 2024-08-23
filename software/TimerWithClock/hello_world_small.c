
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include <stdint.h>

#define TIMER_BASE 0x3000 // Dirección base del temporizador
#define BUTTONS_BASE 0x3050 // Dirección base de los botones
#define LEDS_BASE 0x30a0 // Dirección base de los LEDs
#define SSEG_HOUR_TENS_BASE 0x3030 // Dirección base de los segmentos LED de horas (decenas)
#define SSEG_HOUR_UNITS_BASE 0x3090 // Dirección base de los segmentos LED de horas (unidades)
#define SSEG_MINS_TENS_BASE 0x3060// Dirección base de los segmentos LED de minutos (decenas)
#define SSEG_MIN_UNITS_BASE 0x3080// Dirección base de los segmentos LED de minutos (unidades)
#define SSEG_SEC_TENS_BASE 0x3040 // Dirección base de los segmentos LED de segundos (decenas)
#define SSEG_SEC_UNITS_BASE 0x3070 // Dirección base de los segmentos LED de segundos (unidades)

int main() {
    // Inicializa el temporizador
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x01); // Inicializa el temporizador en modo de conteo ascendente

    // Inicializa los LEDs
    IOWR_ALTERA_AVALON_PIO_DIRECTION(LEDS_BASE, 0xFF); // Configura los LEDs como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_HOUR_TENS_BASE, 0xFF); // Configura los segmentos LED de horas (decenas) como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_HOUR_UNITS_BASE, 0xFF); // Configura los segmentos LED de horas (unidades) como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_MINS_TENS_BASE, 0xFF); // Configura los segmentos LED de minutos (decenas) como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_MIN_UNITS_BASE, 0xFF); // Configura los segmentos LED de minutos (unidades) como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_SEC_TENS_BASE, 0xFF); // Configura los segmentos LED de segundos (decenas) como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_SEC_UNITS_BASE, 0xFF); // Configura los segmentos LED de segundos (unidades) como salidas

    int button_count = 0; // Contador para guardar el conteo de veces que se presiona el botón

        while (1) {
            // Lee el estado de los botones
            int buttons_state = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);
            printf("Estado de los botones: %x\n", buttons_state); // Imprime el estado de los botones en la consola

            // Verifica si se presionó un botón
            if (buttons_state != 0) {
                button_count++; // Incrementa el contador de veces que se presiona el botón

                // Actualiza el valor del temporizador
                uint32_t timer_value = IORD_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE);
                printf("Valor del temporizador: %u\n", timer_value); // Imprime el valor del temporizador en la consola

                // Escribe el valor del temporizador en los segmentos LED (por ejemplo, en formato HH:MM:SS)
                uint8_t horas = (timer_value / 3600) % 24;
                uint8_t minutos = (timer_value / 60) % 60;
                uint8_t segundos = timer_value % 60;

                printf("Horas: %u, Minutos: %u, Segundos: %u\n", horas, minutos, segundos); // Imprime los valores calculados en la consola

                IOWR_ALTERA_AVALON_PIO_DATA(SSEG_HOUR_TENS_BASE, horas / 10);
                IOWR_ALTERA_AVALON_PIO_DATA(SSEG_HOUR_UNITS_BASE, horas % 10);
                IOWR_ALTERA_AVALON_PIO_DATA(SSEG_MINS_TENS_BASE, minutos / 10);
                IOWR_ALTERA_AVALON_PIO_DATA(SSEG_MIN_UNITS_BASE, minutos % 10);
                IOWR_ALTERA_AVALON_PIO_DATA(SSEG_SEC_TENS_BASE, segundos / 10);
                IOWR_ALTERA_AVALON_PIO_DATA(SSEG_SEC_UNITS_BASE, segundos % 10);
            }
        }

        return 0;
    }
