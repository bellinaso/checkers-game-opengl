/*********************************************************************
// Canvas para desenho, criada sobre a API OpenGL. Nao eh necessario conhecimentos de OpenGL para usar.
//  Autor: Cesar Tadeu Pozzer
//         02/2025
//
//  Pode ser utilizada para fazer desenhos, animacoes, e jogos simples.
//  Tem tratamento de mouse e teclado
//  Estude o OpenGL antes de tentar compreender o arquivo gl_canvas.cpp

//  Todos os arquivos do projeto devem ser .cpp. A programacao pode ser em C ou C++
//
//  Versao 2.1
//
// *********************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl_canvas2d.h"

#include "auxiliar.h" //precisa incluir para ter o prototipo da funcao funcaoDeOutroArquivo().


//variaveis globais
int posQuadradoX = 100;
int posQuadradoY = 100;
int screenWidth = 500, screenHeight = 500; //largura e altura inicial da tela. Alteram com o redimensionamento de tela.
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().



void DrawMouseScreenCoords()
{
    char str[100];
    sprintf(str, "Mouse: (%d,%d)", mouseX, mouseY);
    CV::text(10,300, str);
    sprintf(str, "Screen: (%d,%d)", screenWidth, screenHeight);
    CV::text(10,320, str);
}

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis
//globais que podem ser setadas pelo metodo keyboard()
void render()
{
    CV::clear(0,0,0);

    funcaoDeOutroArquivo();

    CV::color(1);

    DrawMouseScreenCoords();


    CV::color(2);
    //desenha o quadrado
    CV::rect(posQuadradoX, posQuadradoY, posQuadradoX+100, posQuadradoY+100);

    CV::circleFill(mouseX, mouseY, 40, 30);

    CV::color(3);
    CV::color(1, 0 , 0);
    CV::text(20,50,   "Programa Demo Canvas."); //imprime texto
    CV::text(40, 400, 88); //imprime numeros inteiros
}

void MoveQuadrado(int x, int y)
{
    posQuadradoX += x;
    posQuadradoY += y;
}

//funcao chamada toda vez que uma tecla for pressionada
void keyboard(int key)
{
   printf("\nTecla: %d" , key);

   switch(key)
   {
      case 27: //finaliza programa
	     exit(0);
        break;

	  case 201: //seta para cima
        MoveQuadrado(0, 10);
        break;

	  case 203: //seta para a baixo
        MoveQuadrado(0, -10);
        break;

	  case 202: //seta para direita
        MoveQuadrado(10, 0);
        break;

   }
}
//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
   printf("\nLiberou tecla: %d" , key);
}


//funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction,  x, y);

   mouseX = x; //guarda as coordenadas do mouse para exibir dentro da render()
   mouseY = y;
}

int main(void)
{
   CV::init(&screenWidth, &screenHeight, "Trabalho do Pozzer: Canvas 2D - Use as setas");
   CV::run();
}
