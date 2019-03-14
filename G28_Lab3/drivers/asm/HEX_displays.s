	.text
	.equ HEX_BASE1, 0xFF200020
	.equ HEX_BASE2, 0xFF200030
	.global HEX_clear_ASM
	.global HEX_flood_ASM
	.global HEX_write_ASM

//Takes HEX to clear as input
HEX_clear_ASM:
	LDR R1, =HEX_BASE1			
	LDR R2, =HEX_BASE2 
	MOV R3, #-1					//R3 is loop counter for display
	MOV R4, #1					//R4 is the control register
	MOV R6, #6					//Loop counter for bit
	LDR R5, [R1]				//value of hex 0 to 3
	MOV R8, #0xFFFFFFFF
	MOV R9, #4
	PUSH {LR}	
	BL LOOP_CLEAR
	POP {LR}					//For the sake of clarity
	LDR R5, [R2]
	MOV R3, #-1
	MOV R9, #2
	PUSH {LR}
	BL LOOP_CLEAR
	POP {LR}
	BX LR	

LOOP_CLEAR:
	ADD R3, R3, #1
	CMP R3, R9
	BXEQ LR
	MOV R7, R3
	LSL R7, #3
	BIC R7, R9, R7
	AND R3, R4, R0
	CMP R3, #0
	LSL R4, #1
	BEQ LOOP_CLEAR
	PUSH {LR}
	BL BIT_CLEAR
	POP {LR}
	B LOOP_CLEAR

BIT_CLEAR:
	SUBS R6, R6, #1
	BXMI LR
	AND R8, R8, R7
	B BIT_CLEAR

//Takes HEX to flood as input

HEX_flood_ASM:
		//LDR R1, =HEX_3_0_BASE
		//LDR R2, =HEX_5_4_BASE
		//store max vals in every display
		//MOV R3, #63
		//STR R3,[R1]
		//BX LR
	   	LDR R1, =HEX_BASE1 //base address for display 3 - 0
	   	LDR R2, =HEX_BASE2 //base address for display 5 - 4
	   	MOV R3, #0 //r3 is ctr
	   	MOV R5, #1 //this will be the compare bit, starting at the 0th display

floop: 	AND R4, R0, R5 //AND the two operands
	   	CMP R4, #0	//if its non-zero go to clear
	   	BLGT flood
		LSL R5, #1 //shift left by 1
	   	ADD R3, R3, #1 //end of loop
       	CMP R3, #6 //this might be #5 (as before)
	   	BLT floop
	   	BX LR


flood: 	PUSH {LR}
	   	CMP R5, #8	//if greater than 8 branch to 5th4th disp
	   	BLGT flood_upr //branch to upper //never goes in here, r5 doesnt get bigger than 8?
	   	BLLE flood_lwr //branch to lower
	  	POP {LR}
	   	BX LR

flood_upr:
		MOV R6, #127 // 7 bits of 1s (looks like an 8)
		SUB R7, R3, #4 //poor mans modulus, reset "r3" to 0
		STRB R6, [R2, R7]
		BX LR 
		//B bck

flood_lwr:
		MOV R6, #63 //6 bits of 1s (looks like a zero)
		STRB R6, [R1, R3]
		BX LR 
		//B bck

//Takes HEX to write and value to write as input
HEX_write_ASM:

	.end
