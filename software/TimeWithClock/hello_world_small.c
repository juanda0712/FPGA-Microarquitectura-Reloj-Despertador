#include "sys/alt_stdio.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include <stdint.h>
#include "io.h"
#include "alt_types.h"

// Variables globales para el tiempo
volatile int hours = 0;
volatile int minutes = 0;
volatile int seconds = 0;

uint8_t num_to_seven_seg(int num) {
    // Patrones invertidos para un display de �nodo com�n
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

    return ~patterns[num];  // Invertir los bits
}

// Funci�n para inicializar el temporizador
void init_timer() {
    // Configurar el temporizador para que cuente hacia abajo desde el valor de carga
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, (alt_u16)(TIMER_LOAD_VALUE & 0xFFFF));
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_BASE, (alt_u16)((TIMER_LOAD_VALUE >> 16) & 0xFFFF));

    // Configurar el temporizador para que genere una interrupci�n peri�dica
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, ALTERA_AVALON_TIMER_CONTROL_CONT_MSK | ALTERA_AVALON_TIMER_CONTROL_START_MSK);
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

// Ejemplo para manejar 14 bits en cada PIO
void display_time() {
    // Horas, minutos y segundos a mostrar
    int hours_tens = hours / 10;
    int hours_units = hours % 10;
    int minutes_tens = minutes / 10;
    int minutes_units = minutes % 10;
    int seconds_tens = seconds / 10;
    int seconds_units = seconds % 10;

    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_HOUR_TENS_BASE, num_to_seven_seg(hours_tens));
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_HOUR_UNITS_BASE, num_to_seven_seg(hours_units));
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_MINS_TENS_BASE, num_to_seven_seg(minutes_tens));
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_MIN_UNITS_BASE, num_to_seven_seg(minutes_units));
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_SEC_TENS_BASE, num_to_seven_seg(seconds_tens));
    IOWR_ALTERA_AVALON_PIO_DATA(SSEG_SEC_UNITS_BASE, num_to_seven_seg(seconds_units));
}

// Funci�n principal
int main() {
    // Inicializar el temporizador
    init_timer();

    while (1) {
        // Leer el estado del temporizador
        alt_u32 timer_status = IORD_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE);

        // Verificar si el temporizador ha generado una interrupci�n
        if (timer_status & ALTERA_AVALON_TIMER_STATUS_TO_MSK) {
            // Actualizar el reloj
            update_clock();

            // Mostrar el tiempo en los displays
            display_time();

            // Reiniciar el temporizador
            IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, ALTERA_AVALON_TIMER_STATUS_TO_MSK);
        }
    }
    return 0;
}
