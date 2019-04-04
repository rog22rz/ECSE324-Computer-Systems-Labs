#include "./drivers/inc/vga.h"
#include "./drivers/inc/ISRs.h"
#include "./drivers/inc/LEDs.h"
#include "./drivers/inc/audio.h"
#include "./drivers/inc/HPS_TIM.h"
#include "./drivers/inc/int_setup.h"
#include "./drivers/inc/wavetable.h"
#include "./drivers/inc/pushbuttons.h"
#include "./drivers/inc/ps2_keyboard.h"
#include "./drivers/inc/HEX_displays.h"
#include "./drivers/inc/slider_switches.h"


int amplitude = 1;
char previous;	
float frequency[8] = {130.813, 146.832, 164.814, 174.614, 195.998, 220.000, 246.942, 261.626};

int keyboard[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int signal(float freq, int time) {
	// calculate the floating value for the index
	float a = (freq*time);
	int b = (int) a/48000;
	float index = a-(b*48000);
	// extract the integer and remainding decimal
	int integer = (int) index;
	float remain = index - integer;

	// calculate the signal by multiplying by the sound amplitude
	float total = (1-remain)*sine[integer] + remain*sine[integer+1];
	return (int) (total*amplitude);

}

float calculateSignal(int time) {
	int i;
	float note = 0;
	// loop through all keys and check if key is pressed
	for(i = 0; i < 8; i++){
		if(keyboard[i] == 1){
			note += signal(frequency[i], time);
		}
	}

	note *= amplitude;
	return note;
}

// calculate the signal played from wavetable give a time and frequency


// display the wave onto the screen when keys are pressed
void displayWave(float freq) {
	// clear the display before drawing the next wave
	VGA_clear_pixelbuff_ASM();
	// initialize the colour and position
	short colour = 0xFFFFFF;
	int x, y;
	// 48000 is the total sine wave, divide this by the number of x pixels per full iteration, with a base frequency of 60 
	int seg = 48000/(320.00*60/freq);
	// initialize x position 
	int time_pos = 0;

	// iterate through the display to print out the wave
	for(x=0; x<320; x++) {
		// use a sine function in order to calculate the y pixel to draw the pt on 
		//sine[6000] = the wave for 1/4 of the cycle.    Amplitude = volume affects amplitude of wave. 
		y = (int) (((float)amplitude)*(float)sine[time_pos]*((float)10/(float)sine[6000])) + 120;

		// draw calculated point in white
		VGA_draw_point_ASM(x, y, colour);
		// Increment position based on the increment variable. 
		time_pos += seg; 	
		//Resets the sine wave to the beginning of the period. 
		if (time_pos > 48000) {
			time_pos -= 48000; 
		} 
	}


}

//Update the state of keyboard array
void update(char *input) {

	switch(*input) {
		case 0x1C:
			keyboard[0] = 0;
			if(previous != 0xF0) {
				keyboard[0] = 1;
			}
			break;
		case 0x1B:
			keyboard[1] = 0;
			if(previous != 0xF0) {
				keyboard[1] = 1;
			}
			break;
		case 0x23:
			keyboard[2] = 0;
			if(previous != 0xF0) {
				keyboard[2] = 1;
			}
			break;
		case 0x2B:
			keyboard[3] = 0;
			if(previous != 0xF0) {
				keyboard[3] = 1;
			}
			break;
		case 0x3B:
			keyboard[4] = 0;
			if(previous != 0xF0) {
				keyboard[4] = 1;
			}
			break;
		case 0x42:
			keyboard[5] = 0;
			if(previous != 0xF0) {
				keyboard[5] = 1;
			}
			break;
		case 0x4B:
			keyboard[6] = 0;
			if(previous != 0xF0) {
				keyboard[6] = 1;
			}
			break;
		case 0x4C:
			keyboard[7] = 0;
			if(previous != 0xF0) {
				keyboard[7] = 1;
			}
			break;
		case 0x79:
			if(previous != 0xF0) {
				amplitude++;
			}
			break;
		case 0x7B:
			if(previous != 0xF0) {
				amplitude--;
			}
			break;
		default:
		break;
	}
}

//Compute frequency
float getFreq(){
	float freq;
	int i;
	for(i = 0; i<8; i++) {
		freq = freq + keyboard[i]*frequency[i];
	}
	return freq;
}

int main() {
	//Turning on interupts for timer 0
	int_setup(1, (int[]){199});

	//Initializing timer 0
	HPS_TIM_config_t hps_tim;
	hps_tim.tim = TIM0;
	hps_tim.timeout = 20; //20 us seconds (1/48000)
	hps_tim.LD_en = hps_tim.INT_en = hps_tim.enable = 1;
	HPS_TIM_config_ASM(&hps_tim);

	// initialization of the variables
	float sumFreq; // freq of notes
	char *input;
	int clock = 0; // keeps track of clock time 
	float note;	// sum of the signal

	while(1) {
		// read the data from the keyboard
		if(read_ps2_data_ASM(input)) {
			// check if the previous frequency changed
			if(previous != *input) {
				// get the frequency for the note of the keyboard clicks
				update(input);
				sumFreq = getFreq();
				displayWave(sumFreq);

				// check when the stop code is present to know when key is released
				if(previous == 0xF0) {
					// allow the same key to be clicked simultaneously
					previous = 0;
				} else {
					// else put in the data clicked
					previous = *input;
				}
			}
		}
		note = calculateSignal(clock);

		// check interrupt happens 
		if(TIM0_FLAG) {
		// write the audio data to the signal
			audio_write_data_ASM(note, note);
			// increment clock value
			clock++;	
			// reset the clock when it reaches the e
			if(clock > (int) (48000/sumFreq)) {
				clock = 0;
			}
			// reset the interrupt flag back to 0
			TIM0_FLAG = 0;
		}
	}
	return 0;
}

