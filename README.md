# Tweets Generator (C)
A Markov chain text generation tool. This is a console program that reads a corpus of tweets and then generates new pseudo-tweets. It uses a Markov Chain algorithm: essentially, it learns the probability of word sequences from the input text and then produces random sentences (“tweets”) that sound like the source. This project was implemented in C, highlighting low-level memory management and data structure manipulation (linked lists to store the chain’s state).

## How it Works
1. **Input**: Reads a file containing a corpus of tweets.
2. **Learning**: Builds a Markov Chain by analyzing the dataset to understand word transitions and their frequencies.
3. **Output**: Generates new tweets by selecting words probabilistically based on the learned word sequences.

## How to run
1. Make sure a C compiler (GCC) is installed. Clone the repository to your machine.
2. Use the provided CMake configuration to build the program: run cmake . followed by make, which will produce an executable (alternatively, compile manually with gcc -std=c99 *.c -o tweets_generator).
3. Run the program providing a text file of tweets and parameters. For example: ./tweets_generator tweets_source.txt 42 5 – this will read tweets_source.txt, use 42 as a random seed, and generate 5 random tweets. (The repository includes a sample justdoit_tweets.txt file which can be used for a quick test.)

## Illustration
As an illustration, the core of the Markov chain algorithm picks the next word based on previous word frequencies:

```c
// Choosing the next word based on frequency in the Markov Chain (C code snippet)
MarkovNode* get_next_random_node(MarkovNode *cur_node) {
    int i = get_random_number(get_sum_freq_list(cur_node));
    MarkovNode* next_node = get_i_word_in_freq_list(cur_node, i);
    return next_node;
}
```
This project strengthened my understanding of pointers and memory allocation (for example, handling linked list nodes and frequency arrays) and taught me to write robust C code (checking for file I/O errors, argument validation, etc.). Running the program will output novel “tweets” that mimic the style of the input text.
