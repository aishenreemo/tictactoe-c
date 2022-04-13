#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum { N = 0, X = 1, O = 2 } Symbol;
typedef enum { PLAYER = 1, OPPONENT = 2 } Player;

char get_symbol(Symbol sym, int i) {
	switch (sym) {
		
	case X: return 'X';
	case O: return 'O';
	default: return '1' + i;
	
	}
}

struct Board {
	Symbol arr[9];
	Player turn;
	Symbol user;
	Symbol opponent;
};

void display_board(struct Board* board) {
	for (int i = 0; i < 9; i += 3) {
		printf("%c | %c | %c\n",
			get_symbol(board->arr[i], i),
			get_symbol(board->arr[i + 1], i + 1),
			get_symbol(board->arr[i + 2], i + 2));
	}
}

void display_turn(struct Board* board) {
	if (board->turn == PLAYER) {
		printf("Player's Turn.\n");
	} else if (board->turn == OPPONENT) {
		printf("Opponent's Turn.\n");
	}
}

void player_move(struct Board* board) {
	int move;
	
	while (1) {
		printf("Enter move [1-9]: ");
		scanf("%d", &move);
		printf("\n");

		if (move > 9 || move < 1) {
			continue;
		}

		if (board->arr[move - 1] != N) {
			continue;
		}

		break;
	}

	board->arr[move - 1] = board->user;
}

int is_draw(struct Board* board) {
	for (int i = 0; i < 9; i++) {
		if (board->arr[i] == N) {
			return 0;
		}
	}
	return 1;
}

int is_winner(struct Board* board, Symbol player) {
	int wins[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
	for(int i = 0; i < 8; ++i) {
		if(board->arr[wins[i][0]] == player &&
			board->arr[wins[i][1]] == player && 
			board->arr[wins[i][2]] == player)
		{
			return 1;
		}
	}
	return 0;
}

int* available_moves(struct Board* board, int* length) {
	for (int i = 0; i < 9; i++) {
		if (board->arr[i] == N) {
			*length = *length + 1;
		}
	}

	int acc = 0;
	int* arr = malloc((*length) * sizeof(int));
	for (int i = 0; i < 9; i++) {
		if (board->arr[i] == N) {
			arr[acc++] = i + 1;
		}
	}

	return arr;
}

int minimax(struct Board* board, int index, int player, int depth) {
	int other_player = player == board->opponent ? board->user : board->opponent;

	// if it blocked the cell that makes the other player win
	board->arr[index] = other_player;
	if (is_winner(board, other_player)) {
		board->arr[index] = N;
		return 3 * depth;
	}

	// if it wins in this move
	board->arr[index] = player;
	if (is_winner(board, player)) {
		board->arr[index] = N;
		return 10 * depth;
	}

	if (depth == 0 || is_draw(board)) {
		board->arr[index] = N;
		return 0;
	}

	int length = 0;
	int* avail_moves = available_moves(board, &length);
	if (player == board->opponent) {
		int score = -100;
		for(int i = 0; i < length; i++) {
			int curr = *(avail_moves + i) - 1;
			int evaluated_score = minimax(board, curr, board->user, depth - 1);
			score = (evaluated_score > score) ? evaluated_score : score;
		}
		board->arr[index] = N;
		free(avail_moves);
		return score;
	} else {
		int score = 100;
		if (length > 6) {
			board->arr[index] = N;
			free(avail_moves);
			return rand() % 10;
		}
		for (int i = 0; i < length; i++) {
			int curr = *(avail_moves + 1) - 1;
			int evaluated_score = minimax(board, curr, board->opponent, depth - 1);
			score = (evaluated_score < score) ? evaluated_score : score;
		}
		board->arr[index] = N;
		free(avail_moves);
		return score;
	}
}

void computer_move(struct Board* board) {
	int length = 0;
	int* avail_moves = available_moves(board, &length);
	int score = -100;
	int move = *avail_moves - 1;
	for (int i = 0; i < length; i++) {
		int curr = *(avail_moves + i) - 1;
		int evaluated_score = minimax(board, curr, board->user, 6);
		if (evaluated_score > score) {
			score = evaluated_score;
			move = curr;
		}
	}
	free(avail_moves);
	board->arr[move] = board->opponent;
}

struct Board initialize_board() {
	struct Board board;
	
	for (int i = 0; i < 9; i++) {
		board.arr[i] = N;
	}

	srand((unsigned int) time(NULL));
	board.turn = (rand() % 2) + 1;
	board.user = (rand() % 2) + 1;
	board.opponent = (board.user == O) ? X : O;

	printf("Computer: %c, Player: %c. \n", 
		get_symbol(board.opponent, 0), 
		get_symbol(board.user, 0));

	display_board(&board);
	display_turn(&board);
	
	return board;
};

int main() {
	struct Board board = initialize_board();

	while (1) {
		switch(board.turn) {
		case PLAYER: 
			player_move(&board);
			board.turn = OPPONENT;
			break;
		case OPPONENT: 
			computer_move(&board);
			board.turn = PLAYER;
			break;
		}

		display_board(&board);
		if (is_winner(&board, board.user)) {
			printf("You Win!\n");
			break;
		} else if (is_winner(&board, board.opponent)) {
			printf("You Lose.\n");
			break;
		} else if (is_draw(&board)) {
			printf("It's a Draw!\n");
			break;
		}
		display_turn(&board);
	}
}
