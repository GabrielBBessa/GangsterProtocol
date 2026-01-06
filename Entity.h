#ifndef __ENTITY__ 																													//Guardas de inclusão
#define __ENTITY__																													//Guardas de inclusão																										

#include "Joystick.h"																												//Estrutura e procedimentos relacionados ao controle da entidade (!)
#include "Pistol.h" 																												//Estrutura e procedimentos relacionados ao controle da arma (pistola) no jogo (!)
#include "Bullet.h"

#define ENTITY_STEP 10																												//Tamanho, em pixels, de um passo da entidade

struct entity{																														//Definição da estrutura de uma entidade
	int id;
	int large;																														//Largura do retangulo
	int high;																														//Altura do retangulo
	int face;																														//A face principal da entidade, algo como a sua "frente" (!)
	int hp;																															//Quantidade de vida da entidade, em unidades (!)
	float x;																														//Posição X do centro da entidade
	float y;																														//Posição Y do centro da entidade
	int speed;
	struct joystick *control;																										//Elemento de controle da entidade no jogo (!)		
	struct pistol *gun;																												//Elemento para realizar disparos no jogo (!)																														
	float knockback_speed;
};

struct entity* entity_create(int id,int large,int high, int face, int life ,int speed ,												//Protótipo da função de criação de uma entidad	
float x, float y, int max_x, int max_y);																							
void entity_move(struct entity *element, char steps, int trajectory, int  max_x, int  max_y);										//Protótipo da função de movimentação de uma entidade (!)
void entity_shot(struct entity *element,int speed,int bullets);																		//Protótipo da função de disparo de uma entidade (!)
void update_speed(struct entity *element,int speed);																				//Atualiza a velocidade da entidade
void entity_destroy(struct entity *element);																						//Protótipo da função de destruição de uma entidade

#endif																															
