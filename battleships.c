#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#define  GRID 5
int human_grid[GRID][GRID];
int computer_grid[GRID][GRID];
int human_aim_grid[GRID][GRID];
int computer_aim_grid[GRID][GRID];
char* battleships[] = {"Battleship (4 Spaces)", "Submarine (3 Spaces)", "Destoryer (2 Spaces)"};
int battleship_ranges[] = {4,3,2};
int player_ships_sunk[] = {0, 0,0};
int computer_ships_sunk[] = {0,0, 0};

//Intialise grid's purpose is to set the entire grid to 'water' to prepare coordidinates for 
//the gameplay and choosing of ship coordidinates.
//Grid Parameter: grid - the address of the grid that needs to be intialised.
int array_sum(int hits[]){
    int sum = 0;
    for(int i = 0;i < 3; i++){
        sum+= hits[i];
       // printf("%d\n", sum);
        
    }    
    return sum;
} 

void intialise_grids(){
    for(int i = 0; i<GRID; i++){
        for (int j = 0; j< GRID; j++){
            human_grid[i][j] = 0;
            computer_grid[i][j] = 0;
            human_aim_grid[i][j] = 0;
            computer_aim_grid[i][j] = 0; 
        }
    }  
}
int letter_transform(char letter){
    int letter_value = letter - 65;
    return letter_value;    
}
int number_transform(char number){
    int number_value = number - 48;
    return number_value;
}
char letter_transform2(char letter){
    char new_letter = 48 + letter;
    return new_letter;
}
char number_transform2(char number){
    char number_value = number + 65;
    return number_value;
}
int check_array(int x, int y, int state, int battleship, int grid[][GRID]){
    if(x >= GRID || y>= GRID || x<0 || y<0){
        return 0;
    }
    else if(state){
        
        for(int i = x; i<x + battleship; i++){
            if (grid[i][y] != 0)
            {
               return 0;
            }
            
        }
    }
    else{        
         for(int i = y; i<y + battleship; i++){
            if (grid[x][i] != 0)
            {                
                return 0;
            }            
        }
    }
    
    return 1; 
}

void plot_choices(int x, int y, int state, int grid[][GRID], int battleship){    
    if(state){        
        for(int i = x; i<x + battleship; i++){            
            grid[i][y] = battleship;               
        }
    }
    else{
        for(int i = y; i<y + battleship; i++){
            grid[x][i] = battleship;                         
        }
    }
}

// Gather user input 
void user_input_config(){
    int counter = 0;
    while(counter < 3){
        printf("Please select the starting coordinate for %s and the direction: \n", battleships[counter]);
        char ch[3];
        scanf("%s", ch);
        int x = letter_transform(toupper(ch[0]));
        int y = number_transform(ch[1]) - 1;
        int direction = number_transform(ch[2]);
        if(check_array(x, y, direction, battleship_ranges[counter], human_grid)){
            plot_choices(x, y, direction, human_grid, battleship_ranges[counter]);
            counter++;
        }
        else{
            printf("Please pick choices for %s again as a ship is already in this position\n", battleships[counter]);
        }
    }                  
}
int generate_computer_shots(int shot_selection){
    time_t t;
    srand((unsigned) time(&t)); 
    int x = rand() % shot_selection;
    return x;       
}
void generate_computer_choices(){
    time_t t;
    srand((unsigned) time(&t)); 
    int counter = 0;
    int x;
    int y; 
    int direction;
    while(counter < 3){        
        direction = rand() % 2;
        if(direction){
             x = generate_computer_shots(GRID - (battleship_ranges[counter] - 1));
             y = generate_computer_shots(GRID);
        }
        else{
             y = generate_computer_shots(GRID - (battleship_ranges[counter] - 1));
             x = generate_computer_shots(GRID);
        }
        if(check_array(x, y, direction, battleship_ranges[counter], computer_grid)){
            plot_choices(x, y, direction, computer_grid, battleship_ranges[counter]);
            counter++;
        }
    }        
}

int validate_strike(int x, int y){
    if((human_grid[x][y] >= 0) && computer_aim_grid[x][y] < 1){
         return 1;
    }
    return 0;
}
void mark_aim_grid(int aim_GRID[][GRID], int strike_value, int x, int y){
    if(strike_value > 1){
        aim_GRID[x][y] = strike_value;
    }
    else{
        aim_GRID[x][y] = 1;
    }
}

void print_previous_choices(int grid[][GRID]){
    for(int i =0; i<GRID; i++){
        for(int j = 0; j<GRID; j++){
            printf("%d\t", grid[i][j]);
        }
        printf("\n");
    }
}
int next_hit_determination(int hits, int previous_hit){
    time_t t;
    srand((unsigned) time(&t)); 
    int strike;
    if((previous_hit == 4 || previous_hit == 3 || previous_hit == 2) && hits == 1){
        strike = rand() % 3;        
    }
    else if(previous_hit == 4 && hits == 2){
        strike = rand() % 2;
    }
    else{
        strike = 1;
    }
    
    return strike;
}
int previous_hit = 0;
int next_hit[2];
void strike_oppoistion(int previous_strike){
    for(int i = 0; i < GRID; i++){
        for(int j =0; j< GRID; j++){
            if(human_grid[i][j] == previous_strike && computer_aim_grid[i][j] < 1){
                next_hit[0] = i;
                next_hit[1] = j;               
            }
        }        
    }
}
int computer_strategy(){
    int x;
    int y;    
    do{    
        if(previous_hit > 0){
        strike_oppoistion(previous_hit);
        x = next_hit[0];
        y = next_hit[1];
        break;  }   
        x = generate_computer_shots(GRID);
        y = generate_computer_shots(GRID);    
    }while(!validate_strike(x,y));
    int strike_value = human_grid[x][y]; 
    mark_aim_grid(computer_aim_grid, strike_value, x, y);
    if(strike_value > 1){
        computer_ships_sunk[4 - strike_value]++;
        printf("%s has been hit at %c,%d!\n", battleships[4- strike_value],number_transform2(x),y+1);
        previous_hit = strike_value;
        if(computer_ships_sunk[4-strike_value] == strike_value){
            printf("%s has sunk!\n", battleships[4-strike_value]);
            previous_hit = 0;
            if(array_sum(computer_ships_sunk) >= 9){
                return 0;             
        }
            else{
                do{
                    x = generate_computer_shots(GRID);
                    y = generate_computer_shots(GRID);
                }while (!validate_strike(x,y));
                
                strike_value = 0;
                printf("Computer missed at %c, %d\n", number_transform2(x),y+1);
                mark_aim_grid(computer_aim_grid, strike_value, x, y);
            }
            
        }
        else{
            int determination = next_hit_determination(computer_ships_sunk[4-strike_value], previous_hit);
            if(determination == 1){
                return determination;
            }
            else{
                do{
                    x = generate_computer_shots(GRID);
                    y = generate_computer_shots(GRID);
                }while (!validate_strike(x,y));
                
                strike_value = 0;
                printf("Computer missed at %c, %d\n", number_transform2(x),y+1);
                mark_aim_grid(computer_aim_grid, strike_value, x, y);
                
            }
            
        }
    } 
    else if(strike_value == 0){
        printf("Computer missed at %c, %d\n", number_transform2(x),y+1);
        mark_aim_grid(computer_aim_grid, strike_value, x, y);
        return 0;
    }
    return 0;         
}
void human_turn(){
    char ch[2];
    int turn = 1;
    while(turn){
        int choiceValid = 0;
        int setup = 1;
        int x;
        int y;
        while(setup){
             printf("Would you like to see previous choices? 0 No 1 Yes\n");
            int choice = 0;
             scanf("%d", &choice);
             if(choice){
                print_previous_choices(human_aim_grid);
                }
            if(choice > 1){printf("Incorrect choice\n");break;}
        
            printf("Please select coordinate: \n");
            scanf("%s", ch);
            x = letter_transform(toupper(ch[0]));
            y = number_transform(ch[1]) -1;
            if(x > GRID || x< 0 || y > GRID || y< 0){
                printf("Incorrect coordinate\n");
                break;
            }
            printf("Confirm choice: %c%c\n", ch[0],ch[1]  );
            scanf("%d", &choiceValid);
            setup = 0;
        }
       
        while(choiceValid){
            int ship = computer_grid[x][y];
            if(computer_grid[x][y] != 0 && human_aim_grid[x][y] < 1){
                printf("Ship Hit at %d,%d!\n", x, y);
                human_aim_grid[x][y] = 5;
                player_ships_sunk[4-ship]++;
                choiceValid = 0;
                if(player_ships_sunk[4 - ship] == ship){
                    printf("You have successfully sunk %s\n", battleships[4-ship]);
                    if(array_sum(player_ships_sunk) >= 9){
                        turn = 0;
                        break;
                    }
                    }
                }
            
            else{
                printf("Miss at %d, %d\n",x,y);
                human_aim_grid[x][y] = 1;
                choiceValid = 0;
                turn = 0;
            }
        }      
    }
    
}
void gameplay(){
    intialise_grids();
    printf("\nWelcome to Battleships!\n\nThe game is a %d by %d grid.\n\n", GRID, GRID);
    printf("Please choose all coordinates within the letters %c and %c and numbers between %d and %d\nas well as the direction. Vertical(1), Horizontal(0). \n\n\n", 65, (65 + GRID -1), 1, GRID);
    user_input_config();      
    generate_computer_choices();
    print_previous_choices(human_grid);
    printf("\n");
    printf("\n");
    printf("\n");
    print_previous_choices(computer_grid);
    
    while(array_sum(computer_ships_sunk) < 9){
        int x;
        human_turn();
        if(array_sum(player_ships_sunk) >= 9){
            break;
        }
        
        do{
            x = computer_strategy();
        }while(x == 1);
    }
    int computer_hits = array_sum(computer_ships_sunk);
    int human_hits = array_sum(player_ships_sunk);
    if(computer_hits > human_hits){
        printf("Computer won, you lost. Better luck next time :)\n");
    }
    else{
        printf("Congratulations! You Won!\n");
    }
}
int main(void){
    gameplay();
    return 0;
}
