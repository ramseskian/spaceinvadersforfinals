#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Player {
    int x, y;
    int is_alive;
};

struct Alien {
    int x, y;
    int is_alive;
};

struct Bullet {
    int x, y;
    int is_active;
};

struct HighScore {
    char name[50];
    int score;
};
//all the global variables in the game to consider
struct Player player;                    //the player object 'A'
struct Alien aliens[3][5];               //the array of aliens
struct Bullet player_bullets[3];         //array of the player's bullets (max is 3 every scene)

int score = 0;                           //current score
int aliens_alive = 15;                   //number of aliens still alive
int g_alien_move_dir = 1;           //variable for the aliens' direction: 1 right, -1 left
int g_level = 1;                    //current level
struct HighScore g_high_score;             //variable to store high score

//function declarations
void initialize_game();
void draw_board();
void process_input();
void update_game();
void clear_screen();
void read_high_score();
void save_high_score();

int main() {
    read_high_score();                //loads high score (if there's a high score)
    initialize_game();                //set up the initial state of the game

    //main game loop
    while (player.is_alive && aliens_alive > 0) {
        draw_board();                 //draws the current game board
        process_input();              //processes input from the user (a,s, or d)
        update_game();          //updates positions and checks if there are any collisions during the scene
    }

    //end of game
    clear_screen();                   //clears the screen
    printf("\n\n");                   //prints new lines
    if (aliens_alive == 0) {        //checks if aliens alive are 0, then the player wins. aliens are = 15.
        printf("    YOU WIN! Level %d completed.\n", g_level);
        g_level++;                    //increases level after winning
        aliens_alive = 15;          //resets aliens but harder difficulty
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 5; c++) {
                aliens[r][c].is_alive = 1;  //revives all the aliens (generates them all again following the matrix [3][5]
                aliens[r][c].y = r + 1;     //resets y
            }
        }
        player.is_alive = 1;        //revives player
        main();                       //restarts main for the new level (new level, harder difficulty)
    } else {                          //if the player died, it prints game over screen and ends the code after performing the if statement in determining the high score
        printf("    GAME OVER!\n");
    }
    printf("    Final Score: %d\n", score);  //shows final score
    printf("    High Score: %d by %s\n", g_high_score.score, g_high_score.name);  //shows high score

    if (score > g_high_score.score) {       //checks new high score, else it won't change anything
        printf("    New High Score! Enter your name: ");
        char name[50];                //buffer for name input
        fgets(name, sizeof(name), stdin);  //reads name using fgets
        name[strcspn(name, "\n")] = 0;     //removes newline from input
        strcpy(g_high_score.name, name);  //copies name to high score struct
        g_high_score.score = score;     //sets new high score
        save_high_score();                //saves in the file
    }

    printf("\n    Press Enter to exit...");
    scanf("%*c");                        //waits for the user to press 'Enter'
    return 0;
}

void initialize_game() {
    score = 0;                      //resets score to 0
    aliens_alive = 15;              //resets alien count
    g_alien_move_dir = 1;             //resets alien moving direction so that they move right to left every time

    player.x = 10;                  //center player (20/2) since the grid width is 20 columns wide
    player.y = 13;                  //player near the bottom (15-2) since the grid height is 15 where 0 is at top and 14 is at the bottom
    player.is_alive = 1;            // Player starts alive

    //initialize where the aliens are in the grid
    for (int r = 0; r < 3; r++) { //for each row
        for (int c = 0; c < 5; c++) { //for each column
            aliens[r][c].x = c * 2 + 1;  //adds a space horizontally
            aliens[r][c].y = r + 1;      //adds a space vertically
            aliens[r][c].is_alive = 1;   //all aliens start alive (generated)
        }
    }

    //deactivate player's bullets
    for (int i = 0; i < 3; i++) {
        player_bullets[i].is_active = 0;
    }
}

//function to clear screen and draw board
void draw_board() {
    clear_screen();                   //clears console

    printf("Space Invaders (Machine Problem) - Level %d\n", g_level);  //print title wiwth level
    printf("Score: %d | Aliens Left: %d | High Score: %d\n", score, aliens_alive, g_high_score.score);  //prints stats on heading
    printf("----------------------\n");  //top border

    //loops through each row and column
    for (int y = 0; y < 15; y++) {
        printf("|");                  //prints left border
        //loops through each column
        for (int x = 0; x < 20; x++) {
            char char_to_print = ' '; //default to add a 'space' for blank spaces

            //checks if the player is in this position
            if (player.x == x && player.y == y) {
                char_to_print = 'A';  //player's character
            }
            else {
                //checks player's bullets if it is in this position
                for (int i = 0; i < 3; i++) {
                    if (player_bullets[i].is_active && player_bullets[i].x == x && player_bullets[i].y == y) {
                        char_to_print = '|';  //player bullet found
                        break;               //stops checking if the bullet is found
                    }
                }
                //if no bullets found, checks aliens
                if (char_to_print == ' ') {
                    for (int r = 0; r < 3; r++) {
                        for (int c = 0; c < 5; c++) {
                            if (aliens[r][c].is_alive && aliens[r][c].x == x && aliens[r][c].y == y) {
                                char_to_print = 'V';  //character for alien
                                break;               //stops checking if the alien is found
                            }
                        }
                        if (char_to_print == 'V')
                            break;  //exit outer loop
                    }
                }
            }

            printf("%c", char_to_print);  //prints the character
        }
        printf("|\n");                //prints right border and new line (proceeds to print the left border after newline)
    }
    printf("----------------------\n");  //after finishing the for loop, prints bottom border (para aesthetic)
    printf("Press: [a] Left | [d] Right | [s] Shoot\n");  //controls reminder
}

//function for user input
void process_input() {
    char input;                       //user input
    while (1) {                       //loops until the input is valid
        printf("Enter your move: ");
        scanf(" %c", &input);         //reads one character
        if (input == 'a' || input == 's' || input == 'd') {
            break;                    //valid input then exits loop
        }
    }
    //used switch cases for different inputs
    switch (input) {
        case 'a':
            if (player.x > 0) player.x--;  //moves left
            break;
        case 'd':
            if (player.x < 19) player.x++;  //moves right
            break;
        case 's':
            //for loop to find bullet slot
            for (int i = 0; i < 3; i++) {
                if (!player_bullets[i].is_active) {
                    player_bullets[i].is_active = 1;  //activates the bullet
                    player_bullets[i].x = player.x; //sets x to player's x coordinate
                    player_bullets[i].y = player.y - 1;  //sets y above the player (bullet comes out a line before the player but in the same x coordinate)
                    break;
                }
            }
            break;
    }
    while (getchar() != '\n');  //prevents spamming letters and only 1 letter per iteration
}

void update_game() { //function to update game state
    //
    for (int i = 0; i < 3; i++) {
        if (player_bullets[i].is_active) {
            player_bullets[i].y--;  //moves player's bullets up
            if (player_bullets[i].y < 0) player_bullets[i].is_active = 0;
        }
    }

    int drop_down = 0;                //aliens changing lanes "dropping down"
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 5; c++) {
            if (aliens[r][c].is_alive) {
                int next_x = aliens[r][c].x + g_alien_move_dir;  //calculates next x
                if (next_x < 0 || next_x >= 20) {  //checks if x is hitting the edge
                    drop_down = 1;                //sets drop flag
                    g_alien_move_dir *= -1;       //reverses direction
                }
            }
        }
    }

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 5; c++) {
            if (drop_down) aliens[r][c].y++;  //drops down if flag set
            aliens[r][c].x += g_alien_move_dir;  //moves left or right

            if (aliens[r][c].is_alive && aliens[r][c].y == player.y) player.is_alive = 0; //makes sure that the aliens don't hit the row of the player
        }
    }
    //checks for collisions between bullets and aliens
    for (int i = 0; i < 3; i++) {
        if (player_bullets[i].is_active) {
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 5; c++) {
                    if (aliens[r][c].is_alive &&
                        player_bullets[i].x == aliens[r][c].x &&
                        player_bullets[i].y == aliens[r][c].y) {
                        aliens[r][c].is_alive = 0;  //kills alien
                        player_bullets[i].is_active = 0;  //deactivates bullet after hitting alien
                        score += 100 * g_level;     //adds score (PS. more points for higher levels, for added game functionality ;))
                        aliens_alive--;             //minus one alien alive from the counter
                    }
                }
            }
        }
    }
}

void clear_screen() {
    system("cls");  //clears screen (so that you do not need to scroll up to see the updated board)
}

//function to read high score from file
void read_high_score() {
    FILE *file = fopen("highscore.txt", "r");  //open file for reading
    if (file) {                               //function checking if the file exists
        fscanf(file, "%49s %d", g_high_score.name, &g_high_score.score);  //reads name and score (name's limit is 50 characters))
        fclose(file);                         //closes file after reading
    } else {                                  //else, `no high score yet` scenario
        strcpy(g_high_score.name, "Nobody");
        g_high_score.score = 0;
    }
}

//function to save high score to file
void save_high_score() {
    FILE *file = fopen("highscore.txt", "w");  //opens file for writing
    if (file) {                               //if the file opens successfully
        fprintf(file, "%s %d\n", g_high_score.name, g_high_score.score);  //writes name and score to file
        fclose(file);                         //close file after writing
    }
}


