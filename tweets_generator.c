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
#define BASE 10


/**
 * Check if the number of arguments is valid.
 * @param argc: Number of arguments
 * @return true if the number of arguments is valid, false otherwise
 */
bool is_number_of_arguments_valid(const int argc)
{
  if (argc != NUM_ARGC && argc != NUM_ARGC_WITH_WORDS)
  {return false;}
  return true;
}


/**
 * checks if the file valid
 * @param input_file
 * @return true if valid, false otherwise.
 */
bool is_file_valid(const char* input_file)
{
  FILE *in_file = fopen (input_file, "r");
  if (!in_file)
  {return false;}
  fclose (in_file);
  return true;
}


/**
 * Fill the markov_chain database with words from the given file.
 * @param fp File pointer to read from.
 * @param words_to_read Number of words to read.
 * @param markov_chain The MarkovChain to fill.
 * @return 0 if successful, 1 if there was an error.
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
    while (word != NULL && (count_words<words_to_read || words_to_read==0))
    {
      int word_len = (int)strlen (word);
      char *end_ptr = word + word_len - 1;
      int is_end_of_sentence = (*end_ptr == ENDS_SENTENCE);
      Node *curr_node = add_to_database (markov_chain, word);
      if (!curr_node)
      {
        return 1;
      }
      if (prev_node!=NULL)
      {
        if (add_node_to_frequency_list (prev_node, curr_node->data)==1)
        {
          return 1;
        }
      }
      if (is_end_of_sentence)
      {prev_node = NULL;}
      else
      {prev_node = curr_node->data;}
      word = strtok (NULL, DELIMITERS);
      count_words++;
    }
  }
  return 0;
}


/**
 * Print the generated tweets.
 * @param markov_chain The MarkovChain to generate tweets from.
 * @param seed The seed for random number generation.
 * @param num_tweets The number of tweets to generate.
 */
void print_tweets(int num_tweets, MarkovChain* markov_chain)
{
  for (int i = 0; i < num_tweets; ++i)
  {
    MarkovNode *first_markov_node = get_first_random_node (markov_chain);
    printf ("Tweet %d:", i + 1);
    generate_tweet (first_markov_node, MAX_TWEET_WORDS);
    printf ("\n");
  }
}


/**
 * @param argc number of arguments - 4 or 5
 * @param argv argv[0]: path.   argv[1]: seed value.(unsigned int)
 * argv[2]: number of tweets (positive int). argv[3]:path to text corpus File.
 * argv[4]: optional.(positive int) if given - the number of words to read
 * from the corpus File.
 * @return EXIT_SUCCESS if the program finished successfully,
 * otherwise EXIT_FAILURE
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
  MarkovChain* markov_chain = malloc (sizeof (MarkovChain));
  if (!markov_chain)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  LinkedList* linked_list = malloc (sizeof (LinkedList));
  if (!linked_list)
  {
    free(markov_chain);
    markov_chain=NULL;
    printf (ALLOCATION_ERROR_MASSAGE);
    return EXIT_FAILURE;
  }
  linked_list->first=NULL;
  linked_list->last=NULL;
  linked_list->size=0;
  markov_chain->database = linked_list;
  srand ((unsigned int) strtol (argv[COMMAND_INDEX_SEED], NULL, BASE));
  int words_to_read = 0;
  if (argc==NUM_ARGC_WITH_WORDS)
  {words_to_read = strtol(argv[COMMAND_INDEX_NUM_WORDS], NULL, BASE);}
  if (fill_database (fp, words_to_read, markov_chain)==1)
  {
    printf (ALLOCATION_ERROR_MASSAGE);
    free_database (&markov_chain);
    fclose (fp);
    return EXIT_FAILURE;
  }
  fclose (fp);
  int num_tweets = (int)strtol(argv[COMMAND_NUM_TWEETS], NULL, BASE);
  print_tweets (num_tweets, markov_chain);
  free_database (&markov_chain);
  return EXIT_SUCCESS;
}