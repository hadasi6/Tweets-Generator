#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov_chain.h"

#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"

#define NUM_ARGC 4
#define NUM_ARGC_WITH_WORDS 5
#define COMMAND_INDEX_SEED 1
#define COMMAND_NUM_TWEETS 2
#define COMMAND_INDEX_PATH 3
#define COMMAND_INDEX_NUM_WORDS 4
#define LINE_MAX 1000
#define ENDS_SENTENCE '.'
#define MAX_TWEET_WORDS 20

bool is_number_of_arguments_valid(int argc)
{
  if (argc != NUM_ARGC && argc != NUM_ARGC_WITH_WORDS)
    return false;
  return true;
}

bool is_file_valid(const char* input_file)
{
  FILE *in_file = fopen (input_file, "r");
  if (!in_file)
    return false;
  fclose (in_file);   //todo - maybe in main?
  return true;
}

int count_number_of_words(FILE *fp);   //todo - maybe change int to size_t/long





/**
 *
 * @param fp
 * @param words_to_read
 * @param markov_chain
 * @return 0 - success, 1 - fail.
 */
int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  char line[LINE_MAX];
  int count_words = 0;

  while (fgets(line, LINE_MAX, fp) && (count_words<words_to_read ||
                                                             words_to_read==0))
  {
    char *word = strtok (line, DELIMITERS);
    MarkovNode *prev_node = NULL;
    while (word != NULL)
    {
      int word_len = (int)strlen (word);         //todo - is it int??/size_t
      char *end_ptr = word + word_len - 1;
      int is_end_of_sentence = (*end_ptr == ENDS_SENTENCE);
      Node *curr_node = add_to_database (markov_chain, word);
      if (!curr_node)
      {
//        printf (ALLOCATION_ERROR_MASSAGE);  //TODO- where to print?
//        fclose (fp);                      // todo - maybe in main?
        return 1;
      }
      if (prev_node!=NULL)
      {
        if (add_node_to_frequency_list (prev_node, curr_node->data)==1)
        {
//          printf (ALLOCATION_ERROR_MASSAGE); //TODO- where to print?
//          fclose (fp);                      // todo - maybe in main?
          return 1;
        }
      }
      if (is_end_of_sentence)
        prev_node = NULL;
      else
        prev_node = curr_node->data;
      word = strtok (NULL, DELIMITERS);
      count_words++;
    }
  }
//  fclose (fp);                      // todo - maybe in main?
  return 0;
}



/**
 *
 * @param argc number of arguments - 4 or 5
 * @param argv argv[0]: path.   argv[1]: seed value.(unsigned int)
 * argv[2]: number of tweets (positive int). argv[3]:path to text corpus File.
 * argv[4]: optional.(positive int) if given - the number of words to read
 * from the corpus File.
 * @return
 */
int main(int argc, char *argv[])
{
  if (!is_number_of_arguments_valid(argc))
  {
    printf(NUM_ARGS_ERROR);
    return EXIT_FAILURE;
  }
  if (!is_file_valid(argv[COMMAND_INDEX_PATH]))
  {
    printf (FILE_PATH_ERROR);
    return EXIT_FAILURE;
  }
  FILE *fp = fopen(argv[COMMAND_INDEX_PATH], "r");
  MarkovChain *markov_chain;                          //todo - validate
  int words_to_read = 0;
  if (argc==NUM_ARGC_WITH_WORDS)
    words_to_read = atoi(argv[COMMAND_INDEX_NUM_WORDS]);
  if (fill_database (fp, words_to_read, markov_chain)==1)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    free_database (&markov_chain);                //todo - validate type
    fclose (fp);
    return EXIT_FAILURE;
  }
  fclose (fp);
  for (int i = 0; i < argv[COMMAND_NUM_TWEETS]; ++i)
  {
    MarkovNode* first_markov_node = get_first_random_node (markov_chain);
    printf ("TWEET %d: ", i+1);
    generate_tweet (first_markov_node, MAX_TWEET_WORDS);
    if (i != argv[COMMAND_NUM_TWEETS]-1)
      printf ("\n");
  }
  free_database (markov_chain);
  return EXIT_SUCCESS;
}