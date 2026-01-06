Jogo/Simulador em C (Allegro 5)

Este é o projeto de um jogo single player no estilo RunAnGun(exemplo de jogos RunAndGun: Contra e MetalSlug)

Tecnologias Utilizadas

Linguagem: C

Biblioteca Gráfica: Allegro 5.2 (Addons: Font, TTF, Image, Primitives)

Ferramenta de Build: GCC / Pkg-config

Ambiente: Linux (Ubuntu 22.04)

Funcionalidades Principais:

Sistema de Estados: Transição fluida entre Menu Principal e tela de Jogo via entrada de teclado.

Menu Interativo: Seleção visual de opções com feedback de cores e navegação por setas.

Menu de Opções: É possível alterara a dificuldade do jogo e o volume da música

Gameplay: O personagem principal possui as funcionalidades de andar para todas as direções , pular (para todas as direções enquanto anda ou parado) , atirar para todas as direções enquanto anda ou parado), 
agachar e correr 

Atributos: O personagem possui 40 de vida , perdendo um de vida por tiro , o tiro do personagem também tira um de vida e os vilões possuem:

5 de vida para a dificuldade fácil 

15 de vida para a dificuldade média 

25 de vida para a dificuldade difícil 

O Chefão possui 40 de vida e só aparece quando os 6 inimigos são derrotados

O cenário possui rolling background (o cenário muda conforme o jogador se movimenta)

Funcionalidade extras:

Nas dificuldades fácil e médio há um coração de vida extra para o jogador coletar

Existem espinhos no mapa que se o jogador pisar é eliminado 

Os tiros dão um knockback no personagem(ao ser atingido ele vai levemente pra trás)

Como jogar:

 SETA PRA DIREITA: ANDA PRA DIREITA
 
 SETA PRA ESQUERDA: ANDA PRA ESQUERDA 
 
 SETA PRA BAIXO: AGACHA
 
 SETA PRA CIMA: MIRA PRA CIMA(se andar o tiro vai na diagonal da direção)
 
 BARRA DE ESPAÇO: PULA(se pegar a pena tem pulo duplo)
 
 TECLA TAB: CORRE
 
 TECLA M: ATIRA
 
 TECLA P: PAUSA
 
 TECLA O:DESPAUSA
 
 TECLA ESC: SAI DO JOGO


Como Executar

Certifique-se de ter as bibliotecas do Allegro 5 instaladas.

Clone o repositório e compile usando o comando:

make 

Execute o binário: ./jogo

Prints

<img width="955" height="534" alt="image" src="https://github.com/user-attachments/assets/9ba4b029-7ea3-42be-9d71-5d97df6128a8" />
