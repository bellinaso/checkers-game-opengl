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

int asideWidth = (windowWidth - windowHeight);

int mouseX, mouseY, mouseButton, mouseState, mouseWheel, mouseDirection;
int pressedKey = NULL;

//bool menu = false;
bool menu = true;

int boardPieces = 8;

int** player1pieces = NULL;
int** player2pieces = NULL;
int** selectionPiece = NULL;


void quitGame() {
	if (!menu) {
		menu = true;
		return;
	}
	exit(0);
	return;
}


void setBoardPieces(int pieces) {
	boardPieces = pieces;
	menu = false;

	player1pieces = (int**)malloc(boardPieces * sizeof(int*));
	player2pieces = (int**)malloc(boardPieces * sizeof(int*));
	selectionPiece = (int**)malloc(boardPieces * sizeof(int*));

	for (int i = 0; i < boardPieces; i++) {
		player1pieces[i] = (int*)malloc(boardPieces * sizeof(int));
		player2pieces[i] = (int*)malloc(boardPieces * sizeof(int));
		selectionPiece[i] = (int*)malloc(boardPieces * sizeof(int));
	}

	for (int i = 0; i < boardPieces; i++) {
		for (int j = 0; j < boardPieces; j++) {
			player1pieces[i][j] = 0;
			player2pieces[i][j] = 0;
			selectionPiece[i][j] = 0;

			if ((i + j) % 2 == 0) {
				if (i < 3) {
					player2pieces[i][j] = 1;
				}

				else if (i >= boardPieces - 3) {
					player1pieces[i][j] = 1;
				}
			}
		}
	}

	selectionPiece[0][0] = 1;
}


bool mouseIn(float x1, float y1, float x2, float y2) {
	if (
		mouseX > x1 && mouseX < x2 &&
		mouseY > y1 && mouseY < y2
		) {
		return true;
	}
	return false;
}


void moveSelectionPiece(int x, int y) {
	for (int i = 0; i < boardPieces; i++) {
		for (int j = 0; j < boardPieces; j++) {
			selectionPiece[i][j] = 0;
		}
	}

	selectionPiece[x][y] = 1;
}


void DrawButton(float x1, float y1, float width, const char* t, int activationKey, std::function<void()> callback) {
	float centerOffset = -(width / 2);
	x1 += centerOffset;
	float x2 = x1 + width;
	float y2 = y1 + 20;

	CV::button(x1, y1, x2, y2, t);

	if (
		(
			mouseIn(x1, y1, x2, y2) &&
			mouseButton == 0 &&
			(mouseState == 0 ||
				mouseState == 1)
			) ||
		(
			pressedKey == activationKey
			)
		) {
		callback();
		pressedKey = NULL; // Evitar doubleclicks
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
	DrawButton(windowWidth / 2, windowHeight - 350, 200, "Tabuleiro 8x8 [1]", 49, []() {setBoardPieces(8);});
	DrawButton(windowWidth / 2, windowHeight - 400, 200, "Tabuleiro 10x10 [2]", 50, []() {setBoardPieces(10);});
	DrawButton(windowWidth / 2, windowHeight - 450, 200, "Tabuleiro 12x12 [3]", 51, []() {setBoardPieces(12);});
	DrawButton(windowWidth / 2, 100, 120, "Sair [ESC]", 27, []() {quitGame();});
}


void DrawBoard() {
	if (menu) {
		return;
	}

	int boardSpace = (windowWidth - asideWidth);

	int smallerAxisSize = boardSpace >= windowHeight ? windowHeight : boardSpace;

	int boardSize = (smallerAxisSize * 75 / 100);

	int pieceSize = boardSize / boardPieces;

	int x1 = (asideWidth + ((boardSpace - boardSize) / 2));
	int x2 = (x1 + boardSize);
	int y1 = (windowHeight / 2 - ((x2 - x1) / 2));
	int y2 = (y1 + boardSize);

	int pieceY = y1;

	for (int i = 0; i < boardPieces; i++) {

		int pieceX = x1;
		for (int j = 0; j < boardPieces; j++) {

			if (selectionPiece[i][j] != 1) {
				if ((i + j) % 2 == 0) {
					CV::color(16);
				}
				else {
					CV::color(17);
				}
			}
			else {
				CV::color(20);

				if (pressedKey) {
					switch (pressedKey) {
						case 119: case 201: // up
							if (i + 1 < boardPieces) {
								moveSelectionPiece(i + 1, j);
							}
							break;
						case 97: case 200: // left
							if (j - 1 < boardPieces && j - 1 >= 0) {
								moveSelectionPiece(i, j - 1);
							}
							break;
						case 100: case 202: // right
							if (j + 1 < boardPieces) {
								moveSelectionPiece(i, j + 1);
							}
							break;
						case 115: case 203: // down
							if (i - 1 < boardPieces && i - 1 >= 0) {
								moveSelectionPiece(i - 1, j);
							}
							break;
					}
					pressedKey = NULL;
				}

			}
			CV::rectFill(pieceX, pieceY, (pieceX + pieceSize), (pieceY + pieceSize));

			if (mouseIn(pieceX, pieceY, (pieceX + pieceSize), (pieceY + pieceSize))) {
				moveSelectionPiece(i, j);
			}

			if (player1pieces[i][j] == 1) {
				CV::color(18);
				CV::circleFill((pieceX + (pieceSize / 2)), (pieceY + (pieceSize / 2)), (pieceSize / 2), 360);
			}
			else if (player2pieces[i][j] == 1) {
				CV::color(19);
				CV::circleFill((pieceX + (pieceSize / 2)), (pieceY + (pieceSize / 2)), (pieceSize / 2), 360);
			}
			pieceX += pieceSize;
		}
		pieceY += pieceSize;
	}
}


void DrawAside() {
	if (menu) {
		return;
	}

	CV::color(15);
	CV::line(asideWidth, windowHeight, asideWidth, 0);
	CV::centeredText(100, windowHeight - 40, "Damas 2D");
	CV::centeredText(100, windowHeight - 70, "Tabuleiro");
	DrawButton((asideWidth / 2), 100, 140, "Menu [ESC]", 27, []() {quitGame();});
}


void keyboard(int key)
{
	pressedKey = key;
	printf("\nTecla: %d", key);
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
	DrawAside();
	DrawBoard();
}

int main(void)
{
	CV::init(&windowWidth, &windowHeight, "`Damas 2D");
	CV::run();
}
