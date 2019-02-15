# LightResourceMaker

A simple and light project to convert hard-coded values from xml files to cpp structs.

## Build and start using LightResourceMaker

Simply build the binary using `make`, and move it somewhere where it can be called everywhere (refer to your PATH env).

Gather all the predefined values of your project into xml files,
and use LRM to build structs based on them.

## Dependencies

This project relies on [rapidxml-1.13](http://rapidxml.sourceforge.net/)
and g++ with the standard C++17.

## Usage

`RFMaker -i in -o out [options]`

Read XML files to produce header files in CPP.

-i, --input         folder containing XML files
-o, --output        folder to which output the resource files based on the input option

options:
    -v, --verbose       toggle the logs
    -h, --help          display this help and exit

Exit status:
0   if OK,
1   if problem occurred

## Purpose of this project

While working with Android Studio for a school project, I liked the way the resources are managed:
they are all gathered in a folder in xml structures, and we refer to said values based on the xml anywhere in the code.
I just wanted to reproduce such behavior for cpp, it may not be the optimal way of working with hard coded values, but I like it this way.

## Example of XML file

Each element must have the attribute "id", the value being its name.
The root element needs to be called "resource",
inside this resource element, you can have as many "struct" element as you want.
Each struct element contains a list of "$type" element, where $type can be int, float, std::string, etc.
In the body of each "$type" element, can be found the value itself.

With the following xml:
```xml
<resource id="fileName">
	<struct id="structName">
		<int id="answerToLife">42</int>
		<string id="foo">bar</string>
	</struct>
</resource>
```

Will be produced inside a "fileName.hpp" a struct named "structName" containing an int named "answerToLife" and a string named "foo".
