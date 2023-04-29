#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

struct node {
    node *next[26];
    bool end_of_word;
};

#define INCORRECT 1
#define CORRECT 0
#define END_OF_WORD true

node *loadDictionary (char *dictionary_name);
void beginSummary (std::ofstream& outfile, char *dictionary_name, char* file_name);
node *createNode();
void addToDictionary(node *dictionary, std::string word);
int checkSpelling(node *dictionary, std::string word, std::ofstream& outfile);
int letterKey (char letter);
void misspelled (std::string word, std::ofstream& outfile);
void closeSummary(std::ofstream& outfile, int incorrect_words, std::chrono::microseconds program_duration, std::chrono::microseconds load_duration, std::chrono::microseconds spell_check_duration, std::chrono::microseconds free_duration);
void unloadDictionary(node *dictionary);

int main (int argc, char *argv[]) {
    //Error check CLA's
    if (argc != 4) {
        std::cout << "ERROR\nUsage: ./main [dictionary].txt [file].txt [output_name].txt";
        return 1;
    }
    

    char *dictionary_name = argv[1];
    char *input_file_name = argv[2];
    char *output_file_name = argv[3];

    //Start Timer
    std::chrono::time_point<std::chrono::high_resolution_clock> start_program, start_load_memory, stop_load_memory, start_free, stop_free, start_spell_check, stop_spell_check, stop_program;
    start_program = std::chrono::high_resolution_clock::now();


    //Load dictionary into memory
    start_load_memory = std::chrono::high_resolution_clock::now();
    node *dictionary = loadDictionary(dictionary_name);
    stop_load_memory = std::chrono::high_resolution_clock::now();

    if (dictionary == NULL) {
        return 1;
    }

    //Open file
    std::ifstream infile(input_file_name);

    if(!infile.is_open()) {
        std::cout << "Failed to open file: " << input_file_name << '\n';
        return 1;
    }

    //Going to add words to a summary file
    std::ofstream outfile;
    outfile.open(output_file_name);
    beginSummary(outfile, dictionary_name, input_file_name);

    //Pass words to checkSpelling
    
    std::string word;
    int incorrect_words = 0;
    start_spell_check = std::chrono::high_resolution_clock::now();
    while (infile >> word) {
        incorrect_words += checkSpelling(dictionary, word, outfile);
    }
    stop_spell_check = std::chrono::high_resolution_clock::now();

    //Free memory
    start_free = std::chrono::high_resolution_clock::now();
    unloadDictionary(dictionary);
    stop_free = std::chrono::high_resolution_clock::now();

    //End timer
    stop_program = std::chrono::high_resolution_clock::now();

    std::chrono::microseconds program_duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_program - start_program);
    std::chrono::microseconds load_duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_load_memory - start_load_memory);
    std::chrono::microseconds spell_check_duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_spell_check - start_spell_check);
    std::chrono::microseconds free_duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_free - start_free);

    //Finish Summary
    closeSummary(
        outfile, 
        incorrect_words, 
        program_duration,
        load_duration,
        spell_check_duration,
        free_duration
    );

    //Close file
    outfile.close();

    return 0;
}

node *loadDictionary (char * dictionary_name) {
    //Opening and reading file into array of word
    std::ifstream infile(dictionary_name);
    std::string word;

    if(!infile.is_open()) {
        std::cout << "Failed to open: " << dictionary_name << '\n';
        return NULL;
    }
    
    node *dictionary = createNode();
    while (infile >> word) {
        addToDictionary(dictionary, word);
    }

    return dictionary;
}

void beginSummary (std::ofstream& outfile, char *dictionary_name, char* input_file_name) {
    outfile << "Welcome to Joshua Louttit's spellchecker!!" << '\n';
    outfile << "Please see some of my other work at https://github.com/joshualouttit or contact me through linked in https://www.linkedin.com/in/joshua-louttit-534903225/ :)\n\n";
    outfile << input_file_name << " has the following misspelled words using " << dictionary_name << " as a dictionary.\n\n";
}

node *createNode() {
    node *NewNode = new node;

    for (int i = 0; i < 26; i++) {
        NewNode->next[i] = NULL;
    }

    NewNode->end_of_word = false;

    return NewNode;
}

void addToDictionary(node *dictionary, std::string word) {
    //Go through word and add each letter
    int word_length = word.length();
    node *current_node = dictionary;

    for (int i = 0; i < word_length; i++) {
        int letter_key = letterKey(word[i]);
        if (letter_key < 26) {
            //Two cases
            //Need to update current_node in both cases
            //1. current_node does not have a place for this letter 
            //2. Already allocated memory
            if (current_node->next[letter_key] == NULL) {
                //Create New Node
                node *new_node = createNode();
                //Add pointer to next node
                current_node->next[letter_key] = new_node;
                current_node = new_node;
    
            } else {
                //No need to make changes - dictionary already indicates this is a possible next letter
                current_node = current_node->next[letter_key];
            }
        }
    }

    //Need to add in end_of_word signal
    //When we reach here need to add end_of_word as possibility to current node
    current_node->end_of_word = true;


    return;
}

int checkSpelling(node *dictionary, std::string word, std::ofstream& outfile) {
    int word_length = word.length();

    node *current_level = dictionary;

    for (int i = 0; i < word_length; i++) {
        //Define letter key
        int letter_key = letterKey(word[i]);

        if (current_level == NULL) {
            misspelled(word, outfile);
            return INCORRECT;
        }

        if (letter_key < 26) {
            current_level = current_level->next[letter_key];
        }
    }


    //if the last letter moves it to a NULL pointer we cant go NULL->next or seg fault
    if (current_level == NULL) {
        misspelled(word, outfile);
        return INCORRECT;
    }

    if (current_level->end_of_word != END_OF_WORD) {
        misspelled(word, outfile);
        return INCORRECT;
    }

    return CORRECT;
}

int letterKey (char letter) {
    if (
        letter >= 'a' &&
        letter <= 'z'
    ) {
        return letter - 'a';
    } else if (
        letter >= 'A' &&
        letter <= 'Z'
    ) {
        return letter - 'A';
    } else {
        return 27;
    }
}

void misspelled (std::string word, std::ofstream& outfile) {
    outfile << word << '\n';
    return;
}

void closeSummary(std::ofstream& outfile, int incorrect_words, std::chrono::microseconds program_duration, std::chrono::microseconds load_duration, std::chrono::microseconds spell_check_duration, std::chrono::microseconds free_duration) {
    outfile << "\nThe program found " << incorrect_words << " words that are not are not the dictionary provided in " << program_duration.count() << " microseconds.\n\n";
    outfile << "Loading memory: " << load_duration.count() << " microseconds\n";
    outfile << "Spellchecking: " << spell_check_duration.count() << " microseconds\n";
    outfile << "Freeing memory: " << free_duration.count() << " microseconds\n";
    return;
}

void unloadDictionary(node *dictionary) {
    for (int i = 0; i < 26; i++) {
        if (dictionary->next[i] != NULL) {
            unloadDictionary(dictionary->next[i]);
        }
    }

    delete dictionary;
}