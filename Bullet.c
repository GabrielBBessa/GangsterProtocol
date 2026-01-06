#include <stdlib.h>
#include "Bullet.h"

struct bullet* bullet_create(unsigned short x, unsigned short y, unsigned char trajectory,int speed,struct bullet *next){	//Implementação da função "bullet_create"

	if ((trajectory < 0) || (trajectory > 7)) return NULL;																	//Verifica se a trajetória informada para o projétil é válida

	struct bullet *new_bullet = (struct bullet*) malloc(sizeof(struct bullet));												//Aloca memória na heap para uma instância de projétil
	if (!new_bullet) return NULL;																							//Verifica o sucesso da alocação de memória; retorna NULL em caso de falha
	new_bullet->x = x;																										//Armazena a posição X informada
	new_bullet->y = y;																										//Armazena a posição Y informada
	new_bullet->speed = speed;																								//Armazena a velocidade da bala
	new_bullet->lifetime = BULLET_TIME;																						//Armazena o tempo que a bala fica viva
	new_bullet->trajectory = trajectory;																					//Armazena a trajetória informada
	new_bullet->next = (struct bullet*) next;																				//Armazena o próximo projétil informado
	return new_bullet;																										//Retorna a instância criada de projétil
}

void bullet_destroy(struct bullet *element){																				//Implementação da função "bullet_destroy"

	free(element);																											//Libera a memória da instância de projétil
}
