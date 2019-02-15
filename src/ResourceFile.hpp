#ifndef RESOURCE_FILE_MAKER_HPP
# define RESOURCE_FILE_MAKER_HPP

#include "../rapidxml-1.13/rapidxml.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

#include <getopt.h>
#include <unistd.h>
#include <inttypes.h>

#define no_argument 0
#define required_argument 1
#define optional_argument 2

const struct option longopts[] = {
	{"help", no_argument, 0, 'h'},
	{"verbose", no_argument, 0, 'v'},
	{"input", required_argument, 0, 'i'},
	{"output", required_argument, 0, 'o'},
	{0,0,0,0},
};

static struct strings_s {
	static inline const std::string rfm_name = "RFMaker";
	static inline const std::string xml_attribute_id = "id";
	static inline const std::string header_endif = "#endif ";
	static inline const std::string header_define = "#define ";
	static inline const std::string header_ifndef = "#ifndef ";
	static inline const std::string hpp_file_extension = ".hpp";
	static inline const std::string header_name_suffix = "_HPP";
	static inline const std::string xml_file_extension = ".xml";
	static inline const std::string xml_element_struct = "struct";
	static inline const std::string xml_element_root = "resource";
	static inline const std::string type_std_string = "std::string";
	static inline const std::string log_error_input = "NO INPUT FILE";
	static inline const std::string log_error_output = "NO OUTPUT FILE";
	static inline const std::string header_name_prefix = "RESOURCE_FILE_";
	static inline const std::string log_error_fileopen = "ERROR WHILE OPENING FILE";
	static inline const std::string log_generation_success = "SUCCESSFULLY GENERATED";
	static inline const std::string rfm_toggle_logs = "Toggle The God Damn Verbose Kyle";
	static inline const std::string log_not_a_xml_file = " IS NOT AN XML FILE, IGNORING...";
	static inline const std::string log_generation_file_failure = "CANNOT CREATE FILE NAMED ";
} strings;

/*
**** Used to make switch cases with strings
**
** Credits to Steven Pigeon: https://hbfs.wordpress.com/2017/01/10/strings-in-c-switchcase-statements/
*/
uint64_t constexpr mix(char m, uint64_t s)
 {
  return ((s<<7) + ~(s>>3)) + ~m;
 }

uint64_t constexpr hash(const char * m)
 {
  return (*m) ? mix(*m,hash(m+1)) : 0;
 }

namespace LRM {
/*
**** RESOURCE STRUCT
**
** THIS STRUCT IS THE RECAP OF A STRUCT NODE
** _elements: an unordered map of variables referenced by their id, with a pair (type, value) for value.
** _name: the name of the struct;
*/
struct resourceStruct {
	std::unordered_map<std::string, std::pair<std::string, std::string>> _elements;
	std::string _name;
};

void log(const std::string &s);
resourceStruct getStruct(rapidxml::xml_node<>*);
void generateFooter(std::ofstream&, std::string&);
void generateFile(std::string, const std::string&);
std::string generateIncludeLine(const std::string&);
void generateFolder(std::string&, const std::string&);
rapidxml::xml_node<> *getFirstNode(const std::string&);
void generateStruct(std::ofstream&, const resourceStruct&);
std::vector<resourceStruct> getAllStruct(rapidxml::xml_node<>*);
void generateFileContent(std::ofstream&,std::string&, std::vector<resourceStruct>&);
void generateHeader(std::ofstream&, std::string&, const std::vector<resourceStruct>&);
std::string generateVariableLine(const std::string&, const std::string&, const std::string&);
void generateVariable(std::ofstream&, std::pair<std::string, std::pair<std::string, std::string>>);
}

#define USAGE "Usage: RFMaker -i in -o out [options]\n\
Read XML files to produce header files in CPP.\n\
\n\
-i,	--input			folder containing XML files\n\
-o,	--output		folder to which output the resource files based on the input option\n\
\n\
options:\n\
	-v,	--verbose	toggle the logs\n\
	-h,	--help		display this help and exit\n\
\n\
Exit status:\n\
0	if OK,\n\
1	if problems"

#endif /* !RESOURCE_FILE_MAKER_HPP */