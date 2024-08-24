#include "system.h"
#include "altera_avalon_pio_regs.h"
#include <stdint.h>

#define BUTTONS_BASE 0x3050 // Direcci�n base de los botones
#define LEDS_BASE 0x30a0 // Direcci�n base de los LEDs
#define SSEG_HOUR_TENS_BASE 0x3030 // Direcci�n base de los segmentos LED de horas (decenas)
#define SSEG_HOUR_UNITS_BASE 0x3090 // Direcci�n base de los segmentos LED de horas (unidades)
#define SSEG_MINS_TENS_BASE 0x3060 // Direcci�n base de los segmentos LED de minutos (decenas)
#define SSEG_MIN_UNITS_BASE 0x3080 // Direcci�n base de los segmentos LED de minutos (unidades)
#define SSEG_SEC_TENS_BASE 0x3040 // Direcci�n base de los segmentos LED de segundos (decenas)
#define SSEG_SEC_UNITS_BASE 0x3070 // Direcci�n base de los segmentos LED de segundos (unidades)
#define SWITCH_BASE 0x3020 // Direcci�n base del switch

// Variables para almacenar el tiempo y el estado del conteo
uint8_t horas = 0, minutos = 0, segundos = 0;
uint8_t counting = 0; // 0: No contando, 1: Contando

// Mapa de segmentos para los d�gitos 0-9
uint8_t num_to_seven_seg(int num) {
    static const uint8_t patterns[10] = {
        0x3F, // 0
        0x06, // 1
        0x5B, // 2
        0x4F, // 3
        0x66, // 4
        0x6D, // 5
        0x7D, // 6
        0x07, // 7
        0x7F, // 8
        0x6F  // 9
    };

    return ~patterns[num]; // Aseg�rate de invertir si los LEDs son comunes al c�todo
}

// Funci�n para mostrar un d�gito en los segmentos LED
void display_digit(uint32_t sseg_base, uint8_t digit) {
    if (digit < 10) {
        IOWR_ALTERA_AVALON_PIO_DATA(sseg_base, num_to_seven_seg(digit));
    }
}

// Funci�n para actualizar los segmentos LED correspondientes
void update_hours() {
    display_digit(SSEG_HOUR_TENS_BASE, horas / 10);
    display_digit(SSEG_HOUR_UNITS_BASE, horas % 10);
}

void update_minutes() {
    display_digit(SSEG_MINS_TENS_BASE, minutos / 10);
    display_digit(SSEG_MIN_UNITS_BASE, minutos % 10);
}

void update_seconds() {
    display_digit(SSEG_SEC_TENS_BASE, segundos / 10);
    display_digit(SSEG_SEC_UNITS_BASE, segundos % 10);
}

// Funci�n para agregar debouncing de botones
uint32_t debounce_buttons() {
    uint32_t stable_state;
    uint32_t last_state = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);
    for (int i = 0; i < 10000; i++) { // Peque�o retraso para debouncing
        stable_state = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);
        if (stable_state == last_state) {
            return stable_state;
        }
        last_state = stable_state;
    }
    return stable_state;
}

// Funci�n para leer el estado del switch
uint32_t read_switch() {
    return IORD_ALTERA_AVALON_PIO_DATA(SWITCH_BASE); // Lee el estado del switch
}

// Simulaci�n de un retraso basado en ciclos de CPU
void delay_ms(uint32_t ms) {
    // Aproxime la duraci�n en ciclos para retrasar
    volatile uint32_t count;
    for (count = 0; count < (ms * 1000); count++) {
        // NOP para crear el retraso
    }
}

int main() {
    // Inicializa los LEDs como salida y botones como entrada
    IOWR_ALTERA_AVALON_PIO_DIRECTION(LEDS_BASE, 0xFF); // LEDs como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(BUTTONS_BASE, 0x00); // Botones como entradas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SWITCH_BASE, 0x00); // Switch como entrada

    // Inicializa los segmentos LED como salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_HOUR_TENS_BASE, 0xFF); // Salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_HOUR_UNITS_BASE, 0xFF); // Salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_MINS_TENS_BASE, 0xFF); // Salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_MIN_UNITS_BASE, 0xFF); // Salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_SEC_TENS_BASE, 0xFF); // Salidas
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_SEC_UNITS_BASE, 0xFF); // Salidas

    uint32_t prev_buttons_state = debounce_buttons(); // Guarda el estado inicial de los botones
    uint32_t switch_state = read_switch(); // Guarda el estado inicial del switch

    // Actualiza los segmentos LED iniciales
    update_hours();
    update_minutes();
    update_seconds();

    while (1) {
        uint32_t buttons_state = debounce_buttons(); // Lee el estado de los botones con debouncing
        uint32_t new_switch_state = read_switch(); // Lee el estado actual del switch

        // Verifica si el estado del bot�n ha cambiado
        if (buttons_state != prev_buttons_state) {
            uint32_t changed_buttons = buttons_state ^ prev_buttons_state; // Detecta botones que cambiaron de estado
            prev_buttons_state = buttons_state; // Actualiza el estado previo

            // Bot�n 1: Incrementar horas
            if ((changed_buttons & 0x01) && (buttons_state & 0x01)) {
                horas = (horas + 1) % 24; // Mantener en el rango 0-23
                update_hours(); // Actualiza solo los segmentos de horas
            }

            // Bot�n 2: Incrementar minutos
            if ((changed_buttons & 0x02) && (buttons_state & 0x02)) {
                minutos = (minutos + 1) % 60; // Mantener en el rango 0-59
                update_minutes(); // Actualiza solo los segmentos de minutos
            }

            // Bot�n 3: Incrementar segundos
            if ((changed_buttons & 0x04) && (buttons_state & 0x04)) {
                segundos = (segundos + 1) % 60; // Mantener en el rango 0-59
                update_seconds(); // Actualiza solo los segmentos de segundos
            }
        }

        // Verifica el estado del switch para iniciar/parar el conteo
        if (new_switch_state != switch_state) {
            if (new_switch_state & 0x01) { // Verifica si el switch est� encendido
                counting = 1; // Inicia el conteo
            } else {
                counting = 0; // Detiene el conteo
            }
            switch_state = new_switch_state; // Actualiza el estado del switch
        }

        // Si se ha iniciado el conteo, actualizar los minutos y segundos
        if (counting) {
            delay_ms(1000); // Simula un retraso de 1 segundo
            segundos++;
            if (segundos >= 60) {
                segundos = 0;
                minutos++;
                if (minutos >= 60) {
                    minutos = 0;
                    horas++;
                    if (horas >= 24) {
                        horas = 0;
                    }
                }
            }

            // Actualiza los segmentos LED con el nuevo tiempo
                      update_hours();
                      update_minutes();
                      update_seconds();
                  }
              }

              return 0;
          }
