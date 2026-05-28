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
#include <functional>

#include "gl_canvas2d.h"

#include "auxiliar.h" //precisa incluir para ter o prototipo da funcao funcaoDeOutroArquivo().


//int screenWidth = GetSystemMetrics(SM_CXSCREEN);
//int screenHeight = GetSystemMetrics(SM_CYSCREEN);

//int windowWidth = screenWidth*70/100;
//int windowHeight = screenHeight*70/100;

int windowWidth = 1000;
int windowHeight = 800;

int mouseX, mouseY, mouseButton, mouseState, mouseWheel, mouseDirection;
int pressedKey = NULL;

//bool menu = false;
bool menu = true;

int boardSize = 8;


void quitGame() {
	exit(0);
	return;
}


void setBoardSize(int size) {
	boardSize = size;
	menu = false;
}


void DrawButton(float x1, float y1, float width, const char* t, std::function<void()> callback) {
	float centerOffset = -(width / 2);
	x1 += centerOffset;
	float x2 = x1 + width;
	float y2 = y1 + 20;

	CV::button(x1, y1, x2, y2, t);

	if (mouseX > x1 && mouseY > y1 &&
		mouseX < x2 && mouseY < y2 &&
		mouseButton == 0 &&
		mouseState == 1
		) {
		callback();
	}
}


void DrawMenu() {
	if (!menu) {
		return;
	}

	CV::color(15);
	CV::centeredText(windowWidth / 2, windowHeight - 75, "Damas 2d");
	CV::centeredText(windowWidth / 2, windowHeight - 150, "Menu Principal");
	CV::centeredText(windowWidth / 2, windowHeight - 300, "Escolha o tamanho do tabuleiro");
	DrawButton(windowWidth / 2, windowHeight - 350, 200, "Tabuleiro 8x8", []() {setBoardSize(8);});
	DrawButton(windowWidth / 2, windowHeight - 400, 200, "Tabuleiro 10x10", []() {setBoardSize(10);});
	DrawButton(windowWidth / 2, windowHeight - 450, 200, "Tabuleiro 12x12", []() {setBoardSize(12);});
	DrawButton(windowWidth / 2, 100, 120, "Sair [ESC]", []() {quitGame();});
}


void DrawBoard() {
	if (menu) {
		return;
	}
}


void DrawAside() {
	if (menu) {
		return;
	}

	CV::color(15);
	CV::line(200, windowHeight, 200, 0);
}


void keyboard(int key)
{
	pressedKey = key;
	printf("\nTecla: %d", key);

	switch (key)
	{
	case 27:
		quitGame();
		break;
	}
}


void keyboardUp(int key)
{
	pressedKey = NULL;
	printf("\nLiberou tecla: %d", key);
}


void mouse(int button, int state, int wheel, int direction, int x, int y)
{
	printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction, x, y);

	mouseX = x;
	mouseY = y;
	mouseButton = button;
	mouseState = state;
	mouseWheel = wheel;
	mouseDirection = direction;
}


void render()
{
	CV::clear(0.957, 0.945, 0.918);

	DrawMenu();
	DrawBoard();
	DrawAside();
}

int main(void)
{
	CV::init(&windowWidth, &windowHeight, "`Damas 2D");
	CV::run();
}
