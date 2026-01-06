#include <stdlib.h>
#include "Joystick.h"

struct joystick* joystick_create(){															//Implementação da função "joystick_create" (!)

	struct joystick *element = (struct joystick*) malloc (sizeof(struct joystick));			//Aloca memória na heap para um novo controle (!)
	element->right = 0;																		//Insere o estado de desligado para o botão de movimentação à direita (!)
	element->left = 0;																		//Insere o estado de desligado para o botão de movimentação à esquerda (!)
	element->jumps = 0;																		//Insere o estado de desligado para o botão de movimentação para cima (!)
	element->up = 0;																		//Insere o estado de desligado para o botão de mirar para cima (!)
	element->down = 0;																		//Insere o estado de desligado para o botão de caindo (!)
    element->crouch = 0;																	//Insere o estado de desligado para o botão de agachado (!)									
	element->fire = 0;
	return element;																			//Retorna o novo controle (!)
}

void joystick_reset(struct joystick* element) {												//Reseta o joystick
    element->right = 0;
    element->left = 0;
    element->jumps = 0;
    element->up = 0;
    element->down = 0;
    element->crouch = 0;
	element->fire = 0;
}

void joystick_destroy(struct joystick *element){ free(element);}							//Implementação da função "joystick_destroy"; libera a memória do elemento na heap (!)

void joystick_left(struct joystick *element,int state){ element->left = state;}				//Implementação da função "joystick_left"; coloca "state" no botão (!)

void joystick_right(struct joystick *element,int state){ element->right = state;}			//Implementação da função "joystick_right"; coloca "state" no botão (!)

void joystick_jumps(struct joystick *element,int jumps){ element->jumps = jumps;}			//Implementação da função "joystick_jumps" inicia o pulo

void joystick_down(struct joystick *element,int state){ element->down = state;}				//Implementação da função "joystick_down"; coloca "state" no botão (!) 

void joystick_crouch(struct joystick *element,int state){ element->crouch = state;}			//Implementação da função "joystick_crouch"; coloca "state" no botão (!)

void joystick_fire(struct joystick *element,int state){ element->fire = state;}				//Implementação da função "joystick_fire"; coloca "state" no botão (!)

void joystick_up(struct joystick *element,int state){ element->up = state;}					//Implementação da função "joystick_up"; coloca "state" no botão (!)












