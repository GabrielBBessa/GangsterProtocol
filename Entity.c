#include <stdlib.h>
#include "Entity.h"
#include <stdio.h>


struct entity* entity_create(int id,int large,int high, int face, int life,int speed ,
float x, float y, int max_x, int max_y){																										//Implementação da função "entity_create"

	if ((x - large/2 < 0) || (x + large/2 > max_x) || (y - high/2 < 0) || (y + high/2 > max_y)) return NULL;									//Verifica se a posição inicial é válida; caso não seja, retorna NULL
	if (face > 3) return NULL;																													//Verifica se a face principal da entidade é válida (!)
	
	struct entity *new_entity = (struct entity*) malloc(sizeof(struct entity));																	//Aloca memória na heap para uma nova entidade
	new_entity->id = id;
	new_entity->large = large;																													//Insere o tamanho da largura do retangulo
	new_entity->high = high;																													//Insere o tamanho da altura do retangulo
	new_entity->hp = life;																														//Insere a indicação da face principal da entidade (!)
	new_entity->face = face;																													//Insere a velocidade da entidade (!)
	new_entity->speed = speed;
	new_entity->x = x;																															//Insere a posição inicial central de X
	new_entity->y = y;																															//Insere a posição inicial central de Y
	new_entity->control = joystick_create();																									//Insere o elemento de controle da entidade (!)
	new_entity->gun = pistol_create();																											//Insere o elemento de disparos da entidade (!)
	new_entity->knockback_speed = 0;																											//Insere o recuo que a entidade terá ao sofrer um tiro
	return new_entity;																															//Retorna a nova entidade
}

void entity_move(struct entity *element, char steps ,int trajectory, int max_x, int max_y){														//Implementação da função "entity_move" (-1)

	if (!trajectory){ 
		if ((element->x - steps*ENTITY_STEP) - element->large/2 >= 0){																			//Verifica se a movimentação para a direita é desejada e possível; se sim, efetiva a mesma
			 element->x = element->x - steps * element->speed; 	
			 element->face = 0;
		}
	}
	
																																				//Verifica se a movimentação para a esquerda é desejada e possível; se sim, efetiva a mesma
	else if (trajectory == 1){ 
		if ((element->x + steps*element->speed) + element->large/2 <= max_x){
			element->x = element->x + steps*element->speed;	
			element->face = 1;
		}
	}																																			
	
	else if (trajectory == 2){ 																													//Verifica se a movimentação para cima é desejada e possível; se sim, efetiva a mesma
		if ((element->y - steps*element->speed) - element->large/2 >= 0){ 
			if(element->speed <= 10 || (element->control->right == 0 && element->control->left == 0)) element->y = element->y - steps*10;
			
			else element->y = element->y - steps*15;
		}
	}																																	
	
	else if (trajectory == 3){ 	
		if ((element->y + steps * element->speed) <= max_y) element->y = element->y + steps * element->speed;									//Verifica se a movimentação para baixo é desejada e possível; se sim, efetiva a mesma
	
		else element->y = max_y;	
	}			
}

void entity_shot(struct entity *element, int speed, int bullets) {
    struct bullet *shot = NULL; 
    
    float distance = 40.0f; 

    for (int i = 0; i < bullets; i++) {
        

        float offset = i * distance;


        if (!element->face) { // Esquerda
            shot = pistol_shot(element->x - offset, element->y, element->face, speed, element->gun);
        }
        else if (element->face == 1) { // Direita
            shot = pistol_shot(element->x + offset, element->y, element->face, speed, element->gun);
        }
        else if (element->face == 2) { // Cima
            shot = pistol_shot(element->x, element->y - offset, element->face, speed, element->gun);
        }
        else if (element->face == 3) { // Baixo
            shot = pistol_shot(element->x, element->y + offset, element->face, speed, element->gun);
        }
        else if (element->face == 4) { // Diagonal (ex: Esquerda e Baixo)
             shot = pistol_shot(element->x - (offset * 0.707f), element->y - (offset * 0.707f), element->face, speed, element->gun);
        }
        
        else if (element->face == 5) { // Diagonal (ex: Esquerda e Baixo)
             shot = pistol_shot(element->x - (offset * 0.707f), element->y - (offset * 0.707f), element->face, speed, element->gun);
        }
        
        else if (element->face == 6) { // Diagonal (ex: Esquerda e Baixo)
             shot = pistol_shot(element->x - (offset * 0.707f), element->y - (offset * 0.707f), element->face, speed, element->gun);
        }
        
        // --- MUDANÇA 2: A INSERÇÃO CORRETA NA LISTA ---
        // Substituímos 'element->gun->shots = shot;' por estas duas linhas
        if (shot) {
            // Primeiro, o 'next' da nova bala aponta para o início antigo da lista
            shot->next = element->gun->shots;
            // Depois, o início da lista passa a ser a nova bala
            element->gun->shots = shot;
        }
    }
}

void update_speed(struct entity *element,int speed){	element->speed = speed;}																//Muda a velocidade da Entidade


void entity_destroy(struct entity *element){																									//Implementação da função "entity_destroy"
	pistol_destroy(element->gun);																												//Destrói o armamento da Entidade (!)
	joystick_destroy(element->control);																											//Destrói controle da Entidade (!)
	free(element);																																//Libera a memória da Entidade na heap
}
