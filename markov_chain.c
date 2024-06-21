#include "markov_chain.h"
#include <string.h>
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
  Node *markov_first_llist = (markov_chain->database)->first; //todo -del?
  Node *markov_last_llist = (markov_chain->database)->last; //todo -del
  int markov_size_llist = (markov_chain->database)->size;
  if (markov_chain==NULL || !data_ptr)         //todo - necessary?
    return NULL;
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
  Node *get_Node = get_node_from_database (markov_chain, data_ptr);
  if (get_Node)
    return get_Node;
  MarkovNode *markov_node = calloc(1,sizeof (MarkovNode));
  //todo
  char * data = malloc (strlen (data_ptr)+1);
  //todo
  //todo-free
  data = strcpy (data, data_ptr);
  markov_node->data = data;
  if (add(markov_chain->database, markov_node)==1)
  {
    //todo-free-free
    return NULL;
  }
  return markov_chain->database->last;
}

bool init_freq (MarkovNode *first_node, MarkovNode *second_node)
{
  MarkovNodeFrequency * freq_list = calloc (1, sizeof (MarkovNodeFrequency));
  //TODO
  freq_list[0].markov_node = second_node;
  freq_list[0].frequency = 1;
  first_node->frequency_size = 1;
  return true;
}

bool is_second_in_first (MarkovNode *first_node, MarkovNode *second_node)
{
  for (int i = 0; i < first_node->frequency_size; i++)
    if (first_node->frequency_list[i].markov_node == second_node)
    {
      first_node->frequency_list[i].frequency ++;
      return true;
    }
  return false;
}

bool add_second_to_freq (MarkovNode *first_node, MarkovNode *second_node)
{
  MarkovNodeFrequency * freq_list = realloc(first_node->frequency_list, sizeof
  (MarkovNodeFrequency)*(first_node->frequency_size+1));
  if (!freq_list)
    return false;
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
int add_node_to_frequency_list(MarkovNode *first_node
    , MarkovNode *second_node)
{
  if (first_node->frequency_size==0)
  {
    if (init_freq (first_node, second_node))
      return 0;
    return 1;
  }
  if (is_second_in_first (first_node, second_node))
  {
    return 0;
  }
  if (!add_second_to_freq (first_node, second_node))
    return 1;
  return 0;
}



/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
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