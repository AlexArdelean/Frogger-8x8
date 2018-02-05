/* Author : Alex
 * Lab Section:022
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 *
 * GccApplication2.c
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "timer.h"
#include <stdio.h>
#include "keypad.h"
#include "bit.h"
#include "io.h"
#include "io.c"

//--------End find GCD function ----------------------------------------------
//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	/*Tasks should have members that include: state, period,
	a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;
//--------End Task scheduler data structure-----------------------------------
//--------Shared Variables----------------------------------------------------
unsigned char blue = 1, red = 2, green = 3, turquoise = 4, purple = 5, redgreen = 6;
unsigned char cars = 0, pads = 2, logs = 1, left = 0, right = 1, token = 0;
unsigned char rbcsize = 16, reset = 0;
unsigned char frog[2] = {7, 3};
unsigned char movementlock = 0; //1 means no movement
unsigned char matrix[8][8] = {{0}};
unsigned char roadblocks[7][16] = {{0}}; 
unsigned char rowdirection[7] = {0};
unsigned char rowtype[7] = {0};
unsigned char gameover = 0, score = 0;

//--------End Shared Variables------------------------------------------------
//--------User defined FSMs---------------------------------------------------

void updatematrix(){
	for(unsigned int i = 1; i  < 7; i++){
		for(unsigned int j = 0; j  < 8; j++){
			matrix[i][j] = roadblocks[i][j];
		}
	}
}
void displaymatrix(){
	static 	unsigned char bluedata = 0, reddata = 0, greendata = 0, coldata = 0, previous;	
	static int r = 0, c = 0;
	
	PORTD = score;
	
	if(matrix[frog[0]][frog[1]] == redgreen){
		matrix[frog[0]][frog[1]] = 0;
		roadblocks[frog[0]][frog[1]] = 0;
		token = 0;
	}
	previous = matrix[frog[0]][frog[1]]; //previous is used so the frog doesn't
	matrix[frog[0]][frog[1]] = green;	 //overwrite the previous block
	reddata = 0, greendata = 0;
	coldata = 0; bluedata = 0;
	transmit_data_Col(coldata);
	transmit_data_Green(greendata);
	transmit_data_Red(reddata);
	transmit_data_Blue(bluedata);
	if (c == 8)
		c = 0;
	for (r = 0; r <= 7; r++){
		if (matrix[r][c] == red)
			reddata += 0x80>>r;
		else if (matrix[r][c] == green)
			greendata += 0x80>>r;
		else if (matrix[r][c] == blue)
			bluedata += 0x80>>r;
		else if (matrix[r][c] == purple){
			bluedata += 0x80>>r;
			reddata += 0x80>>r;
		}
		else if (matrix[r][c] == turquoise){
			bluedata += 0x80>>r;
			greendata += 0x80>>r;
		}
		else if (matrix[r][c] == redgreen){
			reddata += 0x80>>r;
			greendata += 0x80>>r;
		}
	}
	coldata += 0x80>>c;
	c++;
	transmit_data_Col(coldata);
	transmit_data_Green(greendata);
	transmit_data_Red(reddata);
	transmit_data_Blue(bluedata);
	matrix[frog[0]][frog[1]] = previous;
}
void displaystartscreen(){
	static 	unsigned char bluedata = 0, reddata = 0, greendata = 0, coldata = 0;
	static int r = 0, c = 0;
	
	reddata = 0, greendata = 0;
	coldata = 0; bluedata = 0;
	transmit_data_Col(coldata);
	transmit_data_Green(greendata);
	transmit_data_Red(reddata);
	transmit_data_Blue(bluedata);
	if (c == 8)
	c = 0;
	for (r = 0; r <= 7; r++){
		if (matrix[r][c] == red)
		reddata += 0x80>>r;
		else if (matrix[r][c] == green)
		greendata += 0x80>>r;
		else if (matrix[r][c] == blue)
		bluedata += 0x80>>r;
		else if (matrix[r][c] == purple){
			bluedata += 0x80>>r;
			reddata += 0x80>>r;
		}
		else if (matrix[r][c] == turquoise){
			bluedata += 0x80>>r;
			greendata += 0x80>>r;
		}
		else if (matrix[r][c] == redgreen){
			reddata += 0x80>>r;
			greendata += 0x80>>r;
		}
	}
	coldata += 0x80>>c;
	c++;
	transmit_data_Col(coldata);
	transmit_data_Green(greendata);
	transmit_data_Red(reddata);
	transmit_data_Blue(bluedata);
}
void initroadblocks(){
	unsigned char blocktype, size;
	unsigned char logcolor = 0, watercolor = blue;
	unsigned char carcolor = red, padcolor = 0;
	
	for(unsigned int i = 0; i < 7;i++){
		for(unsigned int j = 0; j < 16; j++){
			roadblocks[i][j] = 0;
		}
	}
	for(unsigned int i = 0; i < 7; i++){
		blocktype = rand()%2;
		
		/////////////////////////////////////////////
		//lilis cant be next to each other and only 4 max can happen
		if(i != 1 && rand()%3 == pads && rowdirection[i-1] != pads){
			rowdirection[i] = pads;
			rowtype[i] = pads;
			unsigned char numwater = 0, numpads = 0;
			for(unsigned int j = 0; j < 16; j++){
				if((rand()%2 == 0 && numwater < 4) || numpads >= 4){
					roadblocks[i][j] = watercolor;
					numwater++;
				}
				else{
					roadblocks[i][j] = padcolor;
					if (j != 15)
						roadblocks[i][++j] = watercolor;
					numpads++;
				}
			}
		}
		////////////////////////////////////////////////
		else if (blocktype == cars){
			rowdirection[i] = rand()%2; //left or right
			rowtype[i] = cars;
			for(unsigned int j = 0; j < 16; j++){
				if(rand()%3 == 0){ // 1/3 chance we have a car
					size = rand()%2; // cars size
					roadblocks[i][j] = carcolor;
					if (size >= 1){
						if(j < 15)
						roadblocks[i][++j] = carcolor;
						if(j < 15)
						roadblocks[i][++j] = 0;
					}
				}
			}
		}
		//////////////////////////////////////////////////
		else { //logs
			rowdirection[i] = rand()%2; //left or right
			rowtype[i] = logs;
			for(unsigned int j = 0; j < 16; j++){
				if(rand()%3 == 0){ // 1/3 chance we have a log
					size = rand()%2; // log size
					roadblocks[i][j] = logcolor;
					if(j < 15)
					roadblocks[i][++j] = logcolor;
					if (size >= 1){
						if(j < 15)
						roadblocks[i][++j] = logcolor;
						if(j < 15)
						roadblocks[i][++j] = watercolor;
					}
				}
				else
				roadblocks[i][j] = watercolor;
			}
		}
		///////////////////////////////////////////////
	}
	token = 0;
	unsigned char padLanes = 0, numpads = 0, col = 0;
	//checks how many lilipad lanes there are
	for(unsigned int i = 1; i < 7; i++){
		if(rowtype[i] == pads)
			padLanes++;
	}
	if(padLanes >= 1){
		padLanes = 6%padLanes + 1; 
	
		unsigned char cnt = 0, row = 0;
		for(unsigned int i = 1; i < 7; i++){
			if(rowtype[i] == pads){
				cnt++;
				if (cnt == padLanes){ //i is lane we add powerup in
					row = i;
				}
			}	
		}
		//////////////////////////////////////////////////////////
		//row contains row we need to add powerup in
		//we need to find which col to add in
		for(unsigned int i = 0; i < 8; i++){
			if(roadblocks[row][i] == padcolor){
				numpads++;
			}
		}
		//find which col pad we need to add powerup in
		numpads = 8%numpads + 1;
		cnt = 0;
		for(unsigned int i = 0; i < 8; i++){
			if(roadblocks[row][i] == padcolor){
				cnt++;
				if(cnt == numpads){
					col = i;
				}
			}
		}
		//Now we have row and col of power up;
		token = 1;
		roadblocks[row][col] = redgreen;
	}
}

//display current 2d matrix LED
enum SM1_States { SM1_init, SM1_game, SM1_transition };
int SMTick1(int state) {
	displaymatrix();
	return state;
}

//frog movement
enum SM2_States { SM2_wait };
int SMTick2(int state) {
	if(movementlock != 0)
		return state;
	static unsigned char i = 0, x = 8;
	//left and right, and up and down movement happens independently
	//left and right movement
	if (i % 2 == 0){
		if( x >= 12){
			if (frog[1] != 0)
				frog[1]--;
		}
		else if( x <= 1){
			if (frog[1] != 7)
				frog[1]++;
		}
		x = 0x0F & (~ADC/64);
		Set_A2D_Pin(0x00);
	}
	else{ //up and down movement
		if( x >= 12){
			if (frog[0] != 0)
				frog[0]--;
		}
		else if( x <= 2){
			if (frog[0] != 7)
				frog[0]++;
		}
		x = 0x0F & (~ADC/64);
		Set_A2D_Pin(0x01);
	}
	i++;
	
	//game over
	if(matrix[frog[0]][frog[1]] == red || matrix[frog[0]][frog[1]] == blue){
		gameover = 1;
	}
		
	return state;
}

//init roadblocks
enum SM3_States { SM3_wait, SM3_blink };
int SMTick3(int state) {
	
	static char once = 1;
	if(once){
		initroadblocks();
		updatematrix();
	}
	once=0;
	
	return state;
}

//Cars movement
enum SM4_States { SM4_display };
int SMTick4(int state) {
	static unsigned char temp, temp2;
	
	if (movementlock != 0)
		return state;
		
	//if row type is car we scroll through it left or right
	for(unsigned int r = 0; r < 7;r++){
		temp = roadblocks[r][0];
		temp2 = roadblocks[r][rbcsize-1];
		
		if(rowdirection[r] == left && rowtype[r] == cars){
			for(unsigned int c = 0; c < rbcsize; c++){
				if(c != rbcsize-1)
					roadblocks[r][c] = roadblocks[r][c+1];
				else
					roadblocks[r][c] = temp;
			}
		}
		else if(rowdirection[r] == right && rowtype[r] == cars){
			roadblocks[r][15] =  roadblocks[r][14];
			roadblocks[r][14] =  roadblocks[r][13];
			roadblocks[r][13] =  roadblocks[r][12];
			roadblocks[r][12] =  roadblocks[r][11];
			roadblocks[r][11] =  roadblocks[r][10];
			roadblocks[r][10] =  roadblocks[r][9];
			roadblocks[r][9] =  roadblocks[r][8];
			roadblocks[r][8] =  roadblocks[r][7];
			roadblocks[r][7] =  roadblocks[r][6];
			roadblocks[r][6] =  roadblocks[r][5];
			roadblocks[r][5] =  roadblocks[r][4];
			roadblocks[r][4] =  roadblocks[r][3];
			roadblocks[r][3] =  roadblocks[r][2];
			roadblocks[r][2] =  roadblocks[r][1];
			roadblocks[r][1] =  roadblocks[r][0];
			roadblocks[r][0] =  temp2;
		}
	}	
	updatematrix();
	return state;
}

//logs movement
enum SM5_States { SM5_display };
int SMTick5(int state) {
	static unsigned char temp, temp2;
	
	if(movementlock != 0)
		return state;
		
	//if row type is logs 
	for(unsigned int r = 0; r < 7;r++){
		temp = roadblocks[r][0];
		temp2 = roadblocks[r][rbcsize-1];
		
		//Moves logs either left or right
		if(rowdirection[r] == left && rowtype[r] == logs){
			if (frog[0] == r && frog[1]>0)
				frog[1]--;
			for(unsigned int c = 0; c < rbcsize; c++){
				if(c != rbcsize-1)
				roadblocks[r][c] = roadblocks[r][c+1];
				else
				roadblocks[r][c] = temp;
			}
		}
		else if(rowdirection[r] == right && rowtype[r] == logs){
			if (frog[0] == r && frog[1]<7)
				frog[1]++;
			roadblocks[r][15] =  roadblocks[r][14];
			roadblocks[r][14] =  roadblocks[r][13];
			roadblocks[r][13] =  roadblocks[r][12];
			roadblocks[r][12] =  roadblocks[r][11];
			roadblocks[r][11] =  roadblocks[r][10];
			roadblocks[r][10] =  roadblocks[r][9];
			roadblocks[r][9] =  roadblocks[r][8];
			roadblocks[r][8] =  roadblocks[r][7];
			roadblocks[r][7] =  roadblocks[r][6];
			roadblocks[r][6] =  roadblocks[r][5];
			roadblocks[r][5] =  roadblocks[r][4];
			roadblocks[r][4] =  roadblocks[r][3];
			roadblocks[r][3] =  roadblocks[r][2];
			roadblocks[r][2] =  roadblocks[r][1];
			roadblocks[r][1] =  roadblocks[r][0];
			roadblocks[r][0] =  temp2;
		}
	}
	updatematrix();
	return state;
}

//transition
enum SM6_States { SM6_init, SM6_game, SM6_transition };
int SMTick6(int state) {
	static unsigned char winner = 0, numrows = 0, frogcolor;
	static unsigned long cnt = 0;
	switch(state){ //actions
		case SM6_init:
			break;
		case SM6_game:
			//death in the game
			frogcolor = matrix[frog[0]][frog[1]];
			if(frogcolor == red || frogcolor == blue){
				if (score >= 1)
					score--;
				frog[0] = 7;
				frog[1] = 3;	
			}
			
				
			if(frog[0] == 0 && token == 0)
				winner = 1;
			
			break;
		case SM6_transition:
			if(cnt >= 5){
				//transitions through matrix
				for(unsigned int r = 0; r < 7;r++){ 
					for(unsigned int c = 0; c < 8; c++){
						matrix[7-r][c] = matrix[6-r][c];
					}
				}
				//puts roadblocks into the matrix
				for(unsigned int c = 0; c < 8; c++){
					matrix[0][c] = roadblocks[6-numrows][c];
				}
				//deletes row 0 because code puts things in it
				if(numrows == 6){
					for(unsigned int i = 0; i < 8;i++){
						matrix[0][i] = 0;
					}
				}
				//end transition
				frog[0]++;
				cnt = 0;
				numrows++;
			}
			cnt++;
			break;
		default:
			break;
	}
	switch (state){ //transitions
		case SM6_init:
			state = SM6_game;
		break;
		case SM6_game:
			if(winner && token == 0){
				movementlock = 1;
				numrows = 0;
				state = SM6_transition;
				winner = 0;
				initroadblocks();
			}
		break;
		case SM6_transition:
			if(numrows == 7){
				score++;
				frog[0] = 7;
				state = SM6_game;
				updatematrix();
				movementlock = 0;
			}
			break;
		default:
			state = SM6_init;
			break;
	}

	return state;
}

enum SM7_States { SM7_display };
int SMTick7(int state) {
	return state;
}


// --------END User defined FSMs-----------------------------------------------
// Implement scheduler code from PES.
int main()
{
	PORTA = 0x00; A2D_init(); //output
	//Set_A2D_Pin(0x02); 
	DDRB = 0xFF; PORTB = 0x00; //output
	DDRC = 0xFF; PORTC = 0x00; //output	
	DDRD = 0xFF; PORTD = 0x00; //output
	srand(45766);
			
	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = 2;   //display matrix
	unsigned long int SMTick2_period = 100; //joystick
	unsigned long int SMTick3_period = 300; //settings init
	unsigned long int SMTick4_period = 1500; //cars movement
	unsigned long int SMTick5_period = 1500; //logs movement
	unsigned long int SMTick6_period = 200;
	unsigned long int SMTick7_period = 1;

	//Declare an array of tasks
	static task task1, task2, task3, task4, task5, task6, task7;
	task *tasks[] = { &task1, &task2, &task3, &task4, &task5, &task6, &task7 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// Task 1
	task1.state = -1;
	task1.period = SMTick1_period;
	task1.elapsedTime = SMTick1_period;
	task1.TickFct = &SMTick1;
	
	// Task 2
	task2.state = -1;
	task2.period = SMTick2_period;
	task2.elapsedTime = 0;
	task2.TickFct = &SMTick2;
	
	// Task 3
	task3.state = -1;
	task3.period = SMTick3_period;
	task3.elapsedTime = SMTick3_period; 
	task3.TickFct = &SMTick3; 
	
	// Task 4
	task4.state = -1;
	task4.period = SMTick4_period;
	task4.elapsedTime = SMTick4_period;
	task4.TickFct = &SMTick4; 
	
	// Task 5
	task5.state = -1;
	task5.period = SMTick5_period;
	task5.elapsedTime = SMTick5_period;
	task5.TickFct = &SMTick5;
	
	// Task 6
	task6.state = -1;
	task6.period = SMTick6_period;
	task6.elapsedTime = SMTick6_period;
	task6.TickFct = &SMTick6;	
		
	// Task 6
	task7.state = -1;
	task7.period = SMTick7_period;
	task7.elapsedTime = SMTick7_period;
	task7.TickFct = &SMTick7;
	
	// Set the timer and turn it on
	TimerSet(1);
	TimerOn();
	unsigned short i; // Scheduler for-loop iterator

	while(1) {	
		reset = ~(PIND & 0x01);	
		for ( i = 0; i < numTasks; i++ ) {
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	// Error: Program should not exit!
	return 0;
}