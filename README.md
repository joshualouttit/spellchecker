Program summary: A spellchecker built in c++ using a linked tree for fast spellchecking of large documents.

Context: After beginning to program with Harvards CS50 and UNSW's 1511, I really enjoy programming in C. The language makes sense and the writer has such control over the computer directly. unfortunately, C is not a high level language so this is the first project I have attempted to write C++. Inspired by CS50's speller.c problem, I have built a more complex spellchecker in c++. Compared to previous programs I have written this runs extremely quickly clocking about 0.18 - 0.22 seconds with the large.txt dictionary and sample txt files (both taken from Harvards CS50).

So how does it work? The most important part in understanding this program is understanding the design of the dictionary:

**Dictionary Design [loadDictionary() & addToDictionary()]**
The dictionary is designed as a linked tree in order to minimialise the number of actions the computer must complete to determine whether or not a word is misspelt. If the longest word from the dictionary loaded into memory is N, the maximum number of times the computer will step through the linked tree is N times.
 
I can attempted to illustrate how this tree works in diagram.pdf, but essentially works by creating a node for each letter of a specific permutation which then points to all the possible letters that could come after. Consider the word 'cat'

A node is created for 'c' which in the *next array - next[0] points to the section of the tree to which 'ca' could be the first two letters of a word. Please note that after 'a', the next letter will reference a different node, where 'car' and 'cat' are now different parts of the tree. The end_of_word bool indicating whether that specific permutation of letters can create a word. Additionally node, that whilst the end_of_word value may be true, there may still be words that can created after that whilst will use parts of the same linked tree. E.g 'car' and 'cart'. Obviously at a specific node where the next letter is not a possibility, take the node that represents 'ca' and the next letter being 'c' then *next[2] (2 being representing the third letter in the alphabet) will point to NULL. For how this is created please see spellchecker.cpp

The program simply creates this linked tree and then checks every word from a .txt against it.

 Please comment with suggested improvements or with any questions for clarification :).
