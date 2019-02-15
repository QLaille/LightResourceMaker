#include "ResourceFile.hpp"

namespace LRM {

void log(const std::string &s)
{
	static bool x = false;

	if (x)
		std::cerr << s << std::endl;
	if (s == strings.rfm_toggle_logs)
		x = true;
}

/*
**** getFirstNode
**
** get the root node of an xml file
**
** the parameter is the path of an xml file to be opened
*/

rapidxml::xml_node<> *getFirstNode(const std::string &input)
{
	rapidxml::xml_document<> doc;
	std::vector<char> buffer;
	std::ifstream f(input);

	if (!f.is_open())
		throw(strings.log_error_fileopen);
	buffer.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	doc.parse<0>(&buffer[0]);
	return (doc.first_node(strings.xml_element_root.c_str()));
}

/*
**** getStruct
**
** get the content of the struct element
**
** the parameter is a node "struct"
*/

resourceStruct getStruct(rapidxml::xml_node<> *pNode)
{
	resourceStruct s;
	std::pair<std::string, std::string> p;

	s._name = pNode->first_attribute(strings.xml_attribute_id.c_str())->value();
	for (rapidxml::xml_node<> *elementNode = pNode->first_node(); elementNode; elementNode=elementNode->next_sibling()) {
		p = std::make_pair(elementNode->name(), elementNode->value());
		s._elements.insert({elementNode->first_attribute(strings.xml_attribute_id.c_str())->value(), p});
	}
	return (s);
}

/*
**** getAllStruct
**
** for each element <struct>, generate an object containing all values to be added in a struct.
**
** the parameter is the root element of the xml file.
*/

std::vector<resourceStruct> getAllStruct(rapidxml::xml_node<> *pRoot)
{
	std::vector<resourceStruct> _structs;
	rapidxml::xml_node<> *pNode;

	for (pNode = pRoot->first_node(strings.xml_element_struct.c_str());
	pNode && pNode->name() == strings.xml_element_struct;
	pNode = pNode->next_sibling())
		_structs.push_back(getStruct(pNode));
	return (_structs);
}

/*
**** generateFooter
**
** generate footer of a header file
**
** parameters are the filestream of the output file and its name.
*/

void generateFooter(std::ofstream &outfile, std::string &outfileName)
{
	std::transform(outfileName.begin(), outfileName.end(), outfileName.begin(), ::toupper);
	outfile << strings.header_endif << "/* !" << strings.header_name_prefix << outfileName << strings.header_name_suffix << " */" << std::endl;
	std::transform(outfileName.begin(), outfileName.end(), outfileName.begin(), ::tolower);
}

/*
**** generateIncludeLine
**
** generate an include for a required type of a struct (string for instance)
**
** the parameter is the type of a member of a struct
*/

std::string generateIncludeLine(const std::string &type)
{
	switch (hash(type.c_str())) {
		case hash("string"):
			return ("#include <string>");
		default:
			return ("");
	};
}

/*
**** generateHeader
**
** generate the header of a header file
**
** parameters are the filestream of the output file, its name, and the list of structs to build inside that file.
*/

void generateHeader(std::ofstream &outfile, std::string &outfileName, const std::vector<resourceStruct> &_structs)
{
	//TODO: add a signature ?
	std::transform(outfileName.begin(), outfileName.end(), outfileName.begin(), ::toupper);
	outfile << strings.header_ifndef << strings.header_name_prefix << outfileName << strings.header_name_suffix << std::endl;
	outfile << strings.header_define << strings.header_name_prefix << outfileName << strings.header_name_suffix << std::endl << std::endl;
	std::transform(outfileName.begin(), outfileName.end(), outfileName.begin(), ::tolower);
	for (auto _struct : _structs) {
		for (auto elem : _struct._elements)
			outfile << generateIncludeLine(elem.second.first) << std::endl;;
	}
}

/*
**** generateVariableLine
**
** generate the line of a variable of a struct.
**
** parameters are the type, name and value of said variable.
*/

std::string generateVariableLine(const std::string &type, const std::string &name, const std::string &value)
{
	return ("\tstatic inline const " + type + " " + name + " = " + value + ";");
}

/*
**** generateVariable
**
** generate the variable of a struct
**
** parameters are the filestream of the output file, and a pair containing the name, type and value of a variable.
*/

void generateVariable(std::ofstream &outfile, std::pair<std::string, std::pair<std::string, std::string>> p)
{
	std::string id = p.first;
	std::string type = p.second.first;
	std::string value = p.second.second;

	switch (hash(type.c_str())) {
		case hash("string"):
			type = strings.type_std_string;
			value = "\"" + value + "\"";
			break;
		default:
			break;
	};
	outfile << generateVariableLine(type, id, value) << std::endl;
}

/*
**** generateStruct
**
** generate a struct inside a header file
**
** parameters are the filestream of the output file, its name, and the list of structs to build inside that file.
*/

void generateStruct(std::ofstream &outfile, const resourceStruct &_struct)
{
		outfile << "static struct " << _struct._name  << "_s {" << std::endl;
		for (auto elem : _struct._elements)
			generateVariable(outfile, elem);
		outfile << "} " << _struct._name << ";\n" << std::endl;
}

/*
**** generateFileContent
**
** generate the content of a header file: a header, a footer and the structs.
**
** parameters are the filestream of the output file, its name, and a vector of all structs inside that file.
*/

void generateFileContent(std::ofstream &outfile,std::string &outfileName, std::vector<resourceStruct> &_structs)
{
	generateHeader(outfile, outfileName, _structs);
	for (auto _struct : _structs)
		generateStruct(outfile, _struct);
	generateFooter(outfile, outfileName);
}

/*
**** generateFile
**
** generate a header file
**
** parameters are the filestream of the output file, its name, and the list of structs to build inside that file.
*/

void generateFile(std::string input, const std::string &output)
{
	rapidxml::xml_node<> *pRoot;
	std::vector<resourceStruct> _structs;
	std::ofstream outfile;
	std::string outfileName;

	try {
		pRoot = getFirstNode(input);
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	_structs = getAllStruct(pRoot);
	outfileName = pRoot->first_attribute(strings.xml_attribute_id.c_str())->value();
	outfile.open(output + outfileName + strings.hpp_file_extension);
	if (!outfile.is_open())
		throw(strings.log_generation_file_failure + output + outfileName + strings.hpp_file_extension);
	generateFileContent(outfile, outfileName, _structs);
	log(outfileName + strings.log_generation_success);
}

/*
**** generateFolder
**
** generate a folder containing all resource files as .hpp.
**
** parameters are the folder of input and folder of output
*/

void generateFolder(std::string &input, const std::string &output)
{
	//TODO: generate global resource file if there's more than two xml files
	for (auto file : std::filesystem::directory_iterator(input)) {
		if (file.path().extension() == strings.xml_file_extension)
			generateFile(file.path(), output);
		else
			log(file.path().filename().string() + strings.log_not_a_xml_file);
	}
}

}

void help()
{
	std::cout << USAGE << std::endl;
}

int main(int ac, char **av)
{
	int c = 0;
	int idx = 0;
	int ret = 0;
	std::string input;
	std::string output;

	if (ac == 1)
		exit(1);
	while ((c = getopt_long(ac, av, "i:o:vh", longopts, &idx)) != -1) {
		switch (c) {
			case 'i':
				input = optarg;
				if (!std::filesystem::exists(input))
					exit(1);
				break;
			case 'o':
				output = optarg;
				if (!std::filesystem::exists(output))
					exit(1);
				break;
			case 'v':
				LRM::log(strings.rfm_toggle_logs);
				break;
			case 'h':
			default:
				help();
				exit(1);
		}
	}
	try {
		if (input.empty())
			throw(strings.log_error_input);
		if (output.empty())
			throw(strings.log_error_output);
		LRM::generateFolder(input, output);
	} catch (const std::string &e){
		std::cerr << e << std::endl;
		ret = 1;
	}
	return (ret);
}