#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include <set>
using namespace std;

/* Look for all **'s and complete them */

//=====================================================
// File scanner.cpp written by: Group Number: 5
//=====================================================

// --------- Two DFAs ---------------------------------

// WORD DFA 
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
// RE:  (vowel | vowel n | consonant vowel | consonant vowel n | consonant-pair vowel | consonant-pair vowel n)^+
//for building the executable a makefile was utilized 
//commands include: make scanner (to build the executable)
//commands also include: ./scanner (to run the executable)
//in which it prompts the user to "Enter the input file name:"
//to which the user should type scannertest1 or scannertest2 to get the technically correct output
bool word (string s)
{

  /* States:
    q0 = 0
    qy = 1
    qs = 2
    qt = 3
    qc = 4
    qsa = 5
    q0q1 = 6
    qyq0 = 7
  */

  // Sets for vowels, consonants 1 and consonants 2
  set<char> vowel = {'a', 'e', 'i', 'o', 'u'};
  set<char> cons1 = {'d', 'j', 'w', 'y', 'z'};
  set<char> cons2 = {'b', 'g', 'h', 'k', 'm', 'p', 'r'};

  // Sets initial state and character position to 0
  int state = 0;
  int charpos = 0;

  // Loops through each character
  while (s[charpos] != '\0') 
    {
      // Transitions to state qsa on d, j, w, y, z
      if((state == 0 || state == 6 || state == 7) && (cons1.find(s[charpos]) != cons1.end())){
        state = 5;
      }
        // Transitions to state qsa on h 
      else if((state == 2 || state == 4) && (s[charpos] == 'h')){
        state = 5;
      }
        // Transitions to state qsa on s
      else if((state == 3) && (s[charpos] == 's')){
        state = 5;
      }
        // Transitions to state qsa on y
      else if((state == 1) && (s[charpos] == 'y')){
        state = 5;
      }
        // Transitions to state qc on c
      else if((state == 0 || state == 6 || state == 7) && (s[charpos] == 'c')){
        state = 4;
      }
        // Transitions to state qyqo on n
      else if((state == 6) && (s[charpos] == 'n')){
        state = 7;
      }
        // Transitions to state qy on b, g, h, k, m, n, p, r
      else if((state == 0 || state == 7) && (cons2.find(s[charpos]) != cons2.end() || s[charpos] == 'n')){
        state = 1;
      }
        // Transitions to state qy on b, g, h, k, m, p, r
      else if((state == 6) && (cons2.find(s[charpos]) != cons2.end())){
        state = 1;
      }
        // Transitions to state qt on t
      else if((state == 0 || state == 6 || state == 7) && (s[charpos] == 't')){
        state = 3;
      }
        // Transitions to state qs on s
      else if((state == 0 || state == 6 || state == 7) && (s[charpos] == 's')){
        state = 2;
      }
        // Transitions to state q0q1 on a, e, i, o, u
      else if((state == 0 || state == 1 || state == 2 || state == 3 || state == 5 || state == 6 || state == 7) && (vowel.find(s[charpos]) != vowel.end())){
        state = 6;
      }
        
      // Invalid transition, return false
      else{
        return false;
      }
      
      // Increments character position by 1
      charpos++;
    }//end of while

  // Returns true if we are in final state, otherwise return false
  return (state == 6 || state == 7);
}

// PERIOD DFA 
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
bool period (string s)
{  
  // Checks if the string is a period character
  // Returns true if it is, otherwise false
  return (s == ".");
}

// ------ Three  Tables -------------------------------------

// TABLES Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi

// List of token types
enum tokentype {ERROR, WORD1, WORD2, PERIOD, EOFM, VERB, VERBNEG, VERBPAST, VERBPASTNEG, IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, CONNECTOR};

// Display names for tokens
string tokenName[30] = {"ERROR", "WORD1", "WORD2", "PERIOD", "EOFM", "VERB", "VERBNEG", "VERBPAST", "VERBPASTNEG", "IS", "WAS", "OBJECT", "SUBJECT", "DESTINATION", "PRONOUN", "CONNECTOR"}; 

// Reserved words table
map <string, tokentype> reservedWords = { 
  // Verb Markers
  {"masu", VERB},
  {"masen", VERBNEG},  
  {"mashita", VERBPAST},
  {"masendeshita", VERBPASTNEG},
  {"desu", IS},
  {"deshita", WAS},

  // Particles
  {"o", OBJECT},
  {"wa", SUBJECT},
  {"ni", DESTINATION},
  {"watashi", PRONOUN},
  {"anata", PRONOUN},
  {"kare", PRONOUN},
  {"kanojo", PRONOUN},
  {"sore", PRONOUN},
  {"mata", CONNECTOR},
  {"soshite", CONNECTOR},
  {"shikashi", CONNECTOR},
  {"dakara", CONNECTOR},
  {"eofm", EOFM}
};

// ------------ Scanner and Driver ----------------------- 

ifstream fin;  // global stream for reading from the input file

// Scanner processes only one word each time it is called
// Gives back the token type and the word itself
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
int scanner(tokentype& tt, string& w)
{
  // Grab the next word from the file via fin
  fin >> w;
  
  // Checks if the word is eofm
  if(w == "eofm"){
    // Sets token type to EOFM and returns 0 to exit function
    tt = EOFM;
    return 0;
  }

  // Checks if the word ends in E or I
  if(w.back() == 'E' || w.back() == 'I'){
    // Sets token type to WORD2 and returns 0 to exit function
    tt = WORD2;
    return 0;
  }

  // Checks if string is a word
  if(word(w)){
    // Finds the word in the reservedWords table
    auto new_word = reservedWords.find(w);
    
    // If the word was found it sets the token type to it
    if(new_word != reservedWords.end()){
      tt = new_word -> second;
    }
    
    // Otherwise the token type is WORD1
    else{
      tt = WORD1;
    }
  }
    
  // Checks if string is the period character
  else if (period(w)){
    // Sets token type to PERIOD
    tt=PERIOD;
  }
    
  // Else the string is not in language
  else{
    // Sets token type to ERROR and prints error
    tt = ERROR;
    cout << "Lexical error: " << w << " is not a valid token" << endl;
  }
  
  // Returns 0 to exit function
  return 0;
  
}//the end of scanner


void tokenID(tokentype s, const string& b){//prints the tokens identifiable ID for example
  cout << "\"" << b << "\" is token type " << tokenName[s] << endl;//"daigaku" or "kurasu" any of the tokens/words basically are ensured that they are printed
}
// The temporary test driver to just call the scanner repeatedly  
// This will go away after this assignment
// DO NOT CHANGE THIS!!!!!! 
// Done by:  Louis
int main()
{
  tokentype thetype;
  string theword; 
  string filename;

  cout << "Enter the input file name: ";
  cin >> filename;

  fin.open(filename.c_str());

  // the loop continues until eofm is returned.
   while (true)
    {
       scanner(thetype, theword);  // call the scanner which sets
                                   // the arguments  
       if (theword == "eofm") break;  // stop now

      tokenID(thetype, theword);
       //cout << "Type is:" << tokenName[thetype] << endl;
       //cout << "Word is:" << theword << endl;   
    }

   cout << "End of file is encountered." << endl;
   fin.close();

}// end

