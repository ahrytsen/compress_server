NAME_S		=	server
NAME_C		=	client

DIRSRC		=	./src/
DIRSRC_S	=	./src/server/
DIRSRC_C	=	./src/client/
DIROBJ		=	./obj/
DIROBJ_S	=	./obj/server/
DIROBJ_C	=	./obj/client/

INC_LIB		=	-lboost_thread -lboost_system -lboost_regex -lpthread

SRC			=	MessageClass.cpp

SRC_S		=	main.cpp\
				ServerClass.cpp

SRC_C		=	main.cpp\
				ClientClass.cpp

HDRS		=	src/MessageClass.hpp

HDRS_S		=	src/server/ServerClass.hpp

HDRS_C		=	src/client/ClientClass.hpp

OBJ			=	$(addprefix $(DIROBJ), $(SRC:.cpp=.o))

OBJ_S		=	$(addprefix $(DIROBJ_S), $(SRC_S:.cpp=.o))

OBJ_C		=	$(addprefix $(DIROBJ_C), $(SRC_C:.cpp=.o))

CFLAGS		=	-std=c++11

CC			=	g++
RM			=	rm -rf

all		:		$(NAME_S) $(NAME_C)

$(NAME_S):		$(DIROBJ_S) $(OBJ) $(OBJ_S)
				$(CC) -o $(NAME_S) $(OBJ_S) $(OBJ) $(INC_LIB) $(CFLAGS)
				@echo
				@echo "run server: ./run.sh [port]"
				@echo "port number by default '4000'"
				@echo

$(NAME_C):		$(DIROBJ_C) $(OBJ) $(OBJ_C)
				$(CC) -o $(NAME_C) $(OBJ_C) $(OBJ) $(INC_LIB) $(CFLAGS)

clean	:
				$(RM) $(DIROBJ)

fclean	:		clean
				$(RM) $(NAME_S) $(NAME_C)

re		:		fclean all

.PHONY	:		all clean re

$(DIROBJ):
				mkdir -p $(DIROBJ)

$(DIROBJ_S):	$(DIROBJ)
				mkdir -p $(DIROBJ_S)

$(DIROBJ_C):	$(DIROBJ)
				mkdir -p $(DIROBJ_C)

$(OBJ)	:		$(DIROBJ)%.o : $(DIRSRC)%.cpp $(HDRS)
				$(CC) $(CFLAGS) -o $@ -c $<

$(OBJ_S):		$(DIROBJ_S)%.o : $(DIRSRC_S)%.cpp $(HDRS) $(HDRS_S)
				$(CC) $(CFLAGS) -o $@ -c $<

$(OBJ_C):		$(DIROBJ_C)%.o : $(DIRSRC_C)%.cpp $(HDRS) $(HDRS_C)
				$(CC) $(CFLAGS) -o $@ -c $<
