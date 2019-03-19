	.text
	.equ CHAR_PIXEL_BASE, 0xC8000000
	.equ CHAR_CHAR_BASE, 0xC9000000
	.equ PIXEL_END, 0xC803BE7E 
	.equ CHAR_END, 0xC9001DCF
	.global VGA_write_char_ASM
	.global VGA_clear_pixelbuff_ASM
	.global VGA_clear_charbuff_ASM
	.global VGA_draw_point_ASM

//clear pixel buffer

VGA_clear_pixelbuff_ASM:
	LDR R1, =CHAR_PIXEL_BASE
	MOV R2, #0						//Use to clear
	LDR R3, =320					//x limit 
	LDR R4, =240					//y	limit
	MOV R5, #0						//x counter
	MOV R6, #0						//y counter
	
X_LOOP:
	CMP R5, R3
	BEQ Y_LOOP
	ADD R7, R5, R6, LSL #9			//R7 contains the offset
	LSL R7, #1						
	ADD R7, R7, R1					//Add offset to base adress
	STRH R5, [R1]					//Clear adress
	ADD R5, R5, #1
	B X_LOOP
	
Y_LOOP:
	CMP R6, R4
	BEQ END
	MOV R5, #0
	ADD R6, R6, #1
	B X_LOOP


//clear character buffer
VGA_clear_charbuff_ASM:
	LDR R3, =CHAR_CHAR_BASE
	LDR R5, =CHAR_END
	MOV R4, #0		//Move 0 into adress to clear
	MOV R6, #79		//Counter for x

ClearCharBuffLoopX:
	STRB R4, [R3]
	ADD R3, R3, #1			
	SUBS R6, R6, #1
	BGE ClearCharBuffLoopX
	B ClearCharBuffLoopY

ClearCharBuffLoopY:
	CMP R3, R5  //compare if we reach end point
	BEQ END     //if we are at endpoint, quit 
	MOV R8, #1
	SUB R3, R3, #80
	ADD R3, R3, R8, LSL #7
	MOV R6, #79		//Counter for x
	B ClearCharBuffLoopX

VGA_write_char_ASM:
	LDR R3, =CHAR_CHAR_BASE		//R3 contains char buff base adress
	CMP R0, #79					//Check if x value if ok
	BGT END
	CMP R0, #0
	BMI END
	CMP R1, #59					//Check if y value is ok
	BGT END
	CMP R1, #0
	BMI END
	
FIND_ADRESS:
	ADD R4, R0, R1, LSL #7		//Shift y and add to x
	ADD R5, R3, R4				//Add offset to base adress

STORE_CHAR:
	STRB R2, [R5]				//Store value in adress
	B END



VGA_draw_point_ASM:
	LDR R3, =CHAR_PIXEL_BASE		//R3 contains pixel buff base adress
	LDR R4, =319
	CMP R0, R4					//Check if x value if ok
	BGT END
	CMP R0, #0
	BMI END
	CMP R1, #239					//Check if y value is ok
	BGT END
	CMP R1, #0
	BMI END
	
FIND_PIXEL_ADRESS:
	ADD R4, R0, R1, LSL #9		//Shift y and add to x
	LSL R4, #1	
	ADD R5, R3, R4				//Add offset to base adress
	
STORE_PIXEL:
	STRH R2, [R5]
	B END

END:
	BX LR
