/**
 * a draw program for a tournament
 * Created: 2012
 * Renamed variables in English: 2016
 * Author: Selim
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct{
  char name[25];    // fname + sname
  char studentID[9];  // student id
} COMPETITOR;

void printError(char *error){
  printf("\n%s\n", error);
  exit(0);
}

int draw_competitor(int competitor_count,short int *pot){
  int i;

  do{
    i = rand() % competitor_count;
  }while( pot[i] != 0 );

  // remove from the pot
  pot[i] = 1;

  return i;
}

int main(int argc, char *argv[]){
  char competitors_file[15];
  FILE *f;
  short int *pot;                 // 0 means in the pot, 1 means not in the pot
  COMPETITOR *competitors;
  int competitor_count = 0;
  short int **matches;            // matches[0][match_index] : first competitor of the match
  int match_count;
  int match_index;
  int ch;
  int i,j;

  srand(time(NULL));

  // we do not have many competitors there will be
  competitors = (COMPETITOR*) malloc(1 * sizeof(COMPETITOR));
  if( competitors == NULL ){ printError("Couldnt allocate memory!"); }

  if( argc < 2 ){
    // file name was not given
    printf("File name for competitor list > ");
    scanf("%s", competitors_file);
  }else{
    // file name was given
    strcpy(competitors_file, argv[1]);
  }

  //TODO: file parsing could be moved to a function
  f = fopen(competitors_file, "r");
  if( f == NULL ){ printError("Couldnt find the file!"); }

  i=0; j=0;
  ch = fgetc(f);
  while( ch != EOF ){
    //TODO: could use linked list instead of expensive reallocation
    competitors = (COMPETITOR*) realloc(competitors, (i+1) * sizeof(COMPETITOR));
    if( competitors == NULL ){ printError("Couldnt allocate memory!"); }

    while( ch != '\t' ){
      competitors[i].name[j] = ch;
      j++;
      ch = fgetc(f);
    }

    competitors[i].name[j] = '\0';

    while( !isdigit(ch) ){
      ch = fgetc(f);
    }

    j = 0;
    while( ch != '\n' ){
      competitors[i].studentID[j] = ch;
      j++;
      ch = fgetc(f);
    }

    competitors[i].studentID[j] = '\0';
    i++;
    j=0;
    ch = fgetc(f);
  }

  fclose(f);

  competitor_count = i;

  //TODO: could be moved to a function
  // Listing competitors
  printf(" #   FullName                 Student ID\n");
  printf("-------------------------------------------\n");
  for( i=0; i<competitor_count; i++ ){
    printf("%2d - %-25s", i+1,competitors[i].name);
    printf("%s", competitors[i].studentID);
    printf("\n\n");
  }

  printf("Press any key to continue...\n");
  getchar();

  pot = (short int*) calloc(competitor_count, sizeof(short int));
  if( pot == NULL ){ printError("Couldnt allocate memory!"); }

  //TODO: could be moved to a function, int get_match_count(int competitor_count)
  if( competitor_count % 2 != 0 ){
    match_count = (competitor_count + 1) / 2;
  }else{
    match_count = competitor_count / 2;
  }

  matches = (short int**) calloc(2, sizeof(short int*));
  if( matches == NULL ){ printError("Couldnt allocate memory!"); }

  for( j=0; j<2; j++ ){
    matches[j] = (short int*) calloc(match_count, sizeof(short int));
  }

  match_index = 0;

  // picking a competitor if the competitor count is not even
  if( competitor_count % 2 != 0 ){
    printf("One competitor goes to next stage because of odd count.\n");
    printf("And that competitor : ");
    i = rand() % competitor_count;
    printf("%s\n", competitors[i].name);
    pot[i] = 1; // remove from the pot
    matches[0][match_index] = i;
    matches[1][match_index] = -1; // -1 is a marker
    match_index++;
  }

  printf("Press any key to continue...\n");
  getchar();

  printf("\nStarting to draw: \n");
  printf("---------------------\n");
  for( ;match_index<match_count; match_index++){
    printf("%d.match : (press enter to draw)\n", match_index);
    getchar();
    i = draw_competitor(competitor_count,pot);
    printf("%s\n", competitors[i].name);
    j = draw_competitor(competitor_count,pot);
    printf("%s", competitors[j].name);
    printf("\n\n");
    matches[0][match_index] = i;
    matches[1][match_index] = j;
  }

  printf("There is going to be %d matches.\n\n", match_count);
  printf("Press enter to save the fixture to a file...");
  getchar();


  printf("\nFile name for fixture > ");
  // using the same variable
  scanf("%s",competitors_file);

  f = fopen(competitors_file, "w");
  if( f == NULL ){ printError("Couldnt create the file!"); }

  fprintf(f,"=======  Fixture  =======\n");
  for( i=0; i<match_count; i++ ){
    fprintf(f,"%d.Match : \n", i+1);
    if( matches[1][i] == -1 ){
      fprintf(f,"%s (in the next stage)", competitors[matches[0][i]].name);
    }else{
      fprintf(f,"%s & %s", competitors[matches[0][i]].name, competitors[matches[1][i]].name);
    }
    fprintf(f,"\n\n");
  }

  printf("The fixture was saved to %s.\n", competitors_file);

  fclose(f);
  // releasing memory
  free(competitors);
  free(matches);
  free(pot);

  return 0;
}
