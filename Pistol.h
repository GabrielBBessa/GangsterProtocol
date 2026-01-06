#ifndef __PISTOL__ 																													//Guardas de inclusão (!)
#define __PISTOL__																													//Guardas de inclusão (!)

#include "Bullet.h"

#define PISTOL_COOLDOWN 10																											//Tamanho, em frames, do tempo entre um tiro e outro (!)

struct pistol{																														//Definição da estrutura de uma pistola (!)
	unsigned char timer;																											//Relógio de intervalo entre tiros (!)
	struct bullet *shots;																											//Lista de balas ativas no campo disparadas pelas arma	 (!)
};																																	//Definição do nome da estrutura (!)

struct pistol* pistol_create();																										//Protótipo da função de criação de uma pistola (!)
struct bullet* pistol_shot(unsigned short x, unsigned short y,unsigned char trajectory,int speed ,struct pistol *gun);				//Protótipo da função de disparo de uma pistola (!)
void pistol_destroy(struct pistol *element);																						//Protótipo da função de destruição de uma pistola (!)

#endif																																//Guardas de inclusão (!)
