SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
NAME=configator

all:		$(NAME)

$(NAME):	$(OBJ)
	$(CXX) -std=c++98 -pedantic -Wall -Wextra -Werror -o $@ $^

%.o:		%.cpp
	$(CXX) -std=c++98 -pedantic -Wall -Wextra -Werror -o $@ -c $<

.PHONY:		clean re

clean:
	$(RM) $(OBJ) $(NAME)

re:			clean all