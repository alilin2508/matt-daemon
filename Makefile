NAME    = Matt_daemon
RM      = rm      -rf
CC      = c++
FLAGS   = -Wall -Wextra -Werror
DIR_INC = -I ./includes/

SRCS 	:= 	main.cpp \
			Matt_daemon.cpp \
			Tintin_reporter.cpp \

DIR_SRCS 	= ./srcs

DIR_OBJ 	= obj

OBJS        := $(addprefix ${DIR_OBJ}/, ${SRCS:.cpp=.o})

all: $(NAME)

$(DIR_OBJ)/%.o:	$(DIR_SRCS)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) $(DIR_INC) -o $@ -c $<

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(DIR_INC) $(OBJS) -o $(NAME)

clean:
	@$(RM) $(DIR_OBJ)

fclean:		clean
	@$(RM) $(NAME)

re:	fclean all
