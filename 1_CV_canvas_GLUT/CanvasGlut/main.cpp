/*********************************************************************
// Canvas para desenho, criada sobre a API OpenGL. Nao eh necessario conhecimentos de OpenGL para usar.
//  Autor: Cesar Tadeu Pozzer
//         02/2025
//
//  Pode ser utilizada para fazer desenhos, animacoes, e jogos simples.
//  Tem tratamento de mouse e teclado
//  Estude o OpenGL antes de tentar compreender o arquivo gl_canvas.cpp
//
//  Todos os arquivos do projeto devem ser .cpp. A programacao pode ser em C ou C++
//
//  Versao 2.1
//
// *********************************************************************/

/*********************************************************************
// Trabalho 3 - Jogo de Damas 2D
// Aluno: Bruno Brasil
//
// Quesitos implementados:
// - Tabuleiro 8x8, 10x10 ou 12x12, com matriz alocada dinamicamente.
// - Desenho do tabuleiro e das pecas na Canvas2D.
// - Cursor controlado por teclado, sem sair do tabuleiro.
// - Selecao de peca com ESPACO e finalizacao da jogada com ESPACO.
// - Alternancia entre jogador 1 e jogador 2.
// - Movimentos diagonais validos, impedindo andar sobre peca propria/inimiga.
// - Captura por salto, removendo a peca capturada do tabuleiro.
// - Promocao para dama ao chegar ao lado oposto do tabuleiro.
// - Movimento tambem com mouse: clique na peca e depois clique no destino.
// - Lista de instrucoes desenhada na tela.
//
// Nao foram implementados: tempo de jogada e animacao de captura.
// *********************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <stdio.h>
#include <stdlib.h>
#include <functional>

#include "gl_canvas2d.h"

int windowWidth = 1200;
int windowHeight = 800;

int asideWidth = (windowWidth - windowHeight);

int mouseX = -1, mouseY = -1, mouseButton = -1, mouseState = -1, mouseWheel, mouseDirection;
int pressedKey = NULL;

bool menu = true;

int boardPieces = 8;

int** player1pieces = NULL;
int** player2pieces = NULL;
int** selectionPiece = NULL;

bool selectedPiece = false;

int turnOfPplayer = 2;

int cursorRow = 0;
int cursorCol = 0;
int selectedRow = -1;
int selectedCol = -1;

int player1Captured = 0;
int player2Captured = 0;
int winner = 0;


void clearSelectionPiece();
void updateSelectionPiece();
void selectPiece(int i, int j);
void cancelSelection();
void handleSpaceKey();


bool isInsideBoard(int i, int j) {
	return (
		i >= 0 && i < boardPieces &&
		j >= 0 && j < boardPieces
		);
}


int** getCurrentPlayerPieces() {
	if (turnOfPplayer == 1) {
		return player1pieces;
	}
	return player2pieces;
}


int** getEnemyPlayerPieces() {
	if (turnOfPplayer == 1) {
		return player2pieces;
	}
	return player1pieces;
}


int getPlayerDirection(int player) {
	if (player == 1) {
		return -1;
	}
	return 1;
}


void freeBoard() {
	if (player1pieces != NULL) {
		for (int i = 0; i < boardPieces; i++) {
			free(player1pieces[i]);
		}
		free(player1pieces);
		player1pieces = NULL;
	}

	if (player2pieces != NULL) {
		for (int i = 0; i < boardPieces; i++) {
			free(player2pieces[i]);
		}
		free(player2pieces);
		player2pieces = NULL;
	}

	if (selectionPiece != NULL) {
		for (int i = 0; i < boardPieces; i++) {
			free(selectionPiece[i]);
		}
		free(selectionPiece);
		selectionPiece = NULL;
	}
}


int** createMatrix(int size) {
	int** matrix = (int**)malloc(size * sizeof(int*));

	for (int i = 0; i < size; i++) {
		matrix[i] = (int*)malloc(size * sizeof(int));
	}

	return matrix;
}


void quitGame() {
	if (!menu) {
		menu = true;
		selectedPiece = false;
		return;
	}

	freeBoard();
	exit(0);
}


void setBoardPieces(int pieces) {
	freeBoard();

	boardPieces = pieces;
	menu = false;
	selectedPiece = false;
	turnOfPplayer = 2;
	cursorRow = 0;
	cursorCol = 0;
	selectedRow = -1;
	selectedCol = -1;
	player1Captured = 0;
	player2Captured = 0;
	winner = 0;

	player1pieces = createMatrix(boardPieces);
	player2pieces = createMatrix(boardPieces);
	selectionPiece = createMatrix(boardPieces);

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

	updateSelectionPiece();
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


void clearSelectionPiece() {
	if (selectionPiece == NULL) {
		return;
	}

	for (int i = 0; i < boardPieces; i++) {
		for (int j = 0; j < boardPieces; j++) {
			selectionPiece[i][j] = 0;
		}
	}
}


bool isValidPiece(int i, int j) {
	if (!isInsideBoard(i, j)) {
		return false;
	}

	if (turnOfPplayer == 2 && player2pieces[i][j] > 0) {
		return true;
	}
	if (turnOfPplayer == 1 && player1pieces[i][j] > 0) {
		return true;
	}
	return false;
}


bool hasOwnPiece(int i, int j) {
	if (!isInsideBoard(i, j)) {
		return false;
	}

	if (turnOfPplayer == 1 && player1pieces[i][j] > 0) {
		return true;
	}
	if (turnOfPplayer == 2 && player2pieces[i][j] > 0) {
		return true;
	}

	return false;
}


bool hasEnemyPiece(int i, int j) {
	if (!isInsideBoard(i, j)) {
		return false;
	}

	if (turnOfPplayer == 1 && player2pieces[i][j] > 0) {
		return true;
	}
	if (turnOfPplayer == 2 && player1pieces[i][j] > 0) {
		return true;
	}

	return false;
}


bool isEmptyTile(int i, int j) {
	if (!isInsideBoard(i, j)) {
		return false;
	}

	return (
		player1pieces[i][j] == 0 &&
		player2pieces[i][j] == 0
		);
}


bool isCurrentPieceKing(int i, int j) {
	if (!isValidPiece(i, j)) {
		return false;
	}

	int** currentPieces = getCurrentPlayerPieces();
	return currentPieces[i][j] == 2;
}


bool isPossibleMove(int fromI, int fromJ, int toI, int toJ) {
	if (!isValidPiece(fromI, fromJ) || !isEmptyTile(toI, toJ)) {
		return false;
	}

	int rowDifference = toI - fromI;
	int colDifference = toJ - fromJ;
	int absRowDifference = abs(rowDifference);
	int absColDifference = abs(colDifference);
	int direction = getPlayerDirection(turnOfPplayer);
	bool king = isCurrentPieceKing(fromI, fromJ);

	if (!king && rowDifference != direction && rowDifference != direction * 2) {
		return false;
	}

	if (absRowDifference == 1 && absColDifference == 1) {
		return true;
	}

	if (absRowDifference == 2 && absColDifference == 2) {
		int middleRow = (fromI + toI) / 2;
		int middleCol = (fromJ + toJ) / 2;

		return hasEnemyPiece(middleRow, middleCol);
	}

	return false;
}


void markPossibleMoves(int i, int j) {
	if (!isValidPiece(i, j)) {
		return;
	}

	int directions[4][2] = {
		{ 1,  1 },
		{ 1, -1 },
		{ -1,  1 },
		{ -1, -1 }
	};

	bool king = isCurrentPieceKing(i, j);
	int direction = getPlayerDirection(turnOfPplayer);

	for (int k = 0; k < 4; k++) {
		int rowDirection = directions[k][0];
		int colDirection = directions[k][1];

		if (!king && rowDirection != direction) {
			continue;
		}

		int simpleRow = i + rowDirection;
		int simpleCol = j + colDirection;

		if (isInsideBoard(simpleRow, simpleCol)) {
			if (isEmptyTile(simpleRow, simpleCol)) {
				selectionPiece[simpleRow][simpleCol] = 2;
			}
			else if (hasEnemyPiece(simpleRow, simpleCol)) {
				int jumpRow = i + (rowDirection * 2);
				int jumpCol = j + (colDirection * 2);

				if (isInsideBoard(jumpRow, jumpCol) && isEmptyTile(jumpRow, jumpCol)) {
					selectionPiece[jumpRow][jumpCol] = 2;
				}
			}
		}
	}
}


void updateSelectionPiece() {
	clearSelectionPiece();

	if (selectedPiece && isInsideBoard(selectedRow, selectedCol)) {
		markPossibleMoves(selectedRow, selectedCol);
	}

	if (isInsideBoard(cursorRow, cursorCol)) {
		selectionPiece[cursorRow][cursorCol] = 1;
	}
}


void moveSelectionPiece(int i, int j) {
	if (!isInsideBoard(i, j)) {
		return;
	}

	cursorRow = i;
	cursorCol = j;
	updateSelectionPiece();
}


void cancelSelection() {
	selectedPiece = false;
	selectedRow = -1;
	selectedCol = -1;
	updateSelectionPiece();
}


void selectPiece(int i, int j) {
	if (!isValidPiece(i, j)) {
		return;
	}

	selectedPiece = true;
	selectedRow = i;
	selectedCol = j;
	cursorRow = i;
	cursorCol = j;
	updateSelectionPiece();
}


int countPlayerPieces(int player) {
	int count = 0;
	int** pieces = player == 1 ? player1pieces : player2pieces;

	for (int i = 0; i < boardPieces; i++) {
		for (int j = 0; j < boardPieces; j++) {
			if (pieces[i][j] > 0) {
				count++;
			}
		}
	}

	return count;
}


void checkWinner() {
	if (countPlayerPieces(1) == 0) {
		winner = 2;
	}
	else if (countPlayerPieces(2) == 0) {
		winner = 1;
	}
}


void switchPlayer() {
	if (turnOfPplayer == 1) {
		turnOfPplayer = 2;
	}
	else {
		turnOfPplayer = 1;
	}
}


void performMove(int toI, int toJ) {
	if (!selectedPiece || !isPossibleMove(selectedRow, selectedCol, toI, toJ)) {
		return;
	}

	int** currentPieces = getCurrentPlayerPieces();
	int** enemyPieces = getEnemyPlayerPieces();
	int pieceValue = currentPieces[selectedRow][selectedCol];
	int rowDifference = toI - selectedRow;
	int colDifference = toJ - selectedCol;

	currentPieces[selectedRow][selectedCol] = 0;
	currentPieces[toI][toJ] = pieceValue;

	if (abs(rowDifference) == 2 && abs(colDifference) == 2) {
		int capturedRow = (selectedRow + toI) / 2;
		int capturedCol = (selectedCol + toJ) / 2;

		enemyPieces[capturedRow][capturedCol] = 0;

		if (turnOfPplayer == 1) {
			player1Captured++;
		}
		else {
			player2Captured++;
		}
	}

	if (turnOfPplayer == 1 && toI == 0) {
		currentPieces[toI][toJ] = 2;
	}
	else if (turnOfPplayer == 2 && toI == boardPieces - 1) {
		currentPieces[toI][toJ] = 2;
	}

	cursorRow = toI;
	cursorCol = toJ;
	selectedPiece = false;
	selectedRow = -1;
	selectedCol = -1;

	checkWinner();

	if (winner == 0) {
		switchPlayer();
	}

	updateSelectionPiece();
}


void handleTileClick(int i, int j) {
	if (winner != 0) {
		return;
	}

	moveSelectionPiece(i, j);

	if (!selectedPiece) {
		if (isValidPiece(i, j)) {
			selectPiece(i, j);
		}
		return;
	}

	if (i == selectedRow && j == selectedCol) {
		cancelSelection();
		return;
	}

	if (isPossibleMove(selectedRow, selectedCol, i, j)) {
		performMove(i, j);
	}
	else if (isValidPiece(i, j)) {
		selectPiece(i, j);
	}
	else {
		cancelSelection();
	}
}


void handleSpaceKey() {
	if (winner != 0) {
		return;
	}

	if (!selectedPiece) {
		if (isValidPiece(cursorRow, cursorCol)) {
			selectPiece(cursorRow, cursorCol);
		}
		return;
	}

	if (cursorRow == selectedRow && cursorCol == selectedCol) {
		cancelSelection();
		return;
	}

	if (isPossibleMove(selectedRow, selectedCol, cursorRow, cursorCol)) {
		performMove(cursorRow, cursorCol);
	}
	else if (isValidPiece(cursorRow, cursorCol)) {
		selectPiece(cursorRow, cursorCol);
	}
}


void handleKeyboard() {
	if (pressedKey == NULL) {
		return;
	}

	switch (pressedKey) {
	case 119: case 201: // W ou seta para cima
		moveSelectionPiece(cursorRow + 1, cursorCol);
		break;
	case 97: case 200: // A ou seta para esquerda
		moveSelectionPiece(cursorRow, cursorCol - 1);
		break;
	case 100: case 202: // D ou seta para direita
		moveSelectionPiece(cursorRow, cursorCol + 1);
		break;
	case 115: case 203: // S ou seta para baixo
		moveSelectionPiece(cursorRow - 1, cursorCol);
		break;
	case 32: // ESPACO
		handleSpaceKey();
		break;
	case 27: // ESC
		quitGame();
		break;
	}

	pressedKey = NULL;
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
			mouseState == 0
			) ||
		(
			pressedKey == activationKey
			)
		) {
		callback();
		pressedKey = NULL;
		mouseState = -2;
	}
}


void DrawMenu() {
	if (!menu) {
		return;
	}

	CV::color(15);
	CV::centeredText(windowWidth / 2, windowHeight - 75, "Damas 2D");
	CV::centeredText(windowWidth / 2, windowHeight - 150, "Menu Principal");
	CV::centeredText(windowWidth / 2, windowHeight - 300, "Escolha o tamanho do tabuleiro");
	DrawButton(windowWidth / 2, windowHeight - 350, 200, "Tabuleiro 8x8 [1]", 49, []() {setBoardPieces(8);});
	DrawButton(windowWidth / 2, windowHeight - 400, 200, "Tabuleiro 10x10 [2]", 50, []() {setBoardPieces(10);});
	DrawButton(windowWidth / 2, windowHeight - 450, 200, "Tabuleiro 12x12 [3]", 51, []() {setBoardPieces(12);});
	DrawButton(windowWidth / 2, 100, 120, "Sair [ESC]", 27, []() {quitGame();});
}


void DrawPiece(int tileX, int tileY, int tileSize, int pieceSize, int player, int pieceType) {
	if (player == 1) {
		CV::color(18);
		CV::circleFill((tileX + (tileSize / 2)), (tileY + (tileSize / 2)), (pieceSize / 2), 360);
		CV::color(19);
		CV::circleFill((tileX + (tileSize / 2)), (tileY + (tileSize / 2)), (pieceSize / 2) - (pieceSize / 2 * 17 / 100), 360);
	}
	else {
		CV::color(19);
		CV::circleFill((tileX + (tileSize / 2)), (tileY + (tileSize / 2)), (pieceSize / 2), 360);
		CV::color(18);
		CV::circleFill((tileX + (tileSize / 2)), (tileY + (tileSize / 2)), (pieceSize / 2) - (pieceSize / 2 * 17 / 100), 360);
	}

	if (pieceType == 2) {
		CV::color(15);
		CV::centeredText(tileX + (tileSize / 2), tileY + (tileSize / 2) - 5, "D");
	}
}


void DrawBoard() {
	if (menu) {
		return;
	}

	handleKeyboard();

	int boardSpace = (windowWidth - asideWidth);
	int smallerAxisSize = boardSpace >= windowHeight ? windowHeight : boardSpace;
	int boardSize = (smallerAxisSize * 75 / 100);
	int tileSize = boardSize / boardPieces;
	int pieceSize = tileSize - (tileSize * 10 / 100);

	int x1 = (asideWidth + ((boardSpace - boardSize) / 2));
	int x2 = (x1 + boardSize);
	int y1 = (windowHeight / 2 - ((x2 - x1) / 2));

	int tileY = y1;

	for (int i = 0; i < boardPieces; i++) {
		int tileX = x1;

		for (int j = 0; j < boardPieces; j++) {
			if (selectionPiece[i][j] == 0) {
				if ((i + j) % 2 == 0) {
					CV::color(16);
				}
				else {
					CV::color(17);
				}
			}
			else if (selectionPiece[i][j] == 1) {
				CV::color(20);
			}
			else if (selectionPiece[i][j] == 2) {
				CV::color(21);
			}

			CV::rectFill(tileX, tileY, (tileX + tileSize), (tileY + tileSize));

			if (mouseIn(tileX, tileY, (tileX + tileSize), (tileY + tileSize))) {
				moveSelectionPiece(i, j);

				if (mouseButton == 0 && mouseState == 0) {
					handleTileClick(i, j);
					mouseState = -2;
				}
			}

			if (player1pieces[i][j] > 0) {
				DrawPiece(tileX, tileY, tileSize, pieceSize, 1, player1pieces[i][j]);
			}
			else if (player2pieces[i][j] > 0) {
				DrawPiece(tileX, tileY, tileSize, pieceSize, 2, player2pieces[i][j]);
			}

			tileX += tileSize;
		}

		tileY += tileSize;
	}
}


void DrawAside() {
	if (menu) {
		return;
	}

	CV::color(15);
	CV::line(asideWidth, windowHeight, asideWidth, 0);
	CV::centeredText(asideWidth / 2, windowHeight - 40, "Damas 2D");
	CV::centeredText(asideWidth / 2, windowHeight - 70, "Bruno Brasil");

	char text[80];

	if (winner == 0) {
		sprintf(text, "Vez do jogador %d", turnOfPplayer);
	}
	else {
		sprintf(text, "Jogador %d venceu!", winner);
	}

	CV::centeredText(asideWidth / 2, windowHeight - 120, text);

	sprintf(text, "J1 capturou: %d", player1Captured);
	CV::text(20, windowHeight - 170, text);

	sprintf(text, "J2 capturou: %d", player2Captured);
	CV::text(20, windowHeight - 195, text);

	CV::text(20, windowHeight - 250, "Instrucoes:");
	CV::text(20, windowHeight - 280, "WASD/setas: mover cursor");
	CV::text(20, windowHeight - 305, "Espaco: selecionar/mover");
	CV::text(20, windowHeight - 330, "Mouse: clique peca/destino");
	CV::text(20, windowHeight - 355, "ESC: menu/sair");
	CV::text(20, windowHeight - 380, "D = dama");

	DrawButton((asideWidth / 2), 100, 140, "Menu [ESC]", 27, []() {quitGame();});
}


void keyboard(int key)
{
	pressedKey = key;
}


void keyboardUp(int key)
{
	if (pressedKey == key) {
		pressedKey = NULL;
	}
}


void mouse(int button, int state, int wheel, int direction, int x, int y)
{
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
	CV::init(&windowWidth, &windowHeight, "Bruno Brasil - Damas 2D");
	CV::run();
}
