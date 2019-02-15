##
## 2019
## LightResourceMaker
## File description:
## Makefile
##

NAME = LRM

CXXFLAGS = -std=c++17
CXXFLAGS += -lstdc++fs
CXXFLAGS += -W
CXXFLAGS += -Wall
CXXFLAGS += -Werror
CXXFLAGS += -pedantic
CXXFLAGS += -Wextra
CXXFLAGS += -I./src

SRC = src/ResourceFile.cpp

OBJ = $(SRC:.cpp=.o)

XX = g++

all: $(NAME)

$(NAME): $(OBJ)
	$(XX) $(OBJ) $(CXXFLAGS) -o $(NAME)

clean:
	rm $(OBJ)

fclean: clean
	rm $(NAME)

re: fclean all