CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g3
NAME = webserv.out
MAKEFLAGS = --no-print-directory

SRC =	srcs/main.cpp\
			srcs/ServerCreate.cpp\
			srcs/ClientHandling.cpp\
			srcs/RequestParser.cpp\
			srcs/GetResponse.cpp\
			srcs/PostResponse.cpp\
			srcs/ResponseBuilder.cpp\
			srcs/RequestProcessing.cpp\
			srcs/DeleteResponse.cpp\
			srcs/ParseHeader.cpp\
			srcs/TrieNode.cpp

OBJ = $(SRC:.cpp=.o)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(NAME)

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re all
