#include <stdio.h>

#include "./drivers/inc/VGA.h"			

void test_char(){
	int x, y;
	char c = 0;

	for(y = 0; y <= 59; y++){
		for(x = 0; x <= 79; x++){
			VGA_write_char_ASM(x, y, c++);		
		}
	}
}

void test_pixel(){
	int x, y;
	unsigned short colour = 0;
	
	for(y = 0; y <= 239; y++){
		for(x = 0; x <= 319; x++){
			VGA_draw_point_ASM(x, y, colour++);
		}
	}


}

int main () {
	
	//test_pixel();
	//test_char();
	VGA_clear_pixelbuff_ASM();

	//VGA_clear_charbuff_ASM();

	return 0;

}
