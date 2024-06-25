#include "markov_chain.h"
#include <string.h>

#define ENDS_SENTENCE '.'


/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}


/**
* Check if data_ptr is in database. If so, return the Node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the data to look for
 * @return Pointer to the Node wrapping given data, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr)
{
  if (!markov_chain || !markov_chain->database || !data_ptr)
  {return NULL;}
  int markov_size_llist = (markov_chain->database)->size;
  Node *markov_curr_llist = (markov_chain->database)->first;
  for (int i = 0; i<markov_size_llist; i++)
  {
    if (strcmp ((markov_curr_llist->data)->data, data_ptr)==0)
    {
      return markov_curr_llist;
    }
    markov_curr_llist = markov_curr_llist->next;
  }
  return NULL;
}


/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the data to look for
 * @return Node wrapping given data_ptr in given chain's database,
 * returns NULL in case of memory allocation failure.
 */
Node* add_to_database(MarkovChain *markov_chain, char *data_ptr)
{
  if (!data_ptr)
  {
    return NULL;
  }
  Node *get_node = get_node_from_database (markov_chain, data_ptr);
  if (get_node)
  {return get_node;}
  MarkovNode *new_markov_node = calloc(1,sizeof (MarkovNode));
  if (!new_markov_node)
  {
    new_markov_node=NULL;
    return NULL;
  }
  new_markov_node->data=NULL;
  new_markov_node->frequency_list=NULL;
  new_markov_node->frequency_size=0;
  new_markov_node->data = malloc (strlen (data_ptr)+1);
  if (!new_markov_node->data)
  {
    free (new_markov_node);
    new_markov_node = NULL;
    return NULL;
  }
  strcpy (new_markov_node->data, data_ptr);
  if (add(markov_chain->database, new_markov_node)==1)
  {
    free (new_markov_node->data);
    free (new_markov_node);
    new_markov_node->data = NULL;
    new_markov_node = NULL;
    return NULL;
  }
  return markov_chain->database->last;
}


/**
 * Initialize the frequency list of the first_node to contain the second_node.
 * @param first_node The node to initialize the frequency list for
 * @param second_node The node to add to the frequency list
 * @return true if initialization was successful, false otherwise
 */
bool init_freq (MarkovNode *first_node, MarkovNode *second_node)
{
  MarkovNodeFrequency * freq_list = calloc (1, sizeof (MarkovNodeFrequency));
  if (!freq_list)
  {
    freq_list=NULL;
    return false;
  }
  first_node->frequency_list=freq_list;
  freq_list[0].markov_node = second_node;
  freq_list[0].frequency = 1;
  first_node->frequency_size = 1;
  return true;
}


/**
 * Check if the second_node is already in the frequency list of the first_node.
 * If so, increment its frequency.
 * @param first_node The node whose frequency list is to be checked
 * @param second_node The node to check for in the frequency list
 * @return true if the second_node is in the frequency list, false otherwise
 */
bool is_second_in_first (MarkovNode *first_node, MarkovNode *second_node)
{
  for (int i = 0; i < first_node->frequency_size; i++)
  {
    if (first_node->frequency_list[i].markov_node == second_node)
    {
      first_node->frequency_list[i].frequency ++;
      return true;
    }
  }
  return false;
}


/**
 * Add the second_node to the frequency list of the first_node.
 * @param first_node The node to add the frequency to
 * @param second_node The node to be added to the frequency list
 * @return true if addition was successful, false otherwise
 */
bool add_second_to_freq (MarkovNode *first_node, MarkovNode *second_node)
{
  MarkovNodeFrequency * freq_list = realloc(first_node->frequency_list, sizeof
                        (MarkovNodeFrequency)*(first_node->frequency_size+1));
  if (!freq_list)
  {return false;}
  first_node->frequency_list = freq_list;
  first_node->frequency_size++;
  first_node->frequency_list[first_node->frequency_size-1].markov_node =
                                                                  second_node;
  first_node->frequency_list[first_node->frequency_size-1].frequency = 1;
  return true;
}


/**
 * Add the second markov_node to the frequency list of the first markov_node.
 * If already in list, update it's occurrence frequency value.
 * @param first_node
 * @param second_node
 * @return success/failure: 0 if the process was successful, 1 if in
 * case of allocation error.
 */
int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node)
{
  if (first_node->frequency_size==0)
  {
    if (init_freq (first_node, second_node))
    {return 0;}
    return 1;
  }
  if (is_second_in_first (first_node, second_node))
  {return 0;}
  if (!add_second_to_freq (first_node, second_node))
  {return 1;}
  return 0;
}


/**
 * Free the database and all its content.
 * @param ptr_chain Pointer to the MarkovChain to be freed
 */
void free_database(MarkovChain ** ptr_chain)
{
  Node *curr = (*ptr_chain)->database->first;
  for (int i = 0; i< (*ptr_chain)->database->size; i++)
  {
    free(curr->data->data);
    curr->data->data = NULL;
    free(curr->data->frequency_list);
    curr->data->frequency_list = NULL;
    free (curr->data);
    curr->data = NULL;
    Node * node_to_free =curr;
    curr = curr->next;
    free (node_to_free);
  }
  free ((*ptr_chain)->database);
  (*ptr_chain)->database=NULL;
  free (*ptr_chain);
  *ptr_chain=NULL;
}

/**
 * Get the i-th word in the linked list starting from first_node.
 * @param first_node The first node in the linked list
 * @param i The index of the word to retrieve
 * @return The MarkovNode at the i-th position
 */
MarkovNode* get_i_word_in_lls(Node *first_node, int i)
{
  Node *curr_node = first_node;
  for (int j = 0; j < i; j++)
  {curr_node = curr_node->next;}
  return curr_node->data;
}


/**
 * Get the i-th word in the frequency list.
 * @param freq_list The frequency list
 * @param freq_size The size of the frequency list
 * @param i The index of the word to retrieve
 * @return The MarkovNode at the i-th position in the frequency list
 */
MarkovNode* get_i_word_in_freq_list(MarkovNodeFrequency *freq_list,
                                   int freq_size, int i)
{
  int sum_freqs = 0;
  for (int j = 0; j < freq_size; j++)
  {
    sum_freqs = sum_freqs + freq_list[j].frequency;
    if (i < sum_freqs)
    {
      return freq_list[j].markov_node;
    }
  }
  return NULL;
}


/**
 * Check if a word ends with a sentence-ending character.
 * @param word The word to check
 * @return true if the word ends with a sentence-ending character,
 * false otherwise
 */
bool is_ends_sentence(const char* word)
{
  int len = (int)strlen (word);
  if (word[len-1] == ENDS_SENTENCE)
  {return true;}
  return false;
}


/**
 * Get one random MarkovNode from the given markov_chain's database.
 * @param markov_chain
 * @return the random MarkovNode
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
  MarkovNode* i_markov_node;
  do
  {
    int i = get_random_number ((markov_chain->database)->size);
    i_markov_node = get_i_word_in_lls((markov_chain->database)
                                                      ->first, i);
  }
  while (is_ends_sentence (i_markov_node->data));
  return i_markov_node;
}


/**
 * Calculate the sum of frequencies in the frequency list of the given
 * MarkovNode. This sum represents the total number of transitions from the
 * given node to all its subsequent nodes.
 * @param cur_random_node Pointer to the MarkovNode whose frequency list
 * is to be summed.
 * @return The total sum of frequencies in the frequency list.
 */
int get_sum_freq_list(MarkovNode *cur_random_node)
{
  int count_num_of_shows = 0;
  for (int i = 0; i < cur_random_node->frequency_size; i++)
  {count_num_of_shows += (cur_random_node->frequency_list[i]).freqxxwuency;}
  return count_num_of_shows;
}

/**
 * Choose randomly the next MarkovNode, depend on it's occurrence frequency.
 * @param cur_markov_node current MarkovNode
 * @return the next random MarkovNode
 */
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node)
{
  int i = get_random_number (get_sum_freq_list (cur_markov_node));
  MarkovNode* i_markov_node = get_i_word_in_freq_list
      (cur_markov_node->frequency_list, cur_markov_node->frequency_size, i);
  return i_markov_node;
}


/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence must have at least 2 words in it.
 * @param first_node markov_node to start with
 * @param  max_length maximum length of chain to generate
 */
void generate_tweet(MarkovNode *first_node, int max_length)
{
  MarkovNode* cur_markov_node = first_node;
  while (!is_ends_sentence (cur_markov_node->data) && max_length!=0)
  {
    printf (" ");
    printf ("%s", cur_markov_node->data);
    cur_markov_node = get_next_random_node (cur_markov_node);
    if (is_ends_sentence (cur_markov_node->data))
    {
      printf (" %s", cur_markov_node->data);
      break;
    }
    max_length--;
  }
}