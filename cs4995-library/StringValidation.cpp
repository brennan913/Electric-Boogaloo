#include "StringProcessing.hpp"
#include <exception>
#include <iostream>
#include <unordered_set>
#include <string>
#include "StringValidation.hpp"

/*const std::unordered_set<char> VALID_CHARS{
	' ','-','.','^','/','_','(',')','A','B','C','D','E','F','G','0',
	'1','2','3','4','5','6','7','8','9','#','b'};
*/
void validate_str_input(string str){

	
	// first char must be A-G, 0-9
	if (  !( (str[0] >= 'A' && str[0] <= 'G') || (str[0] >= '0' && str[0] <= '9') )  ){
		throw std::invalid_argument(
		"invalid first character, input must start with digit or a note A-G");
	}

	bool inside_parens = false;
	unsigned int len = str.length();
	
	for(unsigned int i=0; i<len; ++i){
		

		// input is constrained to specific characters
		if (VALID_CHARS.count(str[i]) == 0){
			throw std::invalid_argument("input contains illegal characters");
		}
		// disallow nested parentheses
		if (inside_parens && str[i] == '('){
			throw std::invalid_argument("input contains nested parentheses");
		}
		// update parentheses status
		if (str[i] == '('){
			inside_parens = true;
		}
		// if we start with ), parentheses are unbalanced
		if (!inside_parens && str[i] == ')'){
			throw std::invalid_argument("input contains unbalanced parentheses");
		}
		// '(' must be after a digit character and before a space
		if (str[i] == '(' && ( 
		    !(str[i-1] >= '0' && str[i-1] <= '9') 
		    || i == len-1 
		    || str[i+1] != ' ') ){
			throw std::invalid_argument(
			    "open parenthesis must be after a digit and before a space");
		}
		// ) must be between spaces or the last character in the string
		if (str[i] == ')' && 
		    (str[i-1] != ' ' 
		    || (i != len-1 && str[i+1] != ' ')) ){
			throw std::invalid_argument("close parenthesis must be between spaces, or"
			"after space and before end of input");
		}
		// each '/' must be followed by one of A-G
		if (str[i] == '/' && (i == len-1 || !(str[i+1] >= 'A' && str[i+1] <= 'G')) ){
			throw std::invalid_argument("'/' must be followed by one of A-G");	
		}
		// . and - must be followed by space
		if ( (str[i] == '.' || str[i] == '-') && i != len-1 && str[i+1] != ' '){
			throw std::invalid_argument("tokens must be delimited by spaces");
		}
		// update parentheses status
		if (inside_parens && str[i] == ')'){
			inside_parens = false;
		}
		// digits must be followed by space or parentheses
		if(str[i] >= '0' && str[i] <= '9' && 
		    !( i == len-1 || str[i+1] == ' ' || str[i+1] == '(' ) ){
			throw std::invalid_argument(
			"digits must be followed by space or parentheses");
		}
	}
}

static void test(){
	validate_str_input("4( C E G F ) 1( E ) 8( E/G/B E/G/B - - E D C C ) 1( . ) 3( A )");		

	validate_str_input("4( C E G H ) 1( E ) 8( E/G/B E/G/B - - E D C C ) 1( . ) 3( A )");		
	
	validate_str_input("4( C E G F ) 1(E ) 8( E/G/B E/G/B - - E D C C ) 1( . ) 3( A )");		
}

