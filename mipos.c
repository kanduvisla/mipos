#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <bcm2835.h>
#include <alsa/asoundlib.h>

#define LED_POWER RPI_GPIO_P1_11	// Power LED
#define LED_CLOCK RPI_GPIO_P1_12	// Midi Clock Indicator LED
#define CLOCK_DIV 12			// 12 clock ticks per 8th note
#define PULSE_LEN 15			// 15 ms pulse

static volatile sig_atomic_t keep_running = 1;
static volatile sig_atomic_t is_midi_connected = 0;

typedef enum play_state
{
	play_state_stopped,
	play_state_playing,
}
play_state;

static void sig_handler(int _)
{
	(void)_;
	keep_running = 0;
}

int main(int argc, char **argv)
{
	signal(SIGINT, sig_handler);

	// Argument sent is the midi port to listen to
	snd_rawmidi_t* midiin;
	const char* portname = argv[1];

	// Initialize BCM2835:
	bcm2835_init();
	bcm2835_gpio_fsel(LED_POWER, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LED_CLOCK, BCM2835_GPIO_FSEL_OUTP);

	play_state state = play_state_stopped;
	uint8_t ticks = 0;

	while (keep_running) {
		// check if midi device is available
		// If not, the power LED keeps blinking
		while(is_midi_connected == 0 && keep_running) {
			bcm2835_gpio_write(LED_POWER, LOW);
			bcm2835_delay(500);

			if(!snd_rawmidi_open(&midiin, NULL, portname, SND_RAWMIDI_SYNC) && !snd_rawmidi_read(midiin, NULL, 0)) {
				is_midi_connected = 1;
			}

			bcm2835_gpio_write(LED_POWER, HIGH);
			bcm2835_delay(500);
		}

		// MIDI Device is detected
		uint8_t status;
		snd_rawmidi_read(midiin, &status, 1);

		if(status & 0xF0)
		{
			switch(status)
			{
				case 0xFA: // start
				{
					ticks = 0; // reset clock
					state = play_state_playing;
					break;
				}
				case 0xFB: // continue
				{
					state = play_state_playing;
					break;
				}
				case 0xFC: // stop
				{
					state = play_state_stopped;
					break;
				}
				case 0xF8: // clock tick
				{
					if(ticks % CLOCK_DIV == 0)
					{
						ticks = 0; // reset clock
						if(state == play_state_playing)
						{
							bcm2835_gpio_write(LED_CLOCK, HIGH);
							bcm2835_delay(PULSE_LEN);
							bcm2835_gpio_write(LED_CLOCK, LOW);
						}
					}

					if(state == play_state_playing)
						ticks++;
					break;
				}
				default:
					break;
			}
		}
	}

	// Make sure LED's are turned off
	bcm2835_gpio_write(LED_POWER, LOW);
	bcm2835_gpio_write(LED_CLOCK, LOW);

	return 0;
}
