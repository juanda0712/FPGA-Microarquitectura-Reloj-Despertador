#include "system.h"
#include "altera_avalon_pio_regs.h"
#include <stdint.h>

#define BUTTONS_BASE 0x3050 // Dirección base de los botones
#define LEDS_BASE 0x30a0 // Dirección base de los LEDs
#define SSEG_HOUR_TENS_BASE 0x3030 // Dirección base de los segmentos LED de horas (decenas)
#define SSEG_HOUR_UNITS_BASE 0x3090 // Dirección base de los segmentos LED de horas (unidades)
#define SSEG_MINS_TENS_BASE 0x3060 // Dirección base de los segmentos LED de minutos (decenas)
#define SSEG_MIN_UNITS_BASE 0x3080 // Dirección base de los segmentos LED de minutos (unidades)
#define SSEG_SEC_TENS_BASE 0x3040 // Dirección base de los segmentos LED de segundos (decenas)
#define SSEG_SEC_UNITS_BASE 0x3070 // Dirección base de los segmentos LED de segundos (unidades)

// Variables para almacenar el tiempo
uint8_t horas = 0, minutos = 0, segundos = 0;

// Función para actualizar los segmentos LED
void update_segments() {
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_HOUR_TENS_BASE, horas / 10);
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_HOUR_UNITS_BASE, horas % 10);
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_MINS_TENS_BASE, minutos / 10);
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_MIN_UNITS_BASE, minutos % 10);
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_SEC_TENS_BASE, segundos / 10);
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_SEC_UNITS_BASE, segundos % 10);
}

int main() {
    // Inicializa los LEDs
    IOWR_ALTERA_AVALON_PIO_DIRECTION(LEDS_BASE, 0xFF); // Configura los LEDs como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_HOUR_TENS_BASE, 0xFF); // Configura los segmentos LED de horas (decenas) como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_HOUR_UNITS_BASE, 0xFF); // Configura los segmentos LED de horas (unidades) como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_MINS_TENS_BASE, 0xFF); // Configura los segmentos LED de minutos (decenas) como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_MIN_UNITS_BASE, 0xFF); // Configura los segmentos LED de minutos (unidades) como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_SEC_TENS_BASE, 0xFF); // Configura los segmentos LED de segundos (decenas) como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_SEC_UNITS_BASE, 0xFF); // Configura los segmentos LED de segundos (unidades) como salidas

    uint32_t prev_buttons_state = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE); // Guarda el estado inicial de los botones

    // Configura el tiempo inicial en los segmentos LED
    update_segments();

    while (1) {
        // Lee el estado de los botones
        uint32_t buttons_state = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);

        // Verifica si el estado del botón ha cambiado
        if (buttons_state != prev_buttons_state) {
            // Actualiza el estado previo de los botones
            prev_buttons_state = buttons_state;

            // Verifica qué botón se ha presionado y actualiza solo el segmento correspondiente
            if (buttons_state & 0x01) { // Botón 1: Incrementa las horas
                horas++;
                if (horas >= 24) horas = 0; // Reinicia las horas si llega a 24
                update_segments(); // Actualiza los segmentos LED
            }
            if (buttons_state & 0x02) { // Botón 2: Incrementa los minutos
                minutos++;
                if (minutos >= 60) minutos = 0; // Reinicia los minutos si llega a 60
                update_segments(); // Actualiza los segmentos LED
            }
            if (buttons_state & 0x04) { // Botón 3: Incrementa los segundos
                segundos++;
                if (segundos >= 60) segundos = 0; // Reinicia los segundos si llega a 60
                update_segments(); // Actualiza los segmentos LED
            }
        }
    }

    return 0;
}


