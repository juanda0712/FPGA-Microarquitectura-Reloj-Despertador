#include "system.h"
#include "altera_avalon_pio_regs.h"

int main() {
    // Configura el temporizador para contar cada 500 ms
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, 49999999); // 500 ms
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x7); // Activa el temporizador

    while (1) {
        // Lee el estado de los botones
        int buttons_state = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);

        // Escribe el estado de los botones en los LEDs
        IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, buttons_state);

        // Muestra el valor del temporizador en los displays de siete segmentos
        int timer_value = IORD_ALTERA_AVALON_TIMER_SNAPL(TIMER_BASE);
        IOWR_ALTERA_AVALON_PIO_DATA(SSEG_SEC_UNITS_BASE, timer_value % 10);
        IOWR_ALTERA_AVALON_PIO_DATA(SSEG_SEC_TENS_BASE, (timer_value / 10) % 10);
    }
}
