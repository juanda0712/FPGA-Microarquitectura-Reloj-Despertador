#include "sys/alt_stdio.h"
#include "system.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_pio_regs.h"
#include "io.h"
#include "alt_types.h"

// Definir el valor del temporizador (1 segundo)
//#define TIMER_0_LOAD_VALUE 50000000  // 50MHz / 1Hz = 50,000,000

// Variables globales para el tiempo
volatile alt_u8 hours = 0;
volatile alt_u8 minutes = 0;
volatile alt_u8 seconds = 0;

// Patrones de segmentos para los n�meros del 0 al 9 (7 bits)
static const alt_u8 SEGMENT_PATTERNS[10] = {
    0x00, // 0b00111111
	0x01, // 0b00000110
	0x02, // 0b01011011
    0x4F, // 0b01001111
    0x66, // 0b01100110
    0x6D, // 0b01101101
    0x7D, // 0b01111101
    0x07, // 0b00000111
    0x7F, // 0b01111111
    0x6F  // 0b01101111
};

// Convertir un valor a su patr�n de 7 segmentos
alt_u8 value_to_7_segment_pattern(alt_u8 value) {
    return SEGMENT_PATTERNS[value];
}

// Funci�n para inicializar el temporizador
void init_timer() {
    // Configurar el temporizador para que cuente hacia abajo desde el valor de carga
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, (alt_u16)(TIMER_0_LOAD_VALUE & 0xFFFF));
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, (alt_u16)((TIMER_0_LOAD_VALUE >> 16) & 0xFFFF));

    // Configurar el temporizador para que genere una interrupci�n peri�dica
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, ALTERA_AVALON_TIMER_CONTROL_CONT_MSK | ALTERA_AVALON_TIMER_CONTROL_START_MSK);
}

// Funci�n para actualizar el reloj
void update_clock() {
    // Actualizar los segundos
    seconds++;
    if (seconds >= 60) {
        seconds = 0;
        minutes++;
        if (minutes >= 60) {
            minutes = 0;
            hours++;
            if (hours >= 24) {
                hours = 0;
            }
        }
    }
}

// Funci�n para mostrar el tiempo en los displays de 7 segmentos
void display_time() {
    // Horas, minutos y segundos a mostrar
    alt_u8 hours_tens = hours / 10;
    alt_u8 hours_units = hours % 10;
    alt_u8 minutes_tens = minutes / 10;
    alt_u8 minutes_units = minutes % 10;
    alt_u8 seconds_tens = seconds / 10;
    alt_u8 seconds_units = seconds % 10;

    // Mostrar las horas
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_HOUR_BASE, (value_to_7_segment_pattern(hours_tens) << 7) | value_to_7_segment_pattern(hours_units));

    // Mostrar los minutos
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_MIN_BASE, (value_to_7_segment_pattern(minutes_tens) << 7) | value_to_7_segment_pattern(minutes_units));

    // Mostrar los segundos
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_SEC_BASE, (value_to_7_segment_pattern(seconds_tens) << 7) | value_to_7_segment_pattern(seconds_units));
}

// Funci�n principal
int main() {
    // Inicializar el temporizador
    init_timer();

    while (1) {
        // Leer el estado del temporizador
        alt_u32 timer_status = IORD_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE);

        // Verificar si el temporizador ha generado una interrupci�n
        if (timer_status & ALTERA_AVALON_TIMER_STATUS_TO_MSK) {
            // Actualizar el reloj
            update_clock();

            // Mostrar el tiempo en los displays
            display_time();

            // Reiniciar el temporizador
            IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, ALTERA_AVALON_TIMER_STATUS_TO_MSK);
        }
    }
    return 0;
}
