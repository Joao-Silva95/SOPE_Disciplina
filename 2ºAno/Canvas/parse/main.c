#include "header.h"
#include <readline/readline.h>
#include <readline/history.h>

extern CMD *command_list;

int main(void) {
    char *line;
    while ((line = readline("parse> ")) != NULL) {
        if (*line) add_history(line);
        parse_line(line);
        print_command_list();
        free_command_list();
        command_list = NULL;
        free(line);
    }
    return 0;
}
