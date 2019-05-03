#ifndef STRING_VALIDATION_HPP
#define STRING_VALIDATION_HPP

#include "StringProcessing.hpp"
#include <exception>
#include <iostream>
#include <unordered_set>
#include <string>

const std::unordered_set<char> VALID_CHARS{
	' ','-','.','^','/','_','(',')','A','B','C','D','E','F','G','0',
	'1','2','3','4','5','6','7','8','9','#','b'};

void validate_str_input(string str);
	
#endif
