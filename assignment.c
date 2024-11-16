/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: [Enter your full name and student number here before submission]
  Dated:     [Enter the date that you "signed" the declaration]

*/



#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <math.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions

/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type

// initialize the board
void initBoard(board_t board)
{
    int i = 0, j = 0;
    for (i = 0; i < BOARD_SIZE; i++)
    {
    	for (j = 0; j < BOARD_SIZE; j++)
    	{
            board[i][j] = '.';
    		if (j & 1)
    		{
                if ((i & 1) == 0 && i < ROWS_WITH_PIECES) board[i][j] = CELL_WPIECE;
                else if (i == 6) board[i][j] = CELL_BPIECE;
    		}
            else
            {
                if (i == 1) board[i][j] = CELL_WPIECE;
                else if (i == 5 || i == 7) board[i][j] = CELL_BPIECE;
            }
        }
    }
}




// print the board with edges
void printBoard(board_t board)
{
    printf("    A   B   C   D   E   F   G   H\n");
    printf("   +---+---+---+---+---+---+---+---+\n");
    int i = 0, j = 0;
	for (i = 0; i < 8; i++)
    {
        printf("%d. |", i + 1);
		for (j = 0; j < 8; j ++) 
            printf(" %c |", board[i][j]);
        printf("\n   +---+---+---+---+---+---+---+---+\n");
	}
    
}




// print the output information of the board at beginning
void printStartBoard(board_t board)
{
    initBoard(board);
    printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
    printf("#BLACK PIECES: %d\n", 12);
    printf("#BLACK PIECES: %d\n", 12);
    printBoard(board);
}



// calculate the cost of the current board
int cost(board_t board)
{
    int b_num = 0, B_num = 0, w_num = 0, W_num = 0;
    int i = 0, j = 0;
	for (i = 0; i < BOARD_SIZE; i++)
	{
		for (j = 0; j < BOARD_SIZE; j++)
		{
            if (board[i][j] == CELL_BPIECE) b_num++;
            else if (board[i][j] == CELL_BTOWER) B_num++;
            else if (board[i][j] == CELL_WPIECE) w_num++;
            else if (board[i][j] == CELL_WTOWER) W_num++;
		}
	}
    return b_num * COST_PIECE + B_num * COST_TOWER - w_num * COST_PIECE - W_num * COST_TOWER;
}





// print the current output information of the board during decision
void printMiddleBoard(board_t board, char* start_to_end, char cur_stage_holder, int cur_stage, int flag)
{
    printf("====================================\n");
    if (flag) printf("*** ");
	if (cur_stage_holder == CELL_BPIECE)
		printf("BLACK ACTION #%d: %s\n", cur_stage, start_to_end);
    else
        printf("WHITE ACTION #%d: %s\n", cur_stage, start_to_end);
    printf("BOARD COST: %d\n", cost(board));
    printBoard(board);
}




// check the error type that the move or capture. return 0 if there is no error.
int errorTypeCoord(board_t board, int s_x, int s_y, int e_x, int e_y, char cur_stage_holder)
{
    if (s_x < 0 || s_x > 7 || s_y < 0 || s_y > 7) return 1;
    if (e_x < 0 || e_x > 7 || e_y < 0 || e_y > 7) return 2;
    if (board[s_x][s_y] == CELL_EMPTY) return 3;
    if (board[e_x][e_y] != CELL_EMPTY) return 4;
    if (cur_stage_holder != board[s_x][s_y] &&
        cur_stage_holder - 'a' + 'A' != board[s_x][s_y]) return 5;
    int offset_x = e_x - s_x;
    int offset_y = e_y - s_y;
    if (abs(offset_x) == abs(offset_y))
    {
        if (abs(offset_x) == 1) {
            if (board[s_x][s_y] >= 'A' && board[s_x][s_y] <= 'Z') return 0;
        	if (board[s_x][s_y] == CELL_BPIECE && cur_stage_holder == CELL_BPIECE)
        	{
                if (offset_x == -1) return 0;
                return 6;
        	}
	        if (board[s_x][s_y] == CELL_WPIECE && cur_stage_holder == CELL_WPIECE)
	        {
                if (offset_x == 1) return 0;
                return 6;
	        }
            return 6;
        }
    	if (abs(offset_x) == 2)
    	{
            unsigned char buf = board[s_x + offset_x / 2][s_y + offset_y / 2];
            if (cur_stage_holder == buf || cur_stage_holder - 'a' + 'A' == buf || '.' == buf) return 6;
            if (board[s_x][s_y] >= 'A' && board[s_x][s_y] <= 'Z') return 0;
            if (board[s_x][s_y] == CELL_BPIECE && cur_stage_holder == CELL_BPIECE)
            {
                if (offset_x == -2) return 0;
                return 6;
            }
            if (board[s_x][s_y] == CELL_WPIECE && cur_stage_holder == CELL_WPIECE)
            {
                if (offset_x == 2) return 0;
                return 6;
            }
            return 6;
    	}
        //// TODO
        //unsigned char buf = board[s_x + offset_x / 2][s_y + offset_y / 2];
        //if (abs(offset_x) == 2 && (cur_stage_holder != buf && cur_stage_holder - 'a' + 'A' != buf && buf != '.')) return 0;
        //if (abs(offset_x) == 2 && board[start_to_end[1] - '1' + offset_x / 2][start_to_end[0] - 'A' + offset_y / 2] != CELL_EMPTY)
            //return 0;
        return 6;
    }
    return 6;
}




// print the error information if there is a error for the move or capture
void printErrorInformation(int error_state)
{
	switch (error_state)
	{
    case 1:
        printf("ERROR: Source cell is outside of the board.\n"); break;
    case 2:
        printf("ERROR: Target cell is outside of the board.\n"); break;
    case 3:
        printf("ERROR: Source cell is empty.\n"); break;
    case 4:
        printf("ERROR: Target cell is not empty.\n"); break;
    case 5:
        printf("ERROR: Source cell holds opponent's piece/tower.\n"); break;
	case 6:
        printf("ERROR: Illegal action.\n"); break;
    default:
        break;
	}
}




// update the board if there is no error
void update(board_t board, int start_x, int start_y, int end_x, int end_y)
{
    board[end_x][end_y] = board[start_x][start_y];
    board[start_x][start_y] = CELL_EMPTY;
    int offset_x = end_x - start_x;
    int offset_y = end_y - start_y;
    if (abs(offset_x) == 2)
        board[start_x + offset_x / 2][start_y + offset_y / 2] = CELL_EMPTY;
    if (board[end_x][end_y] == CELL_WPIECE && end_x == BOARD_SIZE - 1) board[end_x][end_y] = CELL_WTOWER;
    if (board[end_x][end_y] == CELL_BPIECE && end_x == 0) board[end_x][end_y] = CELL_BTOWER;
}




// create the multi-way tree to store the minimax decision results
typedef struct DecisionTree{
    board_t board;                      // the board of the current map
    struct DecisionTree* children[96];  // the children with the maximum length of 12*8
    int children_length;                // the length of the children
    int level;                          // the depth of the current node
    int cost;                           // the cost of the current map
    int cur_holder;                     // the state of the current holder, 0 for BLACK and 1 for WHITE
    int next_position;                  // store the index of the selected children.
}MTree;

typedef struct queue_t
{
    MTree** array;  // the arrays that each node is a multi-way tree node
    int    head;    // the head of the queue
    int    tail;    // the tail of the queue
    int    num;     // the current number in the queue
    int    size;    // the size of the queue
}Queue;

// create a queue with the size of 'size'
Queue* initQueue(int size)
{
    Queue* qp;
    qp = (Queue*)malloc(sizeof(Queue));
    qp->size = size;
    qp->head = qp->tail = qp->num = 0;
    qp->array = (MTree**)malloc(size * sizeof(MTree*));
    return qp;
}

// enqueue
int enqueue(Queue* qp, MTree* data)
{
    if (qp == NULL || qp->num >= qp->size) 
    {
        return 0; 
    }

    qp->array[qp->tail] = data; 
    qp->tail = (qp->tail + 1) % (qp->size); 
    ++qp->num;
    return 1;
}



// dequeue
int dequeue(Queue* qp, MTree** data_ptr)
{
    if (qp == NULL || qp->num <= 0) 
    {
        return 0;
    }

    *data_ptr = qp->array[qp->head]; // 
    qp->head = (qp->head + 1) % (qp->size); // 
    --qp->num;

    return 1;
}



// the queue is empty?
int isQueueEmpty(Queue* qp)
{
    if (qp == NULL || qp->num <= 0)
    {
        return 1;
    }

    return 0;
}



// destroy the queue
void freeQueue(Queue* qp)
{
    free(qp->array);
    free(qp);
}



// create and initialize a minimax decision tree node 
void initTreeNode(MTree** tree, board_t board_map, int level, int child_length, int cur_holder)
{
    (*tree)->level = level;
	// there is not necessary to calculate the cost during the initialization
    //(*tree)->cost = cost(board_map);
    int i = 0, j = 0;
    for (i = 0; i < BOARD_SIZE; i++)
        for (j = 0; j < BOARD_SIZE; j++)
            (*tree)->board[i][j] = board_map[i][j];
	// all the children should be addressed
    for (i = 0; i < 96; i++) (*tree)->children[i] = NULL;
    (*tree)->children_length = child_length;
    (*tree)->cur_holder = cur_holder;
}



// to find the minimax decision result, the algorithm should traverse the nodes in post order.
// the nodes should calculate the maximum or minimum cost hierarchically.
void transTree(MTree ** tree, int cur_holder)
{
    if ((*tree)->children_length != 0)
    {
        int i = 0;
        
        for (i = 0; i < (*tree)->children_length; i++)
        {
            transTree(&(*tree)->children[i], cur_holder);
        }
    	
        if (((*tree)->level & 1) == 0)
        {
	        // find max
            int _max = INT_MIN;
            for (i = 0; i < (*tree)->children_length; i++)
            {
                if ((*tree)->children[i]->children_length == 0)
                    (*tree)->children[i]->cost = cost((*tree)->children[i]->board) * (1 - 2 * cur_holder);
                if (_max < (*tree)->children[i]->cost)
                {
                    _max = (*tree)->children[i]->cost;
                    (*tree)->next_position = i;
                }
            }
            (*tree)->cost = _max;
        }
        else
        {
        	// find min
            int _min = INT_MAX;
            for (i = 0; i < (*tree)->children_length; i++)
            {
                if ((*tree)->children[i]->children_length == 0)
                    (*tree)->children[i]->cost = cost((*tree)->children[i]->board) * (1 - 2 * cur_holder);
                if (_min > (*tree)->children[i]->cost)
                {
                    _min = (*tree)->children[i]->cost;
                    (*tree)->next_position = i;
                }
            }
            (*tree)->cost = _min;
        }
        //printf("level : %d, child_length : %d, cost : %d\n", (*tree)->level, (*tree)->children_length, (*tree)->cost);
    }
}




// after getting the decision, the tree should be released.
void freeMTree(MTree* head)
{
    if (head == NULL) return;
    int i = 0;
    for (i = 0; i < head->children_length; ++i)
    {
        freeMTree(head->children[i]);
    }
    free(head);
}


// given two boards, the function aims to calculate the path operation in string format.
char* board2Points(board_t start_board, board_t end_board, int cur_holder)
{
    int i = 0, j = 0;
    int s_x, s_y, e_x, e_y;
    for (i = 0; i < BOARD_SIZE; i++)
        for (j = 0; j < BOARD_SIZE; j++)
        {
            if (cur_holder == 0)
            {
                if (start_board[i][j] == CELL_BPIECE || start_board[i][j] == CELL_BTOWER)
                {
                    if (end_board[i][j] != start_board[i][j] && end_board[i][j] == CELL_EMPTY)
                    {
                        s_x = i;
                        s_y = j;
                    }
                }
                if (end_board[i][j] == CELL_BPIECE || end_board[i][j] == CELL_BTOWER)
                {
                    if (end_board[i][j] != start_board[i][j] && start_board[i][j] == CELL_EMPTY)
                    {
                        e_x = i;
                        e_y = j;
                    }
                }
            }
            else
            {
                if (start_board[i][j] == CELL_WPIECE || start_board[i][j] == CELL_WTOWER)
                {
                    if (end_board[i][j] != start_board[i][j] && end_board[i][j] == CELL_EMPTY)
                    {
                        s_x = i;
                        s_y = j;
                    }
                }
                if (end_board[i][j] == CELL_WPIECE || end_board[i][j] == CELL_WTOWER)
                {
                    if (end_board[i][j] != start_board[i][j] && start_board[i][j] == CELL_EMPTY)
                    {
                        e_x = i;
                        e_y = j;
                    }
                }
            }
        }
	// malloc the heap memory to store the operation
    char* res = (char*)malloc((sizeof(char) * 6));
    res[0] = s_y + 'A';
    res[1] = s_x + '1';
    res[2] = '-';
    res[3] = e_y + 'A';
    res[4] = e_x + '1';
	// the last character is '\0', that is, the ending pos.
    res[5] = '\0';
    return res;
}

// do the decision for the next TREE_DEPTH levels using breadth-first traversal. queue is used to achieve the decision.
char* bfs(board_t board_map, int cur_holder)
{
	// for the last level, there may have the maximum length of queue. the last level has (12*4)^TREE_DEPTH nodes.
    Queue* queue = initQueue((int)pow(12*4, TREE_DEPTH));
    MTree* tree = (MTree*)malloc(sizeof(MTree));
    initTreeNode(&tree, board_map, 0, 0, cur_holder);
    enqueue(queue, tree);
    char state[2];
    state[0] = CELL_BPIECE, state[1] = CELL_WPIECE;
    //int flag = 0;
    int offset_x[] = { -1, -2, 1, 2, 1, 2, -1, -2};
    int offset_y[] = { 1, 2, 1, 2, -1, -2, -1, -2};
    int i = 0, j = 0, k = 0;
    int operation_state = 0;
	while (!isQueueEmpty(queue))
	{
        MTree* cur_node = NULL;
        dequeue(queue, &cur_node);
        cur_node->cost = cost(cur_node->board);
        //printf("Level: %d, cost: %d\n", cur_node->level, cur_node->cost);
        if (cur_node->level >= TREE_DEPTH) continue;
		
		// add nodes.
        for (i = 0; i < BOARD_SIZE; i++)
            for (j = 0; j < BOARD_SIZE; j++)
            {
            	//can up and down
                if (cur_node->board[i][j] == state[cur_node->cur_holder] - 'a' + 'A' || cur_node->board[i][j] == state[cur_node->cur_holder])
                {
                    for (k = 0; k < 8; k++)
                    {
                        if (errorTypeCoord(cur_node->board, i, j, i + offset_x[k], j + offset_y[k], state[cur_node->cur_holder]) == 0)
                        {
                            // the state is avaliable
                            MTree* next_tree = (MTree*)malloc(sizeof(MTree));
                            // init the node and update the node
                            initTreeNode(&next_tree, cur_node->board, cur_node->level + 1, 0, 1 - cur_node->cur_holder);
                            update(next_tree->board, i, j, i + offset_x[k], j + offset_y[k]);

                            // no need to update the cost since it can be calculate during post order traversal
                            //next_tree->cost = cost(next_tree->board);
                            cur_node->children[cur_node->children_length++] = next_tree;
                            enqueue(queue, next_tree);
                            operation_state = 1;
                        }
                    }
                }
            }
	}
	// if the length of queue is 0 at beginning, the game is finished.
    if (!operation_state)
    {
        freeMTree(tree);
        freeQueue(queue);
        if (cur_holder == 0) return "0";
        return "1";
    }

	// find the next decision
    transTree(&tree, cur_holder);
    MTree* p = tree->children[tree->next_position];
	char *res = board2Points(tree->board, p->board, cur_holder);

    freeMTree(tree);
    freeQueue(queue);
    return res;
}

// process the input data from keyboard
void readMap(board_t board)
{
    char start_to_end[6];
    char state[2];
    state[0] = CELL_BPIECE, state[1] = CELL_WPIECE;
    int flag = 0;
    while (scanf("%s", start_to_end) != EOF)
    {
        char cur_state = state[(flag++) & 1];
    	// the input is 'A'
        if (strcmp(start_to_end, "A")==0)
        {
            char * p = bfs(board, (flag - 1) & 1);
            if (strcmp(p, "0") == 0)
            {
                printf("WHITE WIN!\n");
                return;
            }
            if (strcmp(p, "1") == 0) {
                printf("BLACK WIN!\n");
                return;
            }
            update(board, p[1] - '1', p[0] - 'A', p[4] - '1', p[3] - 'A');
            printMiddleBoard(board, p, cur_state, flag, 1);
            free(p);
            return;
        }
    	// the input is 'P'
        else if (strcmp(start_to_end, "P")==0)
        {
            flag--;
            int i = 0;
        	for (i = 0; i < 10; i++)
        	{
                cur_state = state[(flag++) & 1];
                char* p = bfs(board, (flag - 1) & 1);
                if (strcmp(p, "0") == 0)
                {
                    printf("WHITE WIN!\n");
                    return;
                }
                if (strcmp(p, "1") == 0) {
                    printf("BLACK WIN!\n");
                    return;
                }
                update(board, p[1] - '1', p[0] - 'A', p[4] - '1', p[3] - 'A');
                printMiddleBoard(board, p, cur_state, flag, 1);
                free(p);
        	}
            return;
        }
        // the input is 'xx-xx'
        else {
            int error_state = errorTypeCoord(board, start_to_end[1] - '1', start_to_end[0] - 'A', start_to_end[4] - '1', start_to_end[3] - 'A', cur_state);
            if (!error_state)
            {
                update(board, start_to_end[1] - '1', start_to_end[0] - 'A', start_to_end[4] - '1', start_to_end[3] - 'A');
                printMiddleBoard(board, start_to_end, cur_state, flag, 0);
            }
            else {
                printErrorInformation(error_state);
                return;
			}
        }
    }
}













int main(int argc, char* argv[]) {
    // YOUR IMPLEMENTATION OF STAGES 0-2
	
    board_t board;
    printStartBoard(board);
    readMap(board);
	
    return EXIT_SUCCESS;            // exit program with the success code
}

/* THE END -------------------------------------------------------------------*/
