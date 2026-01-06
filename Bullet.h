#ifndef __BULLET__ 																															//Guardas de inclusão (!)
#define __BULLET__																															//Guardas de inclusão (!)

#define BULLET_TIME 30

struct bullet{																																//Definição da estrutura de um pŕojétil (!)
	unsigned short x;																														//Local, no eixo x, onde se localiza a bala (!)
	unsigned short y;																														//Local, no eixo y, onde se localiza a bala (!)
	unsigned char trajectory;																												//Tempo que a bala dura
	int lifetime;																															//Velocidade da bala
	int speed;
	struct bullet *next; 																													//Próxima bala, se houver, formando uma lista encadeada (!)
};																																			//Definição do nome da estrutura (!)

struct bullet* bullet_create(unsigned short x, unsigned short y, unsigned char trajectory,int speed, struct bullet *next);					//Protótipo da função de criação de uma bala (!)
void bullet_destroy(struct bullet *element);																								//Protótipo da função de destruição de uma bala (!)

#endif																																		//Guardas de inclusão (!)

