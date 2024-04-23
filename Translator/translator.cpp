#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <cstdlib>
#include "translator.h"
using namespace std;
// Translator starts at line 563

/* INSTRUCTION:  copy your parser.cpp here
      cp ../ParserFiles/parser.cpp .
   Then, insert or append its contents into this file and edit.
   Complete all parts.
*/

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

ifstream fin;  // global stream for reading from the input file

// Scanner processes only one word each time it is called
// Gives back the token type and the word itself
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
int scanner(tokentype& tt, string& w) {
// Grab the next word from the file via fin
fin >> w;

// Print the scanned word for debugging
cout << "Scanned called using word: " << w << endl;

// Checks if the word is eofm
if (w == "eofm") {
    // Sets token type to EOFM and returns 0 to exit function
    tt = EOFM;
    return 0;
}

    // Checks if the word ends in E or I
    if (w[w.length() - 1] == 'E' || w[w.length() - 1] == 'I') {
        tt = WORD2;
        // Sets token type to WORD2 and returns 0 to exit function
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
        cout << "Lexical error: " << w << " is not a valid token" << endl;
    }

    // Returns 0 to exit function
    return 0;
}//the end of scanner

/* INSTRUCTION:  Complete all parts.
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
  cout << "Processing <story>" << endl << endl;//to match desired output

  // First call s.
  s();

  // Loop through until story finishes parsing.
  while(true){

    switch(next_token()){
      // Any of these cases means we will call s again.
      case CONNECTOR:// if the next token is a connector.
      case WORD1:// if the next token is a word1
      case PRONOUN:// if the next token is a pronoun
        s();
        break;

      // Check for a period to finish the story.
      case PERIOD:// if the next token is a period
        match(PERIOD);
        break;

      case EOFM:// if the next token is EOFM
        cout << endl << "Successfully parsed <story>." << endl;
        return;
      // story finished parsing so we print success and return.
      default:
        syntaxerror2("story");

    }
  }
}

// Grammar: <s> ::= [CONNECTOR #getEword# #gen(“CONNECTOR”)#] <noun> #getEword# SUBJECT #gen(“ACTOR”)# <after subject>
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void s(){
  cout << "Processing <s>" << endl;//to mimic expected output

  // Checks for optional Connector.
  if(next_token() == CONNECTOR){
    match(CONNECTOR);// if next_token is a connector, it will match.
    getEword();
    gen("CONNECTOR");
  }

  // Then calls and matches the rest of the grammar.
  noun();
  match(SUBJECT);// Matches subject
  gen("ACTOR");
  after_subject();// make a recursive call to after_subject
}

// Grammar: <after subject> ::= <verb> #getEword# #gen(“ACTION”)# <tense> #gen(“TENSE”)# PERIOD | <noun> #getEword# <after noun>
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void after_subject(){
  cout << "Processing <after_subject>" << endl;//to match desired output

  // Get's the next token.
  switch(next_token()){
    // Checks if it matches right side.
    case WORD1:// if the next token is a word1
    case PRONOUN: // if the next token is a pronoun
      noun();
      after_noun();//make a recursive call to after_noun
      break;

    // Checks if it matches left side.
    case WORD2: // if the next token is a word2
      verb();
      tense();
      match(PERIOD);// Matches period
      break;

    // Check failed so we call syntax error.
    default:
      syntaxerror2("after_subject");
  }
}

// Grammar: <after noun> ::= <be> #get(“DESCRIPTION”)# #get(“TENSE”)# PERIOD | DESTINATION #gen(“TO”)# <verb> #getEword# #gen(“ACTION”)# <tense> #gen(“TENSE”)# PERIOD | OBJECT #get(“OBJECT”)# <after object>
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void after_noun(){
  cout << "Processing <after_noun>" << endl;//to match desired output

  // Get next token.
  switch(next_token()){
    // Checks if it matches left side.
    case IS: // if the next token is a IS
    case WAS: // if the next token is a WAS
      be();
      match(PERIOD);// Matches period
      break;

    // Checks if it matches middle.
    case DESTINATION: // if the next token is a DESTINATION
      match(DESTINATION);// Matches destination
      gen("TO");
      verb();// make a recursive call to verb
      tense();// make a recursive call to tense
      match(PERIOD);// Matches period
      break;

    // Checks if it matches right side.
    case OBJECT: // if the next token is a OBJECT
      match(OBJECT);// Matches object
      gen("OBJECT");
      after_object();// make a recursive call to after_object
      break;

    // Check failed so call syntax error.
    default:
      syntaxerror2("after_noun");
  }
}

// Grammar: <after object> ::= <verb> #getEword# #gen(“ACTION”)# <tense> #gen(“TENSE”)# PERIOD | <noun> #genEword# DESTINATION #gen(“TO”)# <verb> #getEword# #gen(“ACTION”)# <tense> #gen(“TENSE”)# PERIOD
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void after_object(){
  cout << "Parsing <after_object>" << endl;

  // Get the next token.
  switch(next_token()){
    // Checks if it matches right side.
    case WORD1: // if the next token is a WORD1
    case PRONOUN: // if the next token is a pronoun
      noun();// make a recursive call to noun
      match(DESTINATION);// Matches destination
      gen("TO");
      verb();// make a recursive call to verb
      tense();// make a recursive call to tense
      match(PERIOD);// Matches period
      break;

    // Checks if it matches left side.
    case WORD2: // if the next token is a WORD2
      verb();// make a recursive call to verb
      tense();// make a recursive call to tense
      match(PERIOD);// Matches period
      break;

    // Check failed os call syntax error.
    default:
      syntaxerror2("after_object");
  }
}

// Grammar: WORD1 | PRONOUN
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void noun(){
  cout << "Parsing <noun>" << endl;//display to match expected output

  // Get next token.
  switch(next_token()){
    // Checks if it matches left side.
    case WORD1: // if the next token is a WORD1
      match(WORD1);// Matches word1
      break;

    // Checks if it matches right side.
    case PRONOUN: // if the next token is a pronoun
      match(PRONOUN);// Matches pronoun
      break;

    // Check failed so call syntax error.
    default:
      syntaxerror2("noun");
  }
  getEword();
}

// Grammar: WORD2
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void verb(){
  cout << "Parsing <verb>" << endl;//display to match desired output 

  // Get next token.
  switch(next_token()){
    // Checks if it matches the correct token type.
    case WORD2: // if the next token is a WORD2
      match(WORD2);// Matches word2
      getEword();
      gen("ACTION");
      break;

    // Check failed so call syntax error.
    default:
      syntaxerror2("verb");
  }
}

// Grammar: VERBPAST | VERBPASTNEG | VERB | VERNEG
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void tense(){
  cout << "Parsing <noun>" << endl;//for desired output to match expected

  // Get next token.
  switch(next_token()){
    // Check if it matches far left side.
    case VERBPAST: // if the next token is a VERBPAST
      match(VERBPAST);// Matches verbpast
      break;

    // Checks if it matches middle left side.
    case VERBPASTNEG: // if the next token is a VERBPASTNEG
      match(VERBPASTNEG);// Matches verbpastneg
      break;

    // Checks if it matches middle right side.
    case VERB: // if the next token is a VERB
    match(VERB);// Matches verb
    break;

    // Checks if it matches far right side.
    case VERBNEG: // if the next token is a VERBNEG
    match(VERBNEG);// Matches verbneg
    break;

    // Check failed so call syntax error.
    default:
      syntaxerror2("tense");
  }
  gen("TENSE");
}

// Grammar: IS | WAS
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void be(){
  cout << "Parsing <be>" << endl;//to match expected output

  // Get next token.
  switch(next_token()){
    // Checks if it matches left side.
    case IS: // if the next token is a IS
      match(IS);// Matches is
      break;

    // Checks if it matches right side.
    case WAS: // if the next token is a WAS
      match(WAS);// Matches was
      break;

    // Check failed so call syntax error.
    default:
      syntaxerror2("be");
  }
  gen("DESCRIPTION");
  gen("TENSE");
}

//=================================================
// File translator.cpp written by Group Number: 5
//=================================================

// ----- Additions to the parser.cpp ---------------------

// Declare Lexicon (i.e. dictionary) that will hold the content of lexicon.txt
// Make sure it is easy and fast to look up the translation.
// Do not change the format or content of lexicon.txt 
//  Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
map<string, string> dictionary;

// Stream to store output into translated.txt
ofstream translate;
// String to save the e word globally
string saved_e_word;

// Additions to parser.cpp here:
//    getEword() - using the current saved_lexeme, look up the English word
//                 in Lexicon if it is there -- save the result   
//                 in saved_E_word
//  Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void getEword(){
  // Search through dicionary for saved lexeme 
  map<string, string>::iterator found_word = dictionary.find(saved_lexeme);

  // If word was found then save word
  if(found_word != dictionary.end()){
    saved_e_word = found_word -> second;
  }
  // Otherwise save untranslated word
  else{
    saved_e_word = saved_lexeme;
  }
}

//    gen(line_type) - using the line type,
//                     sends a line of an IR to translated.txt
//                     (saved_E_word or saved_token is used)
//  Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
void gen(string type){
  // String to hold the saved word
  string save_word;

  // If type is tense, search for verb type and save
  if(type == "TENSE"){
    save_word = tokenName[saved_token] + '\n';
  }
  // Otherwise save the e word
  else{
    save_word = saved_e_word;
  }

  // Store in translated file
  translate << type << ": " << save_word << endl;
  
}

// ----- Changes to the parser.cpp content ---------------------

//    Comment update: Be sure to put the corresponding grammar 
//    rule with semantic routine calls
//    above each non-terminal function 

  //  Comments updated

//    Each non-terminal function should be calling
//    getEword and/or gen now.

  //  Functions updated

// ---------------- Driver ---------------------------

// The final test driver to start the translator
// Done by: Blake Jaeger, Gregorio Gomez, Abir Hashmi
int main()
{
  // opens the lexicon.txt file and reads it into Lexicon
  ifstream read_lexicon("lexicon.txt", ios::in);
  string current_line;
  while(getline(read_lexicon, current_line)){
    string untranslated_line, translated_line;
    istringstream file_input(current_line);
    file_input >> untranslated_line;
    file_input >> translated_line;
    dictionary[untranslated_line] = translated_line;
  }
  
  // closes lexicon.txt 
  read_lexicon.close();

  // opens the output file translated.txt
  translate.open("translated.txt", ios::out);
  string filename;

  cout << "Enter the input file name: ";
  cin >> filename;
  fin.open(filename.c_str());

  // calls the <story> to start parsing
  story();
  
  // closes the input file 
  fin.close();
  // closes traslated.txt
  translate.close();
 
}// end
// require no other input files!
// syntax error EC requires producing errors.txt of error messages
// tracing On/Off EC requires sending a flag to trace message output functions

