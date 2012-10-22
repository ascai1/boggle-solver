#include <stdio.h>
#define NUM_LETTERS 26

struct Cell;

struct Cell_V {
	struct Cell * cell;
	struct Cell_V * next;
};

struct Cell {
	char val;
	char visited;
	struct Cell_V next[NUM_LETTERS];
};

struct Cell * init_cell(char v) {
	struct Cell * new_cell = (struct Cell *)malloc(sizeof(struct Cell));
	if (!new_cell) return NULL;
	new_cell->val = v;
	new_cell->visited = 0;
	struct Cell_V * temp;
	for (temp = new_cell->next; temp != new_cell->next + NUM_LETTERS; ++temp) {
		temp->cell = NULL;
		temp->next = NULL;
	}
	return new_cell;
}

char push_child(struct Cell * parent, struct Cell * child) {
	struct Cell_V * temp = &(parent->next[child->val - 'a']);
	while (temp->next) {
		temp = temp->next;
	}
	if (temp->cell) {
		struct Cell_V * new_cellv = (struct Cell_V *)malloc(sizeof(struct Cell_V));
		if (!new_cellv) return 1;
		new_cellv->cell = child;
		new_cellv->next = NULL;
		temp->next = new_cellv;
		return 0;
	} else {
		temp->cell = child;
		return 0;
	}
}

void delete_cellv(struct Cell_V * cellv) {
	if (cellv->next) delete_cellv(cellv->next);
	free(cellv);
}

void delete_cell(struct Cell * cell) {
	int i;
	for (i = 0; i < NUM_LETTERS; ++i) {
		struct Cell_V * temp = cell->next[i].next;
		if (temp) delete_cellv(temp);
	}
	free(cell);
}

char to_lower_case(char c) {
	if (c >= 'a' && c <= 'z') return c;
	if (c >= 'A' && c <= 'Z') return c + ('a' - 'A');
	return 0;
}

struct Cell * is_legal(char * c, struct Cell * cell) {
	if (!(*c)) return cell;

	*c = to_lower_case(*c);
	if (!(*c)) return NULL;

	struct Cell_V * temp;
	for (temp = &(cell->next[*c - 'a']); temp && temp->cell; temp = temp->next) {
		if (temp->cell->visited) continue;
		cell->visited = 1;
		struct Cell * result = is_legal(c + 1, temp->cell);
		cell->visited = 0;
		if (result) return result;
	}
	return NULL;
}

int main(int argc, char * argv[]) {
	if (argc != 3) {
		printf("Usage: boggle_solver [boggle layout] [word list]\n");
		return 1;
	}

	// watch memory
	struct Cell * starting_cell = init_cell(0);

	// watch: opening multiple files
	FILE * word_list = fopen(argv[2], "r");
	if (!word_list) {
		printf("Word list not found\n");
		delete_cell(starting_cell);
		return 1;
	}

	FILE * boggle_layout = fopen(argv[1], "r");
	if (!boggle_layout) {
		printf("Boggle layout not found\n");
		delete_cell(starting_cell);
		return 1;
	}

	int l;
	int w;
	if (fscanf(boggle_layout, "%d %d", &l, &w) != 2) {
		printf("Boggle layout format incorrect\n");
		fclose(boggle_layout);
		delete_cell(starting_cell);
		return 1;
	}

	// watch memory
	struct Cell ** cells = (struct Cell **)malloc(l * w * sizeof(struct Cell *));

	int i;
	int j;
	char c;
	char err = 0;
	struct Cell ** curr_cell = cells;

	for (i = 0; i < l && !err; ++i) {
		for (j = 0; j < w; ++j) {
			if (fscanf(boggle_layout, " %c", &c) != 1 || !(c = to_lower_case(c))) {
				err = 1;
				break;
			}
			curr_cell = &(cells[i * w + j]);
			// watch memory
			*curr_cell = init_cell(c);
			
			char pusherr = 0;
			pusherr |= push_child(starting_cell, *curr_cell);
			if (j) {
				pusherr |= push_child(*(curr_cell), *(curr_cell - 1));
				pusherr |= push_child(*(curr_cell - 1), *(curr_cell));
			}
			if (i) {
				pusherr |= push_child(*(curr_cell), *(curr_cell - w));
				pusherr |= push_child(*(curr_cell - w), *(curr_cell));
			}
			if (i && j) {
				pusherr |= push_child(*(curr_cell), *(curr_cell - w - 1));
				pusherr |= push_child(*(curr_cell - w - 1), *(curr_cell));
			}
			if (i && (j + 1 != w)) {
				pusherr |= push_child(*(curr_cell), *(curr_cell - w + 1));
				pusherr |= push_child(*(curr_cell - w + 1), *(curr_cell));
			}
			if (pusherr) {
				err = 2;
				break;
			}
		}
		if (!err && (i + 1 != w) && !(fscanf(boggle_layout, "%c", &c) && c == '\n')) {
			err = 1;
		}
	}

	fclose(boggle_layout);

	char word[128] = {0};
	if (!err) {
		while (fscanf(word_list, "%127s", word) == 1) {
			// for the sake of simplicity, word can only be max 126 chars long
			if (word[126]) {
				err = 3;
				break;
			}
			if (is_legal(word, starting_cell)) {
				printf("%s\n", word);
			}
		}
	}

	for (; curr_cell != cells; --curr_cell) {
		delete_cell(*curr_cell);
	}
	delete_cell(starting_cell);
	free(cells);
	fclose(word_list);

	switch (err) {
		case 1: {
			printf("Boggle layout format incorrect\n");
			return 1;
		}
		case 2: {
			printf("Memory allocation error: cellv\n");
			return 2;
		}
		case 3: {
			printf("The word beginning with the following sequence is too long:\n%s\n", word);
			return 1;
		}
	}

	return 0;
}
