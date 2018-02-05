#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

#include "jsonformatter.h"

char *delimiter = "    ";

int main(int argc, char **argv) {

    // parse the commandline options
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"delimiter", required_argument, 0, 'd'}
    };
    int c;
    int option_index = 0;
    while ((c = getopt_long(argc, argv, "d:h", long_options, &option_index)) != -1) {
        switch(c) {
            case 'd':
                delimiter = optarg;
                break;
            case 'h':
                show_help(argv[0]);
                exit(EXIT_SUCCESS);
            default:
                abort();
        }
    } 

    delimiter = replace_special_char(delimiter);

    indent_json();
    return 0;
}

char *replace_special_char(char *data) {
    ssize_t len = strlen(data);
    for (int i = 0; i < len; i++) {
        if (data[i] == '\\') {
            if (data[i + 1] == 't') {
              data[i] = '\t';
              memcpy(data + i + 1, data + i + 2, len - i - 2);
              memset(data + len - 1, '\0', 1);
              len--;
            }
        }
    } 
    return data;
}

bool control_char(char ch) {
    return (ch < 33 || ch == 127); 
}

void show_help(char* progr) {
    printf("\nUsage: %s [options]\n\n", progr);
    printf("This program takes input from stdin and tries to apply json pretty formatting\n\n");
    printf("Options:\n");
    printf("\t-h/--help\t\tShow this usage dialog and exit.\n");
    printf("\t-d/--delimiter <value>\tSet the indent delimiter to a given string. The default is four spaces.\n\n"); 
}

char* set_indent(char *tabs, ssize_t count) {
    int indent_len = strlen(delimiter) * count;
    int delimiter_len = strlen(delimiter);
    tabs = realloc(tabs, sizeof(char) * indent_len + sizeof(char));
    for (int i = 0; i < count; i++) {
      memcpy(tabs + (i * delimiter_len), delimiter, delimiter_len);
    }
    memset(tabs + indent_len, '\0', 1);
    return tabs;
}

void indent_json() {

    printf("Delimiter is [%s]\n", delimiter);

    ssize_t num_read;
    char buf[1];
    char ch;
    int num_curly = 0;
    char *tabs = NULL;
    bool double_quote = false;

    while ((num_read = read(0, buf, 1)) > 0) {
        ch = buf[0];

        if (ch == '{' || ch == '[') {

            // an open bracket ha been encountered, so the number of tab characters changes
            tabs = set_indent(tabs, ++num_curly);
            printf("%c\n%s", ch, tabs);

        } else if (ch == '}' || ch == ']') {

            // a close bracket has been encoutnered so the number of tab characters change
            tabs = set_indent(tabs, --num_curly);
            printf("\n%s%c", tabs, ch);

        } else {

            // Only output printable chars
            if (!control_char(ch)) {
                printf("%c", ch);
            }

            // make sure to know when were in side of double quotes
            double_quote = (ch == '\"' && !double_quote);

            // if it's a comma between fields, a line break is inserted
            if (ch == ',' && ! double_quote) {
                printf("\n%s", tabs);
            }
        }
    }

    printf("\n");
    free(tabs);
}
