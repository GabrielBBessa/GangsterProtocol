#ifndef __JOYSTICK__ 																												//Guardas de inclusão (!)
#define __JOYSTICK__																												//Guardas de inclusão (!)

struct joystick{																													//Definição da estrutura de um controle (!)
	unsigned char right;																											//Botão de movimentação à direta (!)
	unsigned char left;																												//Botão de movimentação à esquerda (!)
	int jumps;																														//Botão de movimentação para cima (!)
	int up;																															//Botão de atirar pra cima
	int crouch;																														//Botão de agachar
	unsigned char down;																												//Botão de movimentação para baixo (!)
	unsigned char fire;																												//Botão do tiro (!)
};

																																
struct joystick* joystick_create();																									//Protótipo da função de criação de um controle (!)
void joystick_reset(struct joystick* element);																						//Reseta o joystick
void joystick_destroy(struct joystick *element);																					//Protótipo da função de destruição de um controle (!)
void joystick_right(struct joystick *element,int state);																			//Protótipo da função de ativação/desativação do botão de movimentação à direita (!)
void joystick_left(struct joystick *element,int state);																				//Protótipo da função de ativação/desativação do botão de movimentação à esquerda (!)
void joystick_jumps(struct joystick *element,int jumps);																			//Protótipo da função de ativação/desativação do botão de pulo (!)
void joystick_down(struct joystick *element,int state);																				//Protótipo da função de ativação/desativação do botão de movimentação para baixo (!)
void joystick_crouch(struct joystick *element,int state);																			//Protótipo da função de ativação/desativação do botão de agachar (!)
void joystick_fire(struct joystick *element,int state);																				//Protótipo da função de ativação/desativação do botão de tiro (!)
void joystick_up(struct joystick *element,int state);																				//Protótipo da função de ativação/desativação do botão de mirar pra cima (!)
#endif																																
