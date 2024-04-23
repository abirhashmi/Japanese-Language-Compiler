#include<iostream>
#include<fstream>
#include<string>
#include <map>
#include <set>
#include <cstdlib>
#include "parser.h"
using namespace std;
// Parser starts line 231

bool word(string s) {

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
  set<char> vowel;
  vowel.insert('a');
  vowel.insert('e');
  vowel.insert('i');
  vowel.insert('o');
  vowel.insert('u');
  set<char> cons1;
  cons1.insert('d');
  cons1.insert('j');
  cons1.insert('w');
  cons1.insert('y');
  cons1.insert('z');
  set<char> cons2;
  cons2.insert('b');
  cons2.insert('g');
  cons2.insert('h');
  cons2.insert('k');
  cons2.insert('m');
  cons2.insert('p');
  cons2.insert('r');

  // Sets initial state and character position to 0
  int state = 0;
  int charpos = 0;

  // Loops through each character
  while (s[charpos] != '\0') {
    // Transitions to state qsa on d, j, w, y, z
    if ((state == 0 || state == 6 || state == 7) &&
        (cons1.find(s[charpos]) != cons1.end())) {
      state = 5;
    }
    // Transitions to state qsa on h
    else if ((state == 2 || state == 4) && (s[charpos] == 'h')) {
      state = 5;
    }
    // Transitions to state qsa on s
    else if ((state == 3) && (s[charpos] == 's')) {
      state = 5;
    }
    // Transitions to state qsa on y
    else if ((state == 1) && (s[charpos] == 'y')) {
      state = 5;
    }
    // Transitions to state qc on c
    else if ((state == 0 || state == 6 || state == 7) && (s[charpos] == 'c')) {
      state = 4;
    }
    // Transitions to state qyqo on n
    else if ((state == 6) && (s[charpos] == 'n')) {
      state = 7;
    }
    // Transitions to state qy on b, g, h, k, m, n, p, r
    else if ((state == 0 || state == 7) &&
             (cons2.find(s[charpos]) != cons2.end() || s[charpos] == 'n')) {
      state = 1;
    }
    // Transitions to state qy on b, g, h, k, m, p, r
    else if ((state == 6) && (cons2.find(s[charpos]) != cons2.end())) {
      state = 1;
    }
    // Transitions to state qt on t
    else if ((state == 0 || state == 6 || state == 7) && (s[charpos] == 't')) {
      state = 3;
    }
    // Transitions to state qs on s
    else if ((state == 0 || state == 6 || state == 7) && (s[charpos] == 's')) {
      state = 2;
    }
    // Transitions to state q0q1 on a, e, i, o, u
    else if ((state == 0 || state == 1 || state == 2 || state == 3 ||
              state == 5 || state == 6 || state == 7) &&
             (vowel.find(s[charpos]) != vowel.end())) {
      state = 6;
    }

    // Invalid transition, return false
    else {
      return false;
    }

    // Increments character position by 1
    charpos++;
  } // end of while

  // Returns true if we are in final state, otherwise return false
  return (state == 6 || state == 7);
}

// PERIOD DFA
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
bool period(string s) {
  // Checks if the string is a period character
  // Returns true if it is, otherwise false
  return (s == ".");
}

// ------ Three  Tables -------------------------------------

// TABLES Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi

// List of token types
enum tokentype {
  ERROR,
  WORD1,
  WORD2,
  PERIOD,
  EOFM,
  VERB,
  VERBNEG,
  VERBPAST,
  VERBPASTNEG,
  IS,
  WAS,
  OBJECT,
  SUBJECT,
  DESTINATION,
  PRONOUN,
  CONNECTOR
};

// Display names for tokens
string tokenName[30] = {"ERROR",       "WORD1",       "WORD2",   "PERIOD",
                        "EOFM",        "VERB",        "VERBNEG", "VERBPAST",
                        "VERBPASTNEG", "IS",          "WAS",     "OBJECT",
                        "SUBJECT",     "DESTINATION", "PRONOUN", "CONNECTOR"};

class TokenMapper{
public:
  TokenMapper(){
    // Verb Markers
    reservedWordsTemplate["masu"] = VERB;
    reservedWordsTemplate["masen"] = VERBNEG;
    reservedWordsTemplate["mashita"] = VERBPAST;
    reservedWordsTemplate["masendeshita"] = VERBPASTNEG;
    reservedWordsTemplate["desu"] = IS;
    reservedWordsTemplate["deshita"] = WAS;

    // Particles
    reservedWordsTemplate["o"] = OBJECT;
    reservedWordsTemplate["wa"] = SUBJECT;
    reservedWordsTemplate["ni"] = DESTINATION;
    reservedWordsTemplate["watashi"] = PRONOUN;
    reservedWordsTemplate["anata"] = PRONOUN;
    reservedWordsTemplate["kare"] = PRONOUN;
    reservedWordsTemplate["kanojo"] = PRONOUN;
    reservedWordsTemplate["sore"] = PRONOUN;
    reservedWordsTemplate["mata"] = CONNECTOR;
    reservedWordsTemplate["soshite"] = CONNECTOR;
    reservedWordsTemplate["shikashi"] = CONNECTOR;
    reservedWordsTemplate["dakara"] = CONNECTOR;
    reservedWordsTemplate["eofm"] = EOFM;
  }
  map<string, tokentype> reservedWordsTemplate;
};
// Reserved words table, reservedWords is pointer to template created above.
TokenMapper tokenMapper;
map<string, tokentype>& reservedWords = tokenMapper.reservedWordsTemplate;


// ------------ Scanner and Driver -----------------------

ifstream fin; // global stream for reading from the input file

// Scanner processes only one word each time it is called
// Gives back the token type and the word itself
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
int scanner(tokentype &tt, string &w) {
  // Grab the next word from the file via fin
  fin >> w;

  // Prints the current word the scanner is checking.
  cout << "Scanner called using word: " << w << endl;
  
  // Checks if the word is eofm
  if (w == "eofm") {
    // Sets token type to EOFM and returns 0 to exit function
    tt = EOFM;
    return 0;
  }

  // Checks if the word ends in E or I
  if (w[w.length() - 1] == 'E' || w[w.length() - 1] == 'I') {
    // Sets token type to WORD2 and returns 0 to exit function
    tt = WORD2;
    return 0;
  }

  // Checks if string is a word
  if (word(w)) {
    // Finds the word in the reservedWords table
    map<string, tokentype>::iterator new_word = reservedWords.find(w);

    // If the word was found it sets the token type to it
    if (new_word != reservedWords.end()) {
      tt = new_word->second;
    }

    // Otherwise the token type is WORD1
    else {
      tt = WORD1;
    }
  }

  // Checks if string is the period character
  else if (period(w)) {
    // Sets token type to PERIOD
    tt = PERIOD;
  }

  // Else the string is not in language
  else {
    // Sets token type to ERROR and prints error
    tt = ERROR;
    cout << endl << "Lexical error: " << w << " is not a valid token" << endl;
  }

  // Returns 0 to exit function
  return 0;

} // the end of scanner

/* INSTRUCTION:  Complete all ** parts.
   You may use any method to connect this file to scanner.cpp
   that you had written.  
  e.g. You can copy scanner.cpp here by:
          cp ../ScannerFiles/scanner.cpp .  
       and then append the two files into one: 
          cat scanner.cpp parser.cpp > myparser.cpp
*/

//=================================================
// File parser.cpp written by Group Number: 5
//=================================================

// ----- Four Utility Functions and Globals -----------------------------------

// Variables for the current token type, current token string, and token checker after match function.
tokentype saved_token;
string saved_lexeme;
bool token_available;

// Need syntaxerror1 and syntaxerror2 functions (each takes 2 args)
// to display syntax error messages as specified by me.  

// Type of error: match fails calls this error
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi 
void syntaxerror1(tokentype expect){
  // Prints the syntax error and exits with failure.
  cout << endl << "SYNTAX ERROR: expected " << tokenName[expect] << " but found " << saved_lexeme << endl;
  exit(1);
}

// Type of error: switch default in a parser function calls this error
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void syntaxerror2(string parser_function) {
  // Prints the syntax error and exits with failure.
  cout << endl << "SYNTAX ERROR: unexpected " << saved_lexeme << " found in " << parser_function << endl;
  exit(1);
}

// Need the updated match and next_token with 2 global vars
// saved_token and saved_lexeme

// Purpose: Used to look ahead at the next token.
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
tokentype next_token(){
  // Checks if the next token is available.
  if(!token_available){
      // If it is we call the scanner on the current token.
      scanner(saved_token, saved_lexeme);
      // Update token checker variable.
      token_available = true;
  }

  // Return the scanned token's type.
  return saved_token;
}

// Purpose: Used to get he next scanner token.
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
bool match(tokentype expected) {

  // Checks if the next token is viable.
  if(next_token() != expected){
    // If it isn't it will return false with syntax error.
    syntaxerror1(expected);
    return false;
  }
    
  else{
    // Otherwise update token checker variable.
    token_available = false;
    // And print that the token matched then return true.
    cout << "Matched " << tokenName[expected] << endl;
    return true;
  }
}

// ----- RDP functions - one per non-term -------------------

// Make each non-terminal into a function here
// Be sure to put the corresponding grammar rule above each function
// Be sure to put the name of the programmer above each function

// Grammar: <story> ::= <s> { <s> } 
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void story(){
  cout << "Processing <story>" << endl << endl;

  // First call s.
  s();

  // Loop through until story finishes parsing.
  while(true){
    switch(next_token()){
      // Any of these cases means we will call s again.
      case CONNECTOR:
      case WORD1:
      case PRONOUN:
        s();
        break;

      // story finished parsing so we print success and return.
      default:
        cout << endl << "Successfully parsed <story>." << endl;
        return;
      
    }
  }
}

// Grammar: <s> ::= [CONNECTOR] <noun> SUBJECT <aftersubject>
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void s(){
  cout << "Processing <s>" << endl;

  // Checks for optional Connector.
  if(next_token() == CONNECTOR){
    match(CONNECTOR);
  }

  // Then calls and matches the rest of the grammar.
  noun();
  match(SUBJECT);
  after_subject();
}

// Grammar: <after subject> ::= <verb> <tense> PERIOD | <noun> <after noun>
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void after_subject(){
  cout << "Processing <after_subject>" << endl;

  // Get's the next token.
  switch(next_token()){
    // Checks if it matches right side.
    case WORD1:
    case PRONOUN:
    noun();
    after_noun();
    break;

    // Checks if it matches left side.
    case WORD2:
      verb();
      tense();
      match(PERIOD);
      break;

    // Check failed so we call syntax error.
    default:
      syntaxerror2("after_subject");
  }
}

// Grammar: <after noun> ::= <be> PERIOD | DESTINATION <verb> <tense> PERIOD | OBJECT <after object>
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void after_noun(){
  cout << "Processing <after_noun>" << endl;

  // Get next token.
  switch(next_token()){
    // Checks if it matches left side.
    case IS:
    case WAS:
      be();
      match(PERIOD);
      break;

    // Checks if it matches middle.
    case DESTINATION:
      match(DESTINATION);
      verb();
      tense();
      match(PERIOD);
      break;

    // Checks if it matches right side.
    case OBJECT:
      match(OBJECT);
      after_object();
      break;

    // Check failed so call syntax error.
    default:
      syntaxerror2("after_noun");
  }
}

// Grammar: <after object> ::= <verb> <tense> PERIOD | <noun> DESTINATION <verb> <tense> PERIOD
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void after_object(){
  cout << "Parsing <after_object>" << endl;

  // Get the next token.
  switch(next_token()){
    // Checks if it matches right side.
    case WORD1:
    case PRONOUN:
      noun();
      match(DESTINATION);
      verb();
      tense();
      match(PERIOD);
      break;

    // Checks if it matches left side.
    case WORD2:
      verb();
      tense();
      match(PERIOD);
      break;

    // Check failed os call syntax error.
    default:
      syntaxerror2("after_object");
  }
}

// Grammar: WORD1 | PRONOUN
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void noun(){
  cout << "Parsing <noun>" << endl;

  // Get next token.
  switch(next_token()){
    // Checks if it matches left side.
    case WORD1:
      match(WORD1);
      break;

    // Checks if it matches right side.
    case PRONOUN:
      match(PRONOUN);
      break;

    // Check failed so call syntax error.
    default:
      syntaxerror2("noun");
  }
}

// Grammar: WORD2
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void verb(){
  cout << "Parsing <verb>" << endl;

  // Get next token.
  switch(next_token()){
    // Checks if it matches the correct token type.
    case WORD2:
      match(WORD2);
      break;

    // Check failed so call syntax error.
    default:
      syntaxerror2("verb");
  }
}

// Grammar: VERBPAST | VERBPASTNEG | VERB | VERNEG
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void tense(){
  cout << "Parsing <noun>" << endl;

  // Get next token.
  switch(next_token()){
    // Check if it matches far left side.
    case VERBPAST:
      match(VERBPAST);
      break;

    // Checks if it matches middle left side.
    case VERBPASTNEG:
      match(VERBPASTNEG);
      break;

    // Checks if it matches middle right side.
    case VERB:
    match(VERB);
    break;

    // Checks if it matches far right side.
    case VERBNEG:
    match(VERBNEG);
    break;

    // Check failed so call syntax error.
    default:
      syntaxerror2("tense");
  }
}

// Grammar: IS | WAS
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void be(){
  cout << "Parsing <be>" << endl;

  // Get next token.
  switch(next_token()){
    // Checks if it matches left side.
    case IS:
      match(IS);
      break;

    // Checks if it matches right side.
    case WAS:
      match(WAS);
      break;

    // Check failed so call syntax error.
    default:
      syntaxerror2("be");
  }
}

string filename;

//----------- Driver ---------------------------

// The new test driver to start the parser
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
int main()
{
  // Prints group # and group members names.
  cout << "Group:\t\t5" << endl << "Members: \tBlake Jaeger\n\t\t\tGregorio Gomez\n\t\t\tAbir Hashmi" << endl; 
  
  cout << "Enter the input file name: ";
  cin >> filename;
  fin.open(filename.c_str());

  
  //calls the <story> to start parsing
  story();
  //closes the input file 
  fin.close();

}// end
// require no other input files!
// syntax error EC requires producing errors.txt of error messages
// tracing On/Off EC requires sending a flag to trace message output functions
