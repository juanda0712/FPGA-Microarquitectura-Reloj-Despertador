#include "system.h"
#include "sys/alt_stdio.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include <stdint.h>
#include "io.h"
#include "alt_types.h"
#include "unistd.h"

#define BUTTONS_BASE 0x3050
#define LEDS_BASE 0x30a0
#define SSEG_HOUR_TENS_BASE 0x3030
#define SSEG_HOUR_UNITS_BASE 0x3090
#define SSEG_MINS_TENS_BASE 0x3060
#define SSEG_MIN_UNITS_BASE 0x3080
#define SSEG_SEC_TENS_BASE 0x3040
#define SSEG_SEC_UNITS_BASE 0x3070
#define SWITCH_BASE 0x3020
#define TIMER_BASE 0x3000


// Variables para almacenar el tiempo y el estado del conteo
uint8_t horas = 0, minutos = 0, segundos = 0;
uint8_t horas_alarma = -1, minutos_alarma = -1, segundos_alarma = -1;
uint8_t counting = 0;
uint8_t alarma_activa = 0;
uint8_t segundos_alarma_activa = 0;

#define PWM_FREQUENCY 3480  // Frecuencia de 1 kHz
#define CLOCK_FREQUENCY 50000000  // Frecuencia del reloj (50 MHz)
#define PWM_PERIOD (CLOCK_FREQUENCY / PWM_FREQUENCY)  // Calcula el periodo del PWM

uint32_t pwm_period = CLOCK_FREQUENCY / PWM_FREQUENCY;
uint8_t buzzer_state = 0;

// Variables globales para controlar el estado del buzzer y el ciclo de PWM
static uint32_t buzzer_pwm_counter = 0;
static uint32_t buzzer_pwm_half_period = PWM_PERIOD / 2;
static uint32_t buzzer_pwm_cycles = 0;

// Mapa de segmentos para los dígitos 0-9
uint8_t num_to_seven_seg(int num) {
    static const uint8_t patterns[10] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
    };
    return ~patterns[num];
}

// Función para mostrar un dígito en los segmentos LED
void display_digit(uint32_t sseg_base, uint8_t digit) {
    if (digit < 10) {
        IOWR_ALTERA_AVALON_PIO_DATA(sseg_base, num_to_seven_seg(digit));
    }
}

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

uint32_t debounce_buttons() {
    uint32_t stable_state;
    uint32_t last_state = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);
    for (int i = 0; i < 10000; i++) {
        stable_state = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);
        if (stable_state == last_state) {
            return stable_state;
        }
        last_state = stable_state;
    }
    return stable_state;
}

uint32_t read_switch() {
    return IORD_ALTERA_AVALON_PIO_DATA(SWITCH_BASE);
}

// Inicializa el timer para manejar el reloj (1 interrupción por segundo)
void init_timer() {
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x00);
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, (CLOCK_FREQUENCY & 0xFFFF));
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_BASE, (CLOCK_FREQUENCY >> 16));
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x07);
}

// Espera una interrupción del timer
void wait_for_timer() {
    while (!(IORD_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE) & 0x01)) {
    }
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0x01);
}

void handle_pwm_buzzer(uint32_t prev_buttons_state) {
    // Configura el temporizador para que genere una interrupción cada medio periodo del PWM
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x00); // Detén el temporizador
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_BASE, buzzer_pwm_half_period & 0xFFFF);
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_BASE, (buzzer_pwm_half_period >> 16) & 0xFFFF);
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x07); // Activa el temporizador con interrupciones

    buzzer_pwm_cycles = (1000 * 1000) / (2 * buzzer_pwm_half_period); // Ciclos en 1 segundo

    // Espera el final de cada ciclo de PWM
    while (buzzer_pwm_cycles > 0) {
    	uint32_t buttons_state = debounce_buttons();
		uint32_t new_switch_state = read_switch();
		uint32_t new_leds_state = IORD_ALTERA_AVALON_PIO_DATA(LEDS_BASE);
        // Espera una interrupción del temporizador
        wait_for_timer();

        // Alterna el estado del buzzer
        buzzer_state = !buzzer_state;
        IOWR_ALTERA_AVALON_PIO_DATA(BUZZER_BASE, buzzer_state);

        buzzer_pwm_counter++;
        if (buzzer_pwm_counter >= buzzer_pwm_half_period) {
            buzzer_pwm_counter = 0;
            buzzer_pwm_cycles--;

            // Actualiza el tiempo del reloj en cada ciclo de PWM
            segundos++;
            segundos_alarma_activa++;
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

            // Actualiza las visualizaciones de horas, minutos y segundos
            update_hours();
            update_minutes();
            update_seconds();
        }
        if (segundos_alarma_activa == 30) {
			IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, 0x00);
			IOWR_ALTERA_AVALON_PIO_DATA(BUZZER_BASE, 0);
			segundos_alarma_activa =0;
			break;
		}

        if ((buttons_state != prev_buttons_state) && (new_switch_state & 0x01) && new_leds_state == 1023) {
			IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, 0x00);
			IOWR_ALTERA_AVALON_PIO_DATA(BUZZER_BASE, 0);
			break;
		}
    }
    // Detén el temporizador después de completar los ciclos de PWM
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_BASE, 0x00);
}


int main() {
    IOWR_ALTERA_AVALON_PIO_DIRECTION(LEDS_BASE, 0xFF);
    IOWR_ALTERA_AVALON_PIO_DIRECTION(BUTTONS_BASE, 0x00);
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SWITCH_BASE, 0x00);
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_HOUR_TENS_BASE, 0xFF);
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_HOUR_UNITS_BASE, 0xFF);
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_MINS_TENS_BASE, 0xFF);
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_MIN_UNITS_BASE, 0xFF);
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_SEC_TENS_BASE, 0xFF);
    IOWR_ALTERA_AVALON_PIO_DIRECTION(SSEG_SEC_UNITS_BASE, 0xFF);
    IOWR_ALTERA_AVALON_PIO_DIRECTION(BUZZER_BASE, 0x01);  // Salida para el buzzer

    init_timer();

    uint32_t prev_buttons_state = debounce_buttons();
    uint32_t switch_state = read_switch();

    update_hours();
    update_minutes();
    update_seconds();

    while (1) {
        uint32_t buttons_state = debounce_buttons();
        uint32_t new_switch_state = read_switch();
        uint32_t new_leds_state = IORD_ALTERA_AVALON_PIO_DATA(LEDS_BASE);

        if ((buttons_state != prev_buttons_state) && !(new_switch_state & 0x01)) {
            uint32_t changed_buttons = buttons_state ^ prev_buttons_state;
            prev_buttons_state = buttons_state;

            if ((changed_buttons & 0x01) && (buttons_state & 0x01)) {
                segundos = (segundos + 1) % 60;
                update_seconds();
            }

            if ((changed_buttons & 0x02) && (buttons_state & 0x02)) {
                minutos = (minutos + 1) % 60;
                update_minutes();
            }

            if ((changed_buttons & 0x04) && (buttons_state & 0x04)) {
                horas = (horas + 1) % 24;
                update_hours();
            }

            if ((changed_buttons & 0x08) && (buttons_state & 0x08)) {
                horas_alarma = horas;
                minutos_alarma = minutos;
                segundos_alarma = segundos;
            }
        }

        if ((buttons_state != prev_buttons_state) && (new_switch_state & 0x01) && new_leds_state == 1023) {
            IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, 0x00);
        }

        if ((new_switch_state & 0x01) && !counting) {
            counting = 1;
        }

        if (!(new_switch_state & 0x01) && counting) {
            counting = 0;
        }

        if (alarma_activa) {
            segundos_alarma_activa++;

            if (segundos_alarma_activa == 30) {
                IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, 0x00);
                alarma_activa = 0;
            }
        }

        if (counting) {
            wait_for_timer();
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

            update_hours();
            update_minutes();
            update_seconds();
        }

        if ((horas_alarma == horas) && (minutos_alarma == minutos) && (segundos_alarma == segundos) && (counting)) {
            alarma_activa = 1;
            IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, 1023);
        }

        if (alarma_activa) {
            handle_pwm_buzzer(prev_buttons_state);  // Manejo del PWM del buzzer en cada ciclo
            alarma_activa = 0;
            init_timer();
        }
    }

    return 0;
}
