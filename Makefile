########################################
### Makefile for the webserv project ###
########################################

NAME = webserv
# webserv is the name of the executable
CXX = c++
# c++ is the name of the C++ compiler
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
# CXXFLAGS are the flags for the C++ compiler. Wall enables all warnings, Wextra
# enables extra warnings, Werror treats warnings as errors, and std=c++98 sets 
# the C++ standard to C++98.
SRC_DIR = srcs
# srcs is the directory where the source files are located
OBJ_DIR = obj
# obj is the directory where the object files will be stored
INCLUDE_DIR = include
# include is the directory where the header files are located
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# SRCS is a list of all the source files in the srcs directory
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
# OBJS is a list of all the object files corresponding to the source files
DEPS = $(wildcard $(INCLUDE_DIR)/*.hpp)
# DEPS is a list of all the header files in the include directory


all: $(NAME)
# all compile the project
# $(NAME) is the name of the executable 

$(NAME): $(OBJ_DIR) $(OBJS)
# $(NAME) is the target to create the executable with the object files
# $(OBJ_DIR) is the directory where the object files are stored
# $(OBJS) are the object files that will be linked to create the executable
	@echo "Compiling $(NAME)... ⏳"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(NAME) compiled successfully. ✅"

$(OBJ_DIR):
# This target ensures that the object directory exists before compiling
# If the directory does not exist, it will be created
	@echo "Creating object directory $(OBJ_DIR)... 🗂️"
	@mkdir -p $(OBJ_DIR)
	@echo "Object directory created. ✅"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
# This rule compiles each source file into an object file
# It uses the pattern rule to match source files and create corresponding object files
# /% is a wildcard that matches any file name
	@echo "Compiling $< into $@... 📄"
	@$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@
	@echo "$< compiled into $@ successfully. ✅"

clean:
# clean removes the object files and the object directory
	@echo "Cleaning up object files... 🧹"
	@rm -rf $(OBJ_DIR)
	@echo "Object files cleaned up. ✅"

fclean: clean
# fclean removes the executable and cleans up object files
	@echo "Cleaning up executable... 🗑️"
	@rm -f $(NAME)
	@echo "Executable cleaned up. ✅"

re:
# re rebuilds the project from scratch
	@echo "Rebuilding the project... 🔄"
	@$(MAKE) fclean
	@$(MAKE) all
	@echo "Project rebuilt successfully. ✅"

.PHONY: all clean fclean re
# .PHONY declares that these targets are not files, but commands
# This prevents make from getting confused if a file with the same name exists