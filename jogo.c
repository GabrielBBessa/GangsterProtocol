#include <stdio.h>
#include <allegro5/allegro5.h>                                                      
#include <allegro5/allegro_font.h>                                                  
#include <allegro5/allegro_ttf.h>                                                  
#include <allegro5/allegro_image.h>                                                 
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>
#include "Entity.h"
#include "Joystick.h"
#include "Bullet.h"
#include "Pistol.h"

#define FPS 30

//MAPA
#define SCREEN_X 960
#define SCREEN_Y 540
#define floor_size 430

//Telas
#define MENU 1
#define PLAYING 2 
#define GAME_OVER 3
#define WIN 4
#define OPTIONS 5

//Sprite
#define large_sprite 60
#define high_sprite 75
#define high_crouched 40
#define scale 2


//Player Atributes
#define JUMP 15
#define player_large 70
#define player_high 120
#define ENTITY_COOLDOWN 10
#define BULLET_MOVE 30
#define ENTITY_WALK 10
#define ENTITY_RUN 20 

//Bot Atributes
#define BOT_COOLDOWN 30
#define BOT_LIFE 5

//BOSS atributes
#define large_sprite_boss 240
#define high_sprite_boss 240
#define BOSS_POSITION 270
#define boss_large 250
#define boss_high 250
#define BOSS_WALK 10
#define BOSS_LIFE 40
#define BOSS_COOLDOWN 5


//Atualiza a posição dos tiros no jogo
void update_bullets(struct entity *element,float camera_x){	
	
	struct bullet *previous = NULL;																						
																				
	for (struct bullet *index = element->gun->shots; index != NULL;){												//Para cada projétil presente na lista de projéteis disparados (!)
		
		if(index->trajectory == 6){																					//Tiro para a direita e cima
			index->x = index->x + cos(ALLEGRO_PI/6) * BULLET_MOVE; 
            index->y = index->y - sin(ALLEGRO_PI/6) * BULLET_MOVE;
		}

		if(index->trajectory == 5){																					//Tiro para a esquerda e cima
			index->x = index->x - cos(ALLEGRO_PI/6) * BULLET_MOVE; 
            index->y = index->y - sin(ALLEGRO_PI/6) * BULLET_MOVE;
		}
			
		if(index->trajectory == 4){																					//Tiro para a direita e baixo																					
		//Quanto maior o divisor mais reto vai																					
			index->x = index->x - cos(ALLEGRO_PI/9) * BULLET_MOVE; 
            index->y = index->y + sin(ALLEGRO_PI/9) * BULLET_MOVE;
        }
        
		if(index->trajectory == 3)	index->y = index->y + BULLET_MOVE;												//Se a trajetória for para baixo, atualiza a posição para a baixo (!)
		if(index->trajectory == 2)	index->y = index->y - BULLET_MOVE;												//Se a trajetória for para cima, atualiza a posição para a cima (!)
		if (index->trajectory == 0) index->x = index->x - BULLET_MOVE;												//Se a trajetória for para a esquerda, atualiza a posição para a esquerda (!)
		if (index->trajectory == 1) index->x = index->x + BULLET_MOVE;												//Se a trajetória for para a direita, atualiza a posição para a direita (!)
		
		 index->lifetime--;
		 
		 if(index->lifetime <= 0){																					//Verifica se o projétil saiu das bordas da janela (!)
			if (previous){																							//Verifica se não é o primeiro elemento da lista de projéteis (!)
				previous->next = index->next;																		//Se não for, salva o próximo projétil (!)
				bullet_destroy(index);																				//Chama o destrutor para o projétil atual (!)
				index = (struct bullet*) previous->next;															//Atualiza para o próximo projétil (!)
			}
			else {																									//Se for o primeiro projétil da lista (!)
				element->gun->shots = (struct bullet*) index->next;													//Atualiza o projétil no início da lista (!)
				bullet_destroy(index);																				//Chama o destrutor para o projétil atual (!)
				index = element->gun->shots;																		//Atualiza para o próximo projétil (!)
			}
		}
		else{																										//Se não saiu da tela (!)
			previous = index;																						//Atualiza o projétil anterior (para a próxima iteração) (!)
			index = (struct bullet*) index->next;																	//Atualiza para o próximo projétil (!)
		}
	}
}

//Atualiza a dificuldade de acordo com a escolha do jogador
void put_difficulty(struct entity* element , int difficulty){ 
	element->hp = element->hp * difficulty;
}

//Atualiza a posição da entidade no jogo
void update_position(struct entity* element,int large_img,int camera,int numShots){

	if (element->knockback_speed != 0) {																				//Implementa o knockback que diminuira de acordo com a quantidade de tiros sofridos
    	element->x = element->x + element->knockback_speed;
    	element->knockback_speed = element->knockback_speed * 0.8f; 
    	
    	if (element->knockback_speed < 1) 	element->knockback_speed = 0;
	}

    if (element->control->left  && element->x > 120)    entity_move(element, 1, 0, large_img, SCREEN_Y);				//Movimentação pra esquerda

    if (element->control->right){																						//Movimentação pra esquerda
    	 entity_move(element, 1, 1, large_img, SCREEN_Y);
    }
	
	if (element->control->jumps != 0){																					//Movimentação pra cima(pulo)
		entity_move(element, 1, 2, large_img, SCREEN_Y);
		element->control->jumps--;
	}
	
    if (element->control->jumps == 0 && element->y < floor_size)	entity_move(element, 1, 3, large_img, SCREEN_Y);	//Lógica da gravidade
    
    if (element->control->up == 1) element->face = 2;																	//Faz o player atirar pra cima(quando está parado)
    
    if (element->control->up == 2) element->face = 5;																	//Faz o player atirar pra cima e pra direita (quando está andando pra direita)
    
    if (element->control->up == 3) element->face = 6;																	//Faz o player atirar pra cima e pra esquerda (quando está andando pra esquerda)
   
    
	if (element->control->fire){																						//Verifica se o jogador está atirando (!)
		if (!element->gun->timer){	
																														//Verifica se a arma do jogador não está em cooldown (!) 
				entity_shot(element,BULLET_MOVE,numShots);																																						
				element->gun->timer = BOSS_COOLDOWN;																	//Se não estiver, faz um disparo (!)
		} 
	}
	
	update_bullets(element,camera);
}

//Atualiza o boss
void Boss_Update(struct entity* element,int large_img,int camera,int numShots){
	
    if (element->control->left){  											//Movimentação pra esquerda  
    	entity_move(element, 1, 0, large_img, SCREEN_Y);
    	element->face = 3;
	}
	
    if (element->control->right){											//Movimentação pra direita 
    	 entity_move(element, 1, 1, large_img, SCREEN_Y);
    	 element->face = 3;
    }
	
	if (element->control->jumps != 0){										//Movimentação pra cima 
		entity_move(element, 1, 2, large_img, SCREEN_Y);
		element->control->jumps--;
	}
    
    
	if (element->control->fire){											//Verifica se o boss está atirando (!)
		if (!element->gun->timer){											//Verifica se a arma do boss não está em cooldown (!) 
				entity_shot(element,BULLET_MOVE,numShots);																																						
				element->gun->timer = BOSS_COOLDOWN;						//Se não estiver, faz um disparo (!)
		} 
	}
	
	update_bullets(element,camera);
}


//IA que comanda a movimentação do boss
int Boss_AI(struct entity* element, int BossComands,int large_img){

	if(BossComands < 50)	element->face = 4;								//Garante a direção do tiro	
	
	
	//Movimentação do boss(sobe vai pra esquerda fica volta pra direita e desce)
	if(BossComands == 50)	joystick_jumps(element->control,20);			
	
	if(BossComands == 60)	joystick_left(element->control,1);	
	
	if(BossComands == 110)	joystick_left(element->control,0);
	
	if(BossComands == 140)	joystick_right(element->control,1);
	
	if(BossComands == 190)	joystick_right(element->control,0);
	
	if(BossComands > 250){	
		if(BossComands < 260){	
			entity_move(element, 1, 3, large_img, SCREEN_Y);
			element->face = 4;
		}
		
		else	return 0;
	
	}
	
	return 1;
	
}	

//Faz com que o programa tenha Rolling background
void CameraUpdate(float *cameraPosition, float x , int width,int world_width){
	cameraPosition[0] = -(SCREEN_X / 2) + (x + width / 2);
	
	if(cameraPosition[0] < 0) cameraPosition[0] = 0;
	

    if (cameraPosition[0] > world_width - SCREEN_X) {
        cameraPosition[0] = world_width - SCREEN_X;
    }

    cameraPosition[1] = 0;
}


//Checa se ouve colisão 
unsigned char check_collision(struct entity* a, struct entity* b) {
    // Calcula as bordas do retângulo A
    float a_left = a->x - a->large / 2.0f;
    float a_right = a->x + a->large / 2.0f;
    float a_top = a->y - a->high / 2.0f;
    float a_bottom = a->y + a->high / 2.0f;

    // Calcula as bordas do retângulo B
    float b_left = b->x - b->large / 2.0f;
    float b_right = b->x + b->large / 2.0f;
    float b_top = b->y - b->high / 2.0f;
    float b_bottom = b->y + b->high / 2.0f;

    if (a_right < b_left ||  // A está muito à esquerda de B
        a_left > b_right ||  // A está muito à direita de B
        a_bottom < b_top ||  // A está muito acima de B
        a_top > b_bottom) {  // A está muito abaixo de B
        return 0; // Sem colisão
    }

    // Se nenhuma das condições acima for verdadeira, então eles estão colidindo.
    return 1;
}


//Checa se a "victim" foi morta e tira hp dela se tomar um tiro
void check_kill(struct entity *killer, struct entity *victim) {
    
    struct bullet *previous = NULL;
    for (struct bullet *index = killer->gun->shots; index != NULL; ) {
		
		
		//faz a hitbox da arma
        struct entity bullet_hitbox;
        bullet_hitbox.x = index->x;
        bullet_hitbox.y = index->y;
        bullet_hitbox.large = 10; 
        bullet_hitbox.high = 3; 
		
		//verifica se a arma acertou o player
        if (check_collision(&bullet_hitbox, victim)) {

            victim->hp--;
            
            //Garante que o knockback seja na direção em que o tiro veio
            if (killer->x < victim->x) victim->knockback_speed = 2;
            
            else victim->knockback_speed = -2;
            
            //Destroi a bala
            struct bullet* bullet_to_destroy = index;
            if (previous) {
                previous->next = index->next;
                index = (struct bullet*) previous->next;
            } else {
                killer->gun->shots = (struct bullet*) index->next;
                index = killer->gun->shots;
            }
            bullet_destroy(bullet_to_destroy);

        }
        else {
            previous = index;
            index = (struct bullet*) index->next;
        }
    }
}

int main(){

	//Descrição de como jogar 
	printf(" SETA PRA DIREITA: ANDA PRA DIREITA\n SETA PRA ESQUERDA: ANDA PRA ESQUERDA\n");
	printf(" SETA PRA BAIXO: AGACHA\n SETA PRA CIMA: MIRA PRA CIMA(se andar o tiro vai na diagonal da direção)\n");
	printf(" BARRA DE ESPAÇO: PULA(se pegar a pena tem pulo duplo)\n TECLA TAB: CORRE\n TECLA M: ATIRA\n");
	printf(" TECLA P: PAUSA\n TECLA O:DESPAUSA\n TECLA ESC: SAI DO JOGO\n");
			

    al_init();                                                                      //Faz a preparação de requisitos da biblioteca Allegro
    al_install_keyboard();                                                          //Habilita a entrada via teclado (eventos de teclado), no programa
    al_init_image_addon();                                                          //Habilita o uso de imagens
    al_init_primitives_addon();
    al_init_ttf_addon();
    
    //Fontes usadas nos menus do jogo
    ALLEGRO_FONT *font = NULL;
    ALLEGRO_FONT *title = NULL;
    ALLEGRO_FONT *EndGameFont = NULL;
    ALLEGRO_FONT *difficulty = NULL;
    int font_size = 50;
    
    //Lógica da música do jogo
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(10);

	ALLEGRO_AUDIO_STREAM* soundtrack = al_load_audio_stream("archives/music/brain-implant-cyberpunk-sci-fi-trailer-action-intro-330416.ogg", 4, 2048);
	if (!soundtrack) {
    	fprintf(stderr, "Falha ao carregar a trilha sonora!\n");
    	return -1;
	}	
	float volume = 0;
	al_set_mixer_gain(al_get_default_mixer(), volume);
	al_attach_audio_stream_to_mixer(soundtrack, al_get_default_mixer());
	al_set_audio_stream_playmode(soundtrack, ALLEGRO_PLAYMODE_LOOP);
    float cameraPosition[2] = {0,0};
  
    int numShots = 1;
    int BotShots = 1;
    int BossShots = 1;
    int crouched = 0;
    int oldFace;
    int bots_alive = 6;
    int extralife = 0;
    int pause = 0;
	
	//Carrega as imagens usadas nos menus do jogo
    font = al_load_ttf_font("archives/fonts/Freshman.ttf", font_size, 0);
    difficulty = al_load_ttf_font("archives/fonts/Freshman.ttf", font_size * 0.7, 0);
    title = al_load_ttf_font("archives/fonts/VotragTexture-Bold.otf", font_size * 2, 0);
    EndGameFont = al_load_ttf_font("archives/fonts/VotragTexture-Bold.otf", font_size * 3, 0);
    ALLEGRO_BITMAP* MenuImage = al_load_bitmap("archives/images/menu.png");
    ALLEGRO_BITMAP* EndGame = al_load_bitmap("archives/images/endGame.png");      
    ALLEGRO_BITMAP* icon = al_load_bitmap("archives/images/icon.png");
    
    
    //Carrega as sprites usadas no jogo
    ALLEGRO_BITMAP* SpritePlayer = al_load_bitmap("archives/images/Gangsters_1_Spritelist.png");
    ALLEGRO_BITMAP* SpriteBot = al_load_bitmap("archives/images/Gangsters_2_Spritelist.png");
    ALLEGRO_BITMAP* SpriteBoss = al_load_bitmap("archives/images/ship.png"); 
    ALLEGRO_BITMAP* SpriteBullet = al_load_bitmap("archives/images/Bullets/6.png");
    ALLEGRO_BITMAP* SpriteBulletBot = al_load_bitmap("archives/images/Bullets/5.png");
    ALLEGRO_BITMAP* SpriteBulletBoss = al_load_bitmap("archives/images/Bullets/7.png");   
    ALLEGRO_BITMAP* SpriteLife = al_load_bitmap("archives/images/Icons/Icons_27.png");
	ALLEGRO_BITMAP* ExtraHeart = al_load_bitmap("archives/images/Icons/Icons_30.png"); 
	ALLEGRO_BITMAP* remaining_enemies = al_load_bitmap("archives/images/Icons/Icons_40.png");
    
    //Variaveis usadas para controlar a animação do jogo
    int frames_x[] = {32,160,287,415,543,671,799,926,1054,1182,1310};
    int frames_y[] = {55,181,311,438,569,694,821,949,1095,1200};
	int frame_Idle = 0;
    int frame_right = 0;
    int frame_left = 0;
    int frame_fire = 0;
    int bot_fire1 = 0;
    int bot_fire2 = 0;
    int bot_fire3 = 0;
    int bot_fire4 = 0;
    int bot_fire5 = 0;
    int bot_fire6 = 0;
    
    
	//Carrega o fundo do jogo
    ALLEGRO_BITMAP* background = al_load_bitmap("archives/images/background.png");
    if (!background) {
    	printf("ERRO FATAL: Nao foi possivel carregar o background. Verifique se o arquivo esta na pasta correta.\n");
		return -1;
	}

	//Salvam o tamanho e largura do menu
    int large_menu = al_get_bitmap_width(MenuImage);
    int high_menu = al_get_bitmap_height(MenuImage);
	
	//Variaveis de controle
    int state = MENU;
    int menu_select = 2;
    int op_select = 2;
    int difficulty_select = 1;
    int has_difficulty = 0;
    
    //Controla o tempo para movimentação do Boss
    int ControlBoss = 0;
    
    
    //Vetor usado para posicionar os inimigos
    int bot_loc[] = {500,1000,1270,2200,2700,3400,3600};

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);                             //Cria o relógio do jogo; isso indica quantas atualizações serão realizadas por segundo (30, neste caso)

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();                           //Cria a fila de eventos; todos os eventos (programação orientada a eventos) 

    ALLEGRO_DISPLAY* disp = al_create_display(SCREEN_X, SCREEN_Y);                  //Cria uma janela para o programa, define a largura (x) e a altura (y) da tela em píxeis (320x320, neste caso)

    if(!disp){
        printf("Erro ao criar o display\n");
        return -1;
    }
    
    al_set_display_icon(disp,icon);
    al_set_window_title(disp,"RunAndGun");

	//Salvam o tamanho e largura do background
    float large_img = al_get_bitmap_width(background);
    float high_img = al_get_bitmap_height(background);

    al_register_event_source(queue, al_get_keyboard_event_source());                //Indica que eventos de teclado serão inseridos na nossa fila de eventos
    al_register_event_source(queue, al_get_display_event_source(disp));             //Indica que eventos de tela serão inseridos na nossa fila de eventos
    al_register_event_source(queue, al_get_timer_event_source(timer));              //Indica que eventos de relógio serão inseridos na nossa fila de eventos    

    struct entity* player_1 = entity_create(0,player_large,player_high,1,40,ENTITY_WALK,player_large, floor_size, large_img, SCREEN_Y);				//Cria o retangulo do primeiro jogador (!)
    if (!player_1) return 1;                                                                
     
    struct entity* bot1 = entity_create(1,player_large + 10,player_high,0,BOT_LIFE,ENTITY_WALK ,bot_loc[0], floor_size, large_img, SCREEN_Y);    	//Cria o retangulo do primeiro bot (!)  
    if (!bot1) return 1;
    
    struct entity* bot2 = entity_create(1,player_large + 10,player_high,0,BOT_LIFE,ENTITY_WALK ,bot_loc[1], floor_size, large_img, SCREEN_Y);    	//Cria o retangulo do segundo bot (!)  
    if (!bot2) return 1;
    
    struct entity* bot3 = entity_create(1,player_large + 10,player_high,0,BOT_LIFE,ENTITY_WALK ,bot_loc[2], floor_size, large_img, SCREEN_Y);    	//Cria o retangulo do terceiro bot (!)  
    if (!bot3) return 1;
    
    struct entity* bot4 = entity_create(1,player_large + 10,player_high,0,BOT_LIFE,ENTITY_WALK ,bot_loc[3], floor_size, large_img, SCREEN_Y);    	//Cria o retangulo do quarto bot (!)  
    if (!bot4) return 1;
    
    struct entity* bot5 = entity_create(1,player_large + 10,player_high,0,BOT_LIFE,ENTITY_WALK ,bot_loc[4], floor_size, large_img, SCREEN_Y);    	//Cria o retangulo do quinto bot (!)  
    if (!bot5) return 1;
    
    struct entity* bot6 = entity_create(1,player_large + 10,player_high,0,BOT_LIFE,ENTITY_WALK ,bot_loc[5], floor_size, large_img, SCREEN_Y);    	//Cria o retangulo do sexto bot (!)  
    if (!bot6) return 1;
    
    struct entity* boss = entity_create(2,boss_large,boss_high,0,BOSS_LIFE,BOSS_WALK,bot_loc[6], BOSS_POSITION, large_img, SCREEN_Y);   			//Cria o retangulo do boss (!)
    if (!boss) return 1;
    
    
   	ALLEGRO_KEYBOARD_STATE keyState;
   	
   	ALLEGRO_TRANSFORM camera;

	
    ALLEGRO_EVENT event;                                                            //Variável que guarda um evento capturado, sua estrutura é definida em: https://www.allegro.cc/manual/5/ALLEGRO_EVENT
    al_start_timer(timer);                                                          //Função que inicializa o relógio do programa
    while(1) {                                                                      //Laço principal do programa
        al_wait_for_event(queue, &event);                                       	//Função que captura eventos da fila, inserindo os mesmos na variável de eventos
		
		//Cria a lógica do menu de inicio
		if(state == MENU){
		
		    if (event.type == ALLEGRO_EVENT_KEY_DOWN) { // Tecla FOI PRESSIONADA
		        if(menu_select == 2){
		            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER){
		            	state = PLAYING;
						joystick_reset(player_1->control);
		            }
		        }
		        
		        if(menu_select == 1){
		        	if (event.keyboard.keycode == ALLEGRO_KEY_ENTER){
		        		state = OPTIONS;
		        		joystick_reset(player_1->control);
		        	}
		        }

		        if(!menu_select){
		            if(event.keyboard.keycode == ALLEGRO_KEY_ENTER) break;
		        }

		        if (event.keyboard.keycode == ALLEGRO_KEY_UP && menu_select != 2)   menu_select = menu_select + 1;

		        if (event.keyboard.keycode == ALLEGRO_KEY_DOWN && menu_select != 0) menu_select = menu_select - 1;

		    }
		}
		
		//cria a lógica da tela de opções
		else if(state == OPTIONS){
			if (event.type == ALLEGRO_EVENT_KEY_DOWN) { // Tecla FOI PRESSIONADA
		        if(op_select == 2){
		           if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT && difficulty_select != 5) difficulty_select = difficulty_select + 2;
	
		        	if (event.keyboard.keycode == ALLEGRO_KEY_LEFT && difficulty_select != 1) difficulty_select = difficulty_select - 2;
				}
		        
		        if(op_select == 1){
		        	if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT && volume != 1)	volume = volume + 0.25;
	
		        	if (event.keyboard.keycode == ALLEGRO_KEY_LEFT && volume != 0)	volume = volume - 0.25;	
		        		
		        	
		        	al_set_mixer_gain(al_get_default_mixer(), volume);
		        }

		        if(op_select == 0){
		            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER){
		            	state = MENU;
						joystick_reset(player_1->control);
		            }
		        }
		        
		        if (event.keyboard.keycode == ALLEGRO_KEY_UP && op_select != 2)   op_select = op_select + 1;

		        if (event.keyboard.keycode == ALLEGRO_KEY_DOWN && op_select != 0) op_select = op_select - 1;
		        
		    }
		}
		
		
        if (event.type == ALLEGRO_EVENT_TIMER){                                 		// O evento indica um evento de relógio, ou seja, verificação se a tela deve ser atualizada (conceito de FPS)
        
			
			al_get_keyboard_state(&keyState);
			
			//Lógica dos tiros na diagonal
			bool up_pressed = al_key_down(&keyState, ALLEGRO_KEY_UP);
			bool left_pressed = al_key_down(&keyState, ALLEGRO_KEY_LEFT);
			bool right_pressed = al_key_down(&keyState, ALLEGRO_KEY_RIGHT);

			if (up_pressed && left_pressed) {
				joystick_up(player_1->control,2); //Diagonal Esquerda-Cima
			}
			else if (up_pressed && right_pressed) {
				joystick_up(player_1->control,3); //Diagonal Direita-Cima
			}
			
			else if (up_pressed) joystick_up(player_1->control,1);
			
			else joystick_up(player_1->control,0);
			
			//Cria o visual do MENU
            if (state == MENU){
                al_clear_to_color(al_map_rgb(0, 0, 0));                                 //Substitui tudo que estava desenhado na tela por um fundo preto
				
                al_draw_scaled_bitmap(MenuImage,0, 0, large_menu, high_menu, 0, 0, SCREEN_X, SCREEN_Y,0);  //Dimensiona a imagem para o tamanho do display    
				
                if(menu_select == 2){
                	al_draw_text(title, al_map_rgb(191, 64, 255), 480, 80, ALLEGRO_ALIGN_CENTRE, "Gangster Protocol");
                     al_draw_text(font, al_map_rgb(255, 64, 192), 480, 250, ALLEGRO_ALIGN_CENTRE, "PLAY");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 480, 350, ALLEGRO_ALIGN_CENTRE, "OPTIONS");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 480, 450, ALLEGRO_ALIGN_CENTRE, "QUIT");
                }
                if(menu_select == 1){
                	al_draw_text(title, al_map_rgb(191, 64, 255), 480, 80, ALLEGRO_ALIGN_CENTRE, "Gangster Protocol");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 480, 250, ALLEGRO_ALIGN_CENTRE, "PLAY");
                    al_draw_text(font, al_map_rgb(255, 64, 192), 480, 350, ALLEGRO_ALIGN_CENTRE, "OPTIONS");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 480, 450, ALLEGRO_ALIGN_CENTRE, "QUIT");
                }
                
                if(menu_select == 0){
                	al_draw_text(title, al_map_rgb(191, 64, 255), 480, 80, ALLEGRO_ALIGN_CENTRE, "Gangster Protocol");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 480, 250, ALLEGRO_ALIGN_CENTRE, "PLAY");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 480, 350, ALLEGRO_ALIGN_CENTRE, "OPTIONS");
                    al_draw_text(font, al_map_rgb(255, 64, 192), 480, 450, ALLEGRO_ALIGN_CENTRE, "QUIT");
                }


                al_flip_display();
            }
            
            //Cria o visual da tela de opções
            else if(state == OPTIONS){
				al_clear_to_color(al_map_rgb(0, 0, 0));                                 
				
                al_draw_scaled_bitmap(MenuImage,0, 0, large_menu, high_menu, 0, 0, SCREEN_X, SCREEN_Y,0);          
                
                if(difficulty_select == 5){
		            al_draw_text(difficulty, al_map_rgb(255, 64, 192), 800, 250, ALLEGRO_ALIGN_CENTRE, "HARD");
		            al_draw_text(difficulty, al_map_rgb(0, 0, 0), 600, 250, ALLEGRO_ALIGN_CENTRE, "MEDIUM");
		            al_draw_text(difficulty, al_map_rgb(0, 0, 0), 400, 250, ALLEGRO_ALIGN_CENTRE, "EASY");
				}
				if(difficulty_select == 3){
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 800, 250, ALLEGRO_ALIGN_CENTRE, "HARD");
					al_draw_text(difficulty, al_map_rgb(255, 64, 192), 600, 250, ALLEGRO_ALIGN_CENTRE, "MEDIUM");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 400, 250, ALLEGRO_ALIGN_CENTRE, "EASY");
				}
				
				if(difficulty_select == 1){
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 800, 250, ALLEGRO_ALIGN_CENTRE, "HARD");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 600, 250, ALLEGRO_ALIGN_CENTRE, "MEDIUM");
					al_draw_text(difficulty, al_map_rgb(255, 64, 192), 400, 250, ALLEGRO_ALIGN_CENTRE, "EASY");
				}
				
				if(volume == 1){
		            al_draw_text(difficulty, al_map_rgb(255, 64, 192), 750, 350, ALLEGRO_ALIGN_CENTRE, "100%");
		            al_draw_text(difficulty, al_map_rgb(0, 0, 0), 650, 350, ALLEGRO_ALIGN_CENTRE, "75%");
		            al_draw_text(difficulty, al_map_rgb(0, 0, 0), 550, 350, ALLEGRO_ALIGN_CENTRE, "50%");
		            al_draw_text(difficulty, al_map_rgb(0, 0, 0), 450, 350, ALLEGRO_ALIGN_CENTRE, "25%");
		            al_draw_text(difficulty, al_map_rgb(0, 0, 0), 350, 350, ALLEGRO_ALIGN_CENTRE, "0%");
				}
				if(volume == 0.75){
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 750, 350, ALLEGRO_ALIGN_CENTRE, "100%");
					al_draw_text(difficulty, al_map_rgb(255, 64, 192), 650, 350, ALLEGRO_ALIGN_CENTRE, "75%");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 550, 350, ALLEGRO_ALIGN_CENTRE, "50%");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 450, 350, ALLEGRO_ALIGN_CENTRE, "25%");
		            al_draw_text(difficulty, al_map_rgb(0, 0, 0), 350, 350, ALLEGRO_ALIGN_CENTRE, "0%");
				}
				
				if(volume == 0.5){
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 750, 350, ALLEGRO_ALIGN_CENTRE, "100%");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 650, 350, ALLEGRO_ALIGN_CENTRE, "75%");
					al_draw_text(difficulty, al_map_rgb(255, 64, 192), 550, 350, ALLEGRO_ALIGN_CENTRE, "50%");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 450, 350, ALLEGRO_ALIGN_CENTRE, "25%");
		            al_draw_text(difficulty, al_map_rgb(0, 0, 0), 350, 350, ALLEGRO_ALIGN_CENTRE, "0%");
				}
				
				if(volume == 0.25){
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 750, 350, ALLEGRO_ALIGN_CENTRE, "100%");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 650, 350, ALLEGRO_ALIGN_CENTRE, "75%");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 550, 350, ALLEGRO_ALIGN_CENTRE, "50%");
					al_draw_text(difficulty, al_map_rgb(255, 64, 192), 450, 350, ALLEGRO_ALIGN_CENTRE, "25%");
		            al_draw_text(difficulty, al_map_rgb(0, 0, 0), 350, 350, ALLEGRO_ALIGN_CENTRE, "0%");
				}
				
				if(!volume){
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 750, 350, ALLEGRO_ALIGN_CENTRE, "100%");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 650, 350, ALLEGRO_ALIGN_CENTRE, "75%");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 550, 350, ALLEGRO_ALIGN_CENTRE, "50%");
					al_draw_text(difficulty, al_map_rgb(0, 0, 0), 450, 350, ALLEGRO_ALIGN_CENTRE, "25%");
		            al_draw_text(difficulty, al_map_rgb(255, 64, 192), 350, 350, ALLEGRO_ALIGN_CENTRE, "0%");
				}
                
				if(op_select == 2){
		        	al_draw_text(title, al_map_rgb(191, 64, 255), 480, 80, ALLEGRO_ALIGN_CENTRE, "Gangster Protocol");
		        	al_draw_text(difficulty, al_map_rgb(255, 0, 0), 200, 250, ALLEGRO_ALIGN_CENTRE, "DIFFICULTY-");
		        	al_draw_text(difficulty, al_map_rgb(0, 0, 0), 200, 350, ALLEGRO_ALIGN_CENTRE, "SOUND");
		        	al_draw_text(difficulty, al_map_rgb(0, 0, 0), 200, 450, ALLEGRO_ALIGN_CENTRE, "BACK");
                }
                
                if(op_select == 1){
                	al_draw_text(title, al_map_rgb(191, 64, 255), 480, 80, ALLEGRO_ALIGN_CENTRE, "Gangster Protocol");
                	al_draw_text(difficulty, al_map_rgb(0, 0, 0), 200, 250, ALLEGRO_ALIGN_CENTRE, "DIFFICULTY-");
                	al_draw_text(difficulty, al_map_rgb(255, 0, 0), 200, 350, ALLEGRO_ALIGN_CENTRE, "SOUND");
                	al_draw_text(difficulty, al_map_rgb(0, 0, 0), 200, 450, ALLEGRO_ALIGN_CENTRE, "BACK");
                }
                
                if(op_select == 0){
                	al_draw_text(title, al_map_rgb(191, 64, 255), 480, 80, ALLEGRO_ALIGN_CENTRE, "Gangster Protocol");
                	al_draw_text(difficulty, al_map_rgb(0, 0, 0), 200, 250, ALLEGRO_ALIGN_CENTRE, "DIFFICULTY-");
                	al_draw_text(difficulty, al_map_rgb(0, 0, 0), 200, 350, ALLEGRO_ALIGN_CENTRE, "SOUND");
                	al_draw_text(difficulty, al_map_rgb(250, 0, 0), 200, 450, ALLEGRO_ALIGN_CENTRE, "BACK");
                }  
                al_flip_display();
			}  
            
            //Cria a tela de vitória do jogador
			if (state == WIN){
    	
    	    	int large_win = al_get_bitmap_width(EndGame);
    			int high_win = al_get_bitmap_height(EndGame);
    		
    			al_draw_scaled_bitmap(EndGame,0, 0, large_win, high_win, 0, 0, SCREEN_X, SCREEN_Y,0);  //Dimensiona a imagem para o tamanho do display
    			al_draw_text(EndGameFont, al_map_rgb(255, 215, 0), 480, 110, ALLEGRO_ALIGN_CENTRE, "YOU WIN");
				
				al_flip_display();
    		}
    		
    		//Cria a tela de derrota do jogador
    		if (state == GAME_OVER){
    	
    	    	int large_over = al_get_bitmap_width(EndGame);
    			int high_over = al_get_bitmap_height(EndGame);
    		
    			al_draw_scaled_bitmap(EndGame,0, 0, large_over, high_over, 0, 0, SCREEN_X, SCREEN_Y,0);  //Dimensiona a imagem para o tamanho do display
    			al_draw_text(EndGameFont, al_map_rgb(255, 0, 0), 480, 100, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
				
				al_flip_display();
    		}
			
            if (state == PLAYING){
 
            	if(pause == 0){
            		
            		//Coloca a dificuldade selecionada pelo player
            		if(!has_difficulty){
            			put_difficulty(bot1, difficulty_select);
						put_difficulty(bot2, difficulty_select);
						put_difficulty(bot3, difficulty_select);
						put_difficulty(bot4, difficulty_select);
						put_difficulty(bot5, difficulty_select);
						put_difficulty(bot6, difficulty_select);
						has_difficulty = 1;
					}
            		
		        	update_position(player_1,large_img,cameraPosition[0],numShots);
		        	update_position(bot1,large_img,cameraPosition[0],BotShots);
		        	update_position(bot2,large_img,cameraPosition[0],BotShots);
		        	update_position(bot3,large_img,cameraPosition[0],BotShots);
		        	update_position(bot4,large_img,cameraPosition[0],BotShots);
		        	update_position(bot5,large_img,cameraPosition[0],BotShots);
		        	update_position(bot6,large_img,cameraPosition[0],BotShots);
		        	Boss_Update(boss,large_img,cameraPosition[0],BossShots);         	

					//Lógica da camera
					CameraUpdate(cameraPosition, player_1->x, player_large,large_img);
		            al_identity_transform(&camera);
		            al_translate_transform(&camera,-cameraPosition[0],-cameraPosition[1]);
		            al_use_transform(&camera); 
		            
		            //Implementa as posições em que se o player pisa no chão ele morre
		            if(player_1->x > 1400 && player_1->x < 1590 && player_1->y >= floor_size){	
		            	state = GAME_OVER;
		            }
		            
		            if(player_1->x > 1940 && player_1->x < 2130 && player_1->y >= floor_size){	
		            	state = GAME_OVER;
		            }
		            
					if(player_1->x > 3430 && player_1->y >= floor_size){	
		            	state = GAME_OVER;
		            }
		            
		            	
		            //Checa se cada um dos bots está vivo
		            
					if(bot1->hp != 0){
						check_kill(bot1, player_1);																																						//Verifica se o primeiro jogador matou o segundo jogador
						check_kill(player_1, bot1);
						if(bot1->hp == 0)	bots_alive--;
					}
					
					if(bot2->hp != 0){
						check_kill(bot2, player_1);																																						//Verifica se o primeiro jogador matou o segundo jogador
						check_kill(player_1, bot2);
						if(bot2->hp == 0)	bots_alive--;
					}
					
					if(bot3->hp != 0){
						check_kill(bot3, player_1);																																						//Verifica se o primeiro jogador matou o segundo jogador
						check_kill(player_1, bot3);
						if(bot3->hp == 0)	bots_alive--;
					}
					
					if(bot4->hp != 0){
						check_kill(bot4, player_1);																																						//Verifica se o primeiro jogador matou o segundo jogador
						check_kill(player_1, bot4);
						if(bot4->hp == 0)	bots_alive--;
					}
					
					if(bot5->hp != 0){
						check_kill(bot5, player_1);																																						//Verifica se o primeiro jogador matou o segundo jogador
						check_kill(player_1, bot5);
						if(bot5->hp == 0)	bots_alive--;
					}
					
					if(bot6->hp != 0){
						check_kill(bot6, player_1);																																						//Verifica se o primeiro jogador matou o segundo jogador
						check_kill(player_1, bot6);
						if(bot6->hp == 0)	bots_alive--;
					}
					
					if(boss->hp != 0){
						check_kill(boss, player_1);																																						//Verifica se o primeiro jogador matou o segundo jogador
						check_kill(player_1, boss);
					}
					
					
					else state = WIN;
					
					if(player_1->hp == 0) state = GAME_OVER;
						

		            al_clear_to_color(al_map_rgb(0, 0, 0));		//Substitui tudo que estava desenhado na tela por um fundo preto
		            
		            
		            al_draw_bitmap(background,0,0,0);                      
		            
		            //Variaveis usadas nos sprites
		            
					float player_Xposition = player_1->x - large_sprite;
					float player_Yposition = player_1->y - high_sprite;
					
					float bot1_Xposition = bot1->x - large_sprite;
					float bot1_Yposition = bot1->y - high_sprite;
					
					float bot2_Xposition = bot2->x - large_sprite;
					float bot2_Yposition = bot2->y - high_sprite;
					
					float bot3_Xposition = bot3->x - large_sprite;
					float bot3_Yposition = bot3->y - high_sprite;
					
					float bot4_Xposition = bot4->x - large_sprite;
					float bot4_Yposition = bot4->y - high_sprite;
					
					float bot5_Xposition = bot5->x - large_sprite;
					float bot5_Yposition = bot5->y - high_sprite;
					
					float bot6_Xposition = bot6->x - large_sprite;
					float bot6_Yposition = bot6->y - high_sprite;
					
					float boss_Xposition = boss->x - large_sprite_boss;
					float boss_Yposition = boss->y - high_sprite_boss;
					
					//al_draw_filled_rectangle(player_1->x-player_1->large/2, player_1->y-player_1->high/2, 
					//player_1->x+player_1->large/2, player_1->y+player_1->high/2, al_map_rgb(255, 0, 0));
					
					//Coloca sprite no tiro do player
					for (struct bullet *index = player_1->gun->shots; index != NULL; index = (struct bullet*) index->next){
						//al_draw_filled_circle(index->x, index->y, 10, al_map_rgb(255, 255, 0));	
						
						if(player_1->face == 0){
							al_draw_scaled_bitmap(
								SpriteBullet,
								0, 0,                    
								10, 3, 
								index->x - 10, index->y,      
								10 * scale, 3 * scale,   
								ALLEGRO_FLIP_HORIZONTAL
								);
						}
						
						else if(player_1->face == 1){
							al_draw_scaled_bitmap(
								SpriteBullet,
								0, 0,                    
								10, 3, 
								index->x - 10, index->y,      
								10 * scale, 3 * scale,   
								0
								);
						}
						else if (player_1->face == 2){	
							al_draw_scaled_rotated_bitmap(
		    					SpriteBullet,
		    					5, 1.5,               
		    					index->x, index->y,    
		   						scale, scale,   
		    					-ALLEGRO_PI / 2,0
							);
						}
						else if(player_1->face == 6){
							al_draw_scaled_rotated_bitmap(
		    					SpriteBullet,
		    					5, 1.5,               
		    					index->x, index->y,    
		   						scale, scale,   
		    					-ALLEGRO_PI / 4,0
							);
						}
						else if(player_1->face == 5){
							al_draw_scaled_rotated_bitmap(
		    					SpriteBullet,
		    					5, 1.5,               
		    					index->x, index->y,    
		   						scale, scale,   
		    					ALLEGRO_PI * 5 / 4,0
							);
						}
					}
					
					if (player_1->gun->timer) player_1->gun->timer--;
					
					/*if(bot1->hp != 0){
						al_draw_filled_rectangle(bot1->x - bot1->large/2, bot1->y - bot1->high/2, 
						bot1->x + bot1->large/2, bot1->y + bot1->high/2, al_map_rgb(0, 0, 255));
					}
					
					if(bot2->hp != 0){
						al_draw_filled_rectangle(bot2->x - bot2->large/2, bot2->y - bot2->high/2, 
						bot2->x + bot2->large/2, bot2->y + bot2->high/2, al_map_rgb(0, 0, 255));
					}
					
					if(bot3->hp != 0){
						al_draw_filled_rectangle(bot3->x - bot3->large/2, bot3->y - bot3->high/2, 
						bot3->x + bot3->large/2, bot3->y + bot3->high/2, al_map_rgb(0, 0, 255));
					}
					
					if(bot4->hp != 0){
						al_draw_filled_rectangle(bot4->x - bot4->large/2, bot4->y - bot4->high/2, 
						bot4->x + bot4->large/2, bot4->y + bot4->high/2, al_map_rgb(0, 0, 255));
					}
					
					if(bot5->hp != 0){
						al_draw_filled_rectangle(bot5->x - bot5->large/2, bot5->y - bot5->high/2, 
						bot5->x + bot5->large/2, bot5->y + bot5->high/2, al_map_rgb(0, 0, 255));
					}
					
					if(bot6->hp != 0){
						al_draw_filled_rectangle(bot6->x - bot6->large/2, bot6->y - bot6->high/2, 
						bot6->x + bot6->large/2, bot6->y + bot6->high/2, al_map_rgb(0, 0, 255));
					}
					
					if (boss->hp != 0 && (!bots_alive)) {
					al_draw_filled_rectangle(boss->x - boss->large/2, boss->y - boss->high/2, 
						boss->x + boss->large/2, boss->y + boss->high/2, al_map_rgb(0, 0, 255));
					}*/
					
					//Coloca sprite no tiro de cada um dos bots e do boss:
					
					if(bot1->hp != 0 && bot1->x > cameraPosition[0] && bot1->x < cameraPosition[0] + SCREEN_X){
						for (struct bullet *index = bot1->gun->shots; index != NULL; index = (struct bullet*) index->next){
							//al_draw_filled_circle(index->x, index->y-20, 10, al_map_rgb(0, 0, 0));	
							al_draw_scaled_bitmap(
								SpriteBulletBot,
								0, 0,                    
								10, 3, 
								index->x - 10, index->y-20,      
								10 * scale, 3 * scale,   
								0
								);	
						}
						
						if (bot1->gun->timer) bot1->gun->timer--;
						
						if (!bot1->gun->timer){							//Verifica se a arma do bot não está em cooldown (!)
							joystick_fire(bot1->control,1);
							entity_shot(bot1,10,BotShots);				//Se não estiver, faz um disparo (!)
							bot1->gun->timer = BOT_COOLDOWN;			//Inicia o cooldown da arma (!)
						} 
						if (player_Xposition < bot1_Xposition){
							bot1->face = 0;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire1/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot1_Xposition, bot1_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									ALLEGRO_FLIP_HORIZONTAL
						);
						}
						else{
							bot1->face = 1;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire1/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot1_Xposition, bot1_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									0
							);
						}
									bot_fire1++;
									if(bot_fire1 == 33)	bot_fire1 = 0;	
					}
					
					if(bot2->hp != 0 && bot2->x > cameraPosition[0] && bot2->x < cameraPosition[0] + SCREEN_X){
						for (struct bullet *index = bot2->gun->shots; index != NULL; index = (struct bullet*) index->next){
							//al_draw_filled_circle(index->x, index->y-20, 10, al_map_rgb(0, 0, 0));	
							al_draw_scaled_bitmap(
								SpriteBulletBot,
								0, 0,                    
								10, 3, 
								index->x - 10, index->y-20,      
								10 * scale, 3 * scale,   
								0
								);	
						}
						
						if (bot2->gun->timer) bot2->gun->timer--;
						
						if (!bot2->gun->timer){						//Verifica se a arma do bot não está em cooldown (!)
							joystick_fire(bot2->control,1);
							entity_shot(bot2,10,BotShots);			//Se não estiver, faz um disparo (!)
							bot2->gun->timer = BOT_COOLDOWN;			//Inicia o cooldown da arma (!)
						} 
						
						if (player_Xposition < bot2_Xposition){
							bot2->face = 0;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire2/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot2_Xposition, bot2_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									ALLEGRO_FLIP_HORIZONTAL
						);
						}
						else{
							bot2->face = 1;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire2/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot2_Xposition, bot2_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									0
							);
						}
									bot_fire2++;
									if(bot_fire2 == 33)	bot_fire2 = 0;	
					}
					
					if(bot3->hp != 0 && bot3->x > cameraPosition[0] && bot3->x < cameraPosition[0] + SCREEN_X){
						for (struct bullet *index = bot3->gun->shots; index != NULL; index = (struct bullet*) index->next){
							//al_draw_filled_circle(index->x, index->y-20, 10, al_map_rgb(0, 0, 0));	
							al_draw_scaled_bitmap(
								SpriteBulletBot,
								0, 0,                    
								10, 3, 
								index->x - 10, index->y-20,      
								10 * scale, 3 * scale,   
								0
								);	
						}
						
						if (bot3->gun->timer) bot3->gun->timer--;
						
						if (!bot3->gun->timer){						//Verifica se a arma do bot não está em cooldown (!)
							joystick_fire(bot3->control,1);
							entity_shot(bot3,10,BotShots);			//Se não estiver, faz um disparo (!)
							bot3->gun->timer = BOT_COOLDOWN;			//Inicia o cooldown da arma (!)
						} 

						if (player_Xposition < bot3_Xposition){
							bot3->face = 0;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire3/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot3_Xposition, bot3_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									ALLEGRO_FLIP_HORIZONTAL
						);
						}
						else{
							bot3->face = 1;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire3/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot3_Xposition, bot3_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									0
							);
						}
									bot_fire3++;
									if(bot_fire3 == 33)	bot_fire3 = 0;	
					}
					
					if(bot4->hp != 0 && bot4->x > cameraPosition[0] && bot4->x < cameraPosition[0] + SCREEN_X){
						for (struct bullet *index = bot4->gun->shots; index != NULL; index = (struct bullet*) index->next){
							//al_draw_filled_circle(index->x, index->y-20, 10, al_map_rgb(0, 0, 0));	
							al_draw_scaled_bitmap(
								SpriteBulletBot,
								0, 0,                    
								10, 3, 
								index->x - 10, index->y-20,      
								10 * scale, 3 * scale,   
								0
								);	
						}
						
						if (bot4->gun->timer) bot4->gun->timer--;
						
						if (!bot4->gun->timer){						//Verifica se a arma do bot não está em cooldown (!)
							joystick_fire(bot4->control,1);
							entity_shot(bot4,10,BotShots);			//Se não estiver, faz um disparo (!)
							bot4->gun->timer = BOT_COOLDOWN;			//Inicia o cooldown da arma (!)
						} 
						if (player_Xposition < bot4_Xposition){
							bot4->face = 0;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire4/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot4_Xposition, bot4_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									ALLEGRO_FLIP_HORIZONTAL
						);
						}
						else{
							bot4->face = 1;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire4/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot4_Xposition, bot4_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									0
							);
						}
									bot_fire4++;
									if(bot_fire4 == 33)	bot_fire4 = 0;	
					}
					
					if(bot5->hp != 0 && bot5->x > cameraPosition[0] && bot5->x < cameraPosition[0] + SCREEN_X){
						for (struct bullet *index = bot5->gun->shots; index != NULL; index = (struct bullet*) index->next){
							//al_draw_filled_circle(index->x, index->y-20, 10, al_map_rgb(0, 0, 0));	
							al_draw_scaled_bitmap(
								SpriteBulletBot,
								0, 0,                    
								10, 3, 
								index->x - 10, index->y-20,      
								10 * scale, 3 * scale,   
								0
								);	
						}
						
						if (bot5->gun->timer) bot5->gun->timer--;
						
						if (!bot5->gun->timer){						//Verifica se a arma do bot não está em cooldown (!)
							joystick_fire(bot5->control,1);
							entity_shot(bot5,10,BotShots);			//Se não estiver, faz um disparo (!)
							bot5->gun->timer = BOT_COOLDOWN;		//Inicia o cooldown da arma (!)
						} 
						if (player_Xposition < bot5_Xposition){
							bot5->face = 0;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire5/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot5_Xposition, bot5_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									ALLEGRO_FLIP_HORIZONTAL
						);
						}
						else{
							bot5->face = 1;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire5/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot5_Xposition, bot5_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									0
							);
						}
									bot_fire5++;
									if(bot_fire5 == 33)	bot_fire5 = 0;	
					}
					
					if(bot6->hp != 0 && bot6->x > cameraPosition[0] && bot6->x < cameraPosition[0] + SCREEN_X) {
						for (struct bullet *index = bot6->gun->shots; index != NULL; index = (struct bullet*) index->next){
							//al_draw_filled_circle(index->x, index->y-20, 10, al_map_rgb(0, 0, 0));	
							al_draw_scaled_bitmap(
								SpriteBulletBot,
								0, 0,                    
								10, 3, 
								index->x - 10, index->y-20,      
								10 * scale, 3 * scale,   
								0
								);	
						}
						
						if (bot6->gun->timer) bot6->gun->timer--;
						
						if (!bot6->gun->timer){						//Verifica se a arma do bot não está em cooldown (!)
							joystick_fire(bot6->control,1);
							entity_shot(bot6,10,BotShots);			//Se não estiver, faz um disparo (!)
							bot6->gun->timer = BOT_COOLDOWN;			//Inicia o cooldown da arma (!)
						} 

						if (player_Xposition < bot6_Xposition){
							bot6->face = 0;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire6/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot6_Xposition, bot6_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									ALLEGRO_FLIP_HORIZONTAL
						);
						}
						else{
							bot6->face = 1;
							al_draw_scaled_bitmap(
									SpriteBot,
									frames_x[bot_fire6/3],frames_y[5],                      
									large_sprite, high_sprite,
									bot6_Xposition, bot6_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									0
							);
						}
									bot_fire6++;
									if(bot_fire6 == 33)	bot_fire6 = 0;	
					}
					
					//Boss só aparece quando morre todos os bots
					if(boss->hp != 0 && (!bots_alive) && boss->x > cameraPosition[0] && boss->x < cameraPosition[0] + SCREEN_X + SCREEN_X/2)   {
						for (struct bullet *index = boss->gun->shots; index != NULL; index = (struct bullet*) index->next){
							//al_draw_filled_circle(index->x, index->y, 10, al_map_rgb(0, 0, 0));	
							if(boss->face != 3){
								al_draw_scaled_bitmap(
									SpriteBulletBoss,
									0, 0,                    
									13, 3, 
									index->x - 1, index->y - 1,      
									13 * 5, 3 * 5,   
									0
								);	
							}
							else{
								al_draw_scaled_rotated_bitmap(
		    						SpriteBulletBoss,
		    						13, 3,               
		    						index->x - 1, index->y - 1,    
		   							5, 5,   
		    						-ALLEGRO_PI / 2,0
		    					);
		    				}
						}
							
						al_draw_scaled_bitmap(
								SpriteBoss,
								0, 0,
								large_sprite_boss, high_sprite_boss,
								boss_Xposition,boss_Yposition,
								large_sprite_boss * scale,
								high_sprite_boss * scale,
								ALLEGRO_FLIP_HORIZONTAL	
						);
						
						
						if (boss->gun->timer) boss->gun->timer--;
						
						
						if (!boss->gun->timer){						//Verifica se a arma do boss não está em cooldown (!)
							joystick_fire(boss->control,1);
							entity_shot(boss,10,BossShots);			//Se não estiver, faz um disparo (!)
							boss->gun->timer = BOSS_COOLDOWN;		//Inicia o cooldown da arma (!)
						} 
					}
					
					
					//Faz a IA do boss usando controlboss como um cronometro
					if(!bots_alive){
						ControlBoss++;

						if(!Boss_AI(boss, ControlBoss,large_img))	ControlBoss = 0;
					}		
					
					//Desenha os sprites:
					
					if(crouched){
						al_draw_scaled_bitmap(
							SpritePlayer,
							frames_x[4], frames_y[9],                    
							large_sprite, high_sprite, 
							player_1->x - large_sprite,  player_1->y - high_sprite,      
							large_sprite * scale, high_sprite * scale,   
							0
						);
					}
					
					else{
						
						//Esta pulando ou caindo
						if((player_1->control->jumps > 0 || player_1->y < floor_size) && player_1->control->fire == 0){
							if(player_1->control->left == 1){														
								al_draw_scaled_bitmap(
								SpritePlayer,
								frames_x[3],frames_y[4],                      
								large_sprite, high_sprite,
								player_Xposition, player_Yposition,      
								large_sprite * scale, high_sprite * scale,   
								ALLEGRO_FLIP_HORIZONTAL
								);
							}
							else{
								al_draw_scaled_bitmap(
								SpritePlayer,
								frames_x[3],frames_y[4],                      
								large_sprite, high_sprite,
								player_Xposition, player_Yposition,      
								large_sprite * scale, high_sprite * scale,   
								0
								);
							}
						}
						
						//Esta mirando para cima
						else if(player_1->control->up == 1){
							if(player_1->control->right == 1){
								al_draw_scaled_bitmap(
									SpritePlayer,
									frames_x[frame_right],frames_y[7],                    
									large_sprite, high_sprite, 
									player_Xposition, player_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									0
									);
								frame_right++;
								if(frame_right == 10)	frame_right = 0;
							}
							else if(player_1->control->left == 1){
								al_draw_scaled_bitmap(
									SpritePlayer,
									frames_x[frame_left],frames_y[7],                    
									large_sprite, high_sprite, 
									player_Xposition, player_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									ALLEGRO_FLIP_HORIZONTAL
									);
								frame_left++;
								if(frame_left == 10)	frame_left = 0;
							}
							else{
								al_draw_scaled_bitmap(
									SpritePlayer,
									frames_x[1],frames_y[1],                      
									large_sprite, high_sprite,
									player_Xposition, player_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									0
									);
							}
						}
							
						//Atira parado(valido só para quando está parado e para quando está pulando ou caindo atirando)
						 else if(player_1->control->fire == 1 && player_1->control->right == player_1->control->left && 
						 (player_1->control->jumps == 0 || player_1->y < floor_size)){		
						 	if(player_1->face){		
								al_draw_scaled_bitmap(
								SpritePlayer,
								frames_x[frame_fire],frames_y[5],                      
								large_sprite, high_sprite,
								player_Xposition, player_Yposition,      
								large_sprite * scale, high_sprite * scale,   
								0
								);
							}
							else{
								al_draw_scaled_bitmap(
								SpritePlayer,
								frames_x[frame_fire],frames_y[5],                      
								large_sprite, high_sprite,
								player_Xposition, player_Yposition,      
								large_sprite * scale, high_sprite * scale,   
								ALLEGRO_FLIP_HORIZONTAL
								);
							}
							frame_fire++;
							if(frame_fire == 4)	frame_fire = 0;					
						}				
						
						//Esta parado fazendo a animação padrão
						else if((player_1->control->right == player_1->control->left) && player_1->control->jumps == 0  && player_1->y >= floor_size){		
							if (player_1->face){												
								al_draw_scaled_bitmap(
								SpritePlayer,
								frames_x[frame_Idle/33],frames_y[1],                 
								large_sprite, high_sprite, 
								player_Xposition, player_Yposition,      
								large_sprite * scale, high_sprite * scale,   
								0
							);
							}
							else{						
								al_draw_scaled_bitmap(
								SpritePlayer,
								frames_x[frame_Idle/33],frames_y[1],                 
								large_sprite, high_sprite, 
								player_Xposition, player_Yposition,      
								large_sprite * scale, high_sprite * scale,   
								ALLEGRO_FLIP_HORIZONTAL
								);
							}
							frame_Idle++;
							if(frame_Idle == 163)	frame_Idle = 0;
						}
						
						//esta andando
						else if(player_1->speed == ENTITY_WALK){
							 //Andando para direita sem estar caindo
							 if(player_1->y >= floor_size){
							 	if(player_1->control->right == 1){															
									al_draw_scaled_bitmap(
									SpritePlayer,
									frames_x[frame_right],frames_y[2],                    
									large_sprite, high_sprite, 
									player_Xposition, player_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									0
									);
									frame_right++;
									if(frame_right == 10)	frame_right = 0;
								}
							
								//Andando para esquerda sem estar caindo
								else if(player_1->control->left == 1){															
									al_draw_scaled_bitmap(
									SpritePlayer,
									frames_x[frame_left], frames_y[2],                    
									large_sprite, high_sprite, 
									player_Xposition, player_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									ALLEGRO_FLIP_HORIZONTAL
								);
									frame_left++;
									if(frame_left == 10)	frame_left = 0;
								}
							}
							else{
							 	if(player_1->control->right == 1){															
									al_draw_scaled_bitmap(
									SpritePlayer,
									frames_x[3],frames_y[5],                    
									large_sprite, high_sprite, 
									player_Xposition, player_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									0
									);
								}
							
								//Andando para esquerda sem estar caindo
								else if(player_1->control->left == 1){															
									al_draw_scaled_bitmap(
									SpritePlayer,
									frames_x[3], frames_y[5],                    
									large_sprite, high_sprite, 
									player_Xposition, player_Yposition,      
									large_sprite * scale, high_sprite * scale,   
									ALLEGRO_FLIP_HORIZONTAL
								);
								}
							}
							
						}
						
						//Esta na velocidade de correndo 
						else if(player_1->speed == ENTITY_RUN && player_1->y >= floor_size){
							//Correndo para a direita e não esta pulando ou caindo 
							if(player_1->control->right == 1 && player_1->control->jumps == 0 && player_1->y >= floor_size && player_1->control->fire == 0){															
								al_draw_scaled_bitmap(
								SpritePlayer,
								frames_x[frame_right],frames_y[3],                    
								large_sprite, high_sprite, 
								player_Xposition, player_Yposition,      
								large_sprite * scale, high_sprite * scale,   
								0
								);
								frame_right++;
								if(frame_right == 10)	frame_right = 0;
							}
						
							//Correndo para a esquerda e não esta pulando ou caindo
							else if(player_1->control->left == 1 && player_1->control->jumps == 0 && player_1->y >= floor_size && player_1->control->fire == 0){															
								al_draw_scaled_bitmap(
								SpritePlayer,
								frames_x[frame_left], frames_y[3],                    
								large_sprite, high_sprite, 
								player_Xposition, player_Yposition,      
								large_sprite * scale, high_sprite * scale,   
								ALLEGRO_FLIP_HORIZONTAL
								);
								frame_left++;
								if(frame_left == 10)	frame_left = 0;
							}
						}
				}
				
				//Lógica da vida extra				
				if(difficulty_select != 5){
					if(extralife == 0)	al_draw_bitmap(ExtraHeart, 1400, 200, 0);
				
					if(player_1->x > 1300 && player_1->x < 1500 && player_1->y > 200 && player_1->y < 250 && extralife != -1)	extralife = 1;
				
					if(extralife == 1){	
						player_1->hp = player_1->hp + 5;
						extralife = -1;
					}
				}
						
				//Coloca o contador de vida e a quantidade de inimigos restantes:      
				       
				al_identity_transform(&camera);
				al_use_transform(&camera);      
				
				float space = 40;


				for(int i = 0; i < player_1->hp; i++){
					al_draw_bitmap(SpriteLife,10 + ((i % 10) * 40),((i / 10) * 30), 0);
				}
				
				if(bots_alive){
					for(int i = 0; i < bots_alive; i++){
						al_draw_bitmap(remaining_enemies, 920, 10 + i * space, 0);
					}
				}
				else{
					for(int i = 0; i < boss->hp; i++){
						al_draw_bitmap(remaining_enemies,550 + ((i % 10) * 40),((i / 10) * 30), 0);
					}
				}
					
																	
				al_flip_display();
				
		    	}
		   
        	}     
        }   	
        
		if (event.type == ALLEGRO_EVENT_KEY_DOWN) { 	// Tecla FOI PRESSIONADA
			
			if (event.keyboard.keycode == ALLEGRO_KEY_DOWN && player_1->y >= floor_size){
				joystick_reset(player_1->control);
				player_1->high = high_crouched;
				player_1->y =  player_1->y + (player_high - high_crouched) / 2;
				crouched = 1;
			}
			
			if(event.keyboard.keycode == ALLEGRO_KEY_P)	pause = 1;
			
			if(event.keyboard.keycode == ALLEGRO_KEY_O)	pause = 0;
			

			
			if(event.keyboard.keycode == ALLEGRO_KEY_UP && crouched == 0) {			
				oldFace = player_1->face; 
				joystick_up(player_1->control,1);
			}
			
			else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && player_1->y >= floor_size && crouched == 0)	joystick_jumps(player_1->control,JUMP);
							
			else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT && crouched == 0)     	joystick_left(player_1->control,1);

			else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT && crouched == 0)    	joystick_right(player_1->control,1);
                    	
			if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)	break;
                                       
			if (event.keyboard.keycode == ALLEGRO_KEY_TAB && player_1->control->fire == 0 && crouched == 0)		update_speed(player_1,ENTITY_RUN);	
			
				if (event.keyboard.keycode == ALLEGRO_KEY_M && crouched == 0){	
					if (!player_1->gun->timer){							//Verifica se a arma do primeiro jogador não está em cooldown (!)
						update_speed(player_1,ENTITY_WALK);
						joystick_fire(player_1->control,1);
						entity_shot(player_1,BULLET_MOVE,numShots);		//Se não estiver, faz um disparo (!)
						player_1->gun->timer = ENTITY_COOLDOWN;			//Inicia o cooldown da arma (!)
					} 
				}	
        }    
        
        if (event.type == ALLEGRO_EVENT_KEY_UP) {				//Tecla foi SOLTA
        
        	if(event.keyboard.keycode == ALLEGRO_KEY_UP){	
        		joystick_up(player_1->control,0);
        		player_1->face = oldFace;
        	}
				
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)  joystick_left(player_1->control,0);
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) joystick_right(player_1->control,0);
			
			 else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN){    
			 	if(crouched){ 
				 	crouched = 0;
				 	player_1->high = player_high;
					player_1->y = player_1->y - (player_high - high_crouched) / 2;
				}
			}
			
			if (event.keyboard.keycode == ALLEGRO_KEY_M) 		joystick_fire(player_1->control,0);
			
			if (event.keyboard.keycode == ALLEGRO_KEY_TAB)			update_speed(player_1,ENTITY_WALK);

        }
        
        
        
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)  break;                      //Evento de clique no "X" de fechamento da tela. Encerra o programa graciosamente.
    
    }
    

    	

    al_destroy_font(font);                                                          //Destrutor da fonte
    al_destroy_font(title);                                                         //Destrutor da fonte
    al_destroy_font(EndGameFont);													//Destrutor da fonte
	al_destroy_audio_stream(soundtrack);											//Destrutor da música
    al_destroy_bitmap(MenuImage);                                                   //Destrutor da imagem carregada
    al_destroy_bitmap(EndGame);                                                   	//Destrutor da imagem carregada
    al_destroy_bitmap(background);                                                  //Destrutor da imagem carregada
    al_destroy_bitmap(SpritePlayer);												//Destrutor da imagem carregada
    al_destroy_bitmap(SpriteBot);													//Destrutor da imagem carregada	
    al_destroy_bitmap(SpriteBullet);												//Destrutor da imagem carregada
    al_destroy_bitmap(SpriteBulletBot);												//Destrutor da imagem carregada
    entity_destroy(player_1);														//Destrutor do personagem
    entity_destroy(bot1);															//Destrutor do personagem
    entity_destroy(bot2);															//Destrutor do personagem
    entity_destroy(bot3);															//Destrutor do personagem
    entity_destroy(bot4);															//Destrutor do personagem
    entity_destroy(bot5);															//Destrutor do personagem
    entity_destroy(bot6);															//Destrutor do personagem
    al_destroy_display(disp);                                                       //Destrutor da tela
    al_destroy_timer(timer);                                                        //Destrutor do relógio
    al_destroy_event_queue(queue);                                                  //Destrutor da fila

    return 0;
}
