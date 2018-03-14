

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define TABLE_SIZE 200
#define STRING_SIZE 80
#define MAGIC 20


typedef struct sPair
{
      char key[STRING_SIZE];
      char value[STRING_SIZE];
} sPair_t;

static sPair_t table[TABLE_SIZE];
static int size = 0;

int main(int argc, char **argv)
{
      //sPair_t table[TABLE_SIZE];
      int size = 0;
      int ctr = 0;
      char *line = NULL;
      size_t length = 0;
      ssize_t read;
      FILE *bookmarkFile;

      printf("aaAAa\n");

      bookmarkFile  = fopen(".mybookmarks", "r");
      //fscanf(bookmarkFile, "%d", &size);
      // getline(&line, &length, bookmarkFile);
      printf("Line: %s\n",  line);
      if(bookmarkFile == NULL)     printf("YOUR dir ist NULL \n");

      while ((read = getline(&line, &length, bookmarkFile)) != -1)
      {
            printf("wrhher\n");
            strcpy(table[ctr].key, line);
            //table[ctr].key = *line;
            if(getline(&line, &length, bookmarkFile) != -1)
            {
                  strcpy(table[ctr].value, line);
                  ctr++;
            }else
            {
                  printf("a!!!!!a");
            }

      }
      fclose(bookmarkFile);
      free(line);


      printf("size: %d\n", size);
      printf("ctr: %d\n", ctr);
      printf("Key: %s, Value: %s", table[0].key, table[0].value);


      //sPair_t *sPair_ptr;
      //sPair_t sPairVal;
      //sPair_ptr = &sPairVal;



      if(argc < 3)
      {
            printf("No correct command entered. \n" );

      }else if(1)
      {

      }









      return 0;
}
