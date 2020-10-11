#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>


int run_regex_get_child(char *ps_output, char *ppid) {
    char *regexString = (char *) malloc(22 * sizeof(char) + sizeof(ppid));;
    sprintf(regexString, "^.* +([0-9]+) +(%s) +.*$", ppid);
    size_t maxMatches = 1;
    size_t maxGroups = 3;

    regex_t regexCompiled;
    regmatch_t groupArray[maxGroups];
    unsigned int m;
    char *cursor;

    if (regcomp(&regexCompiled, regexString, REG_EXTENDED)) {
        printf("Could not compile regular expression.\n");
        return 1;
    };

    m = 0;
    cursor = ps_output;
    for (m = 0;; m++) {
        if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
            break;  // No more matches

        unsigned int g = 0;
        unsigned int offset = 0;
        for (g = 0; g < maxGroups; g++) {
            if (groupArray[g].rm_so == (size_t) -1)
                break;  // No more groups

            if (g == 0)
                offset = groupArray[g].rm_eo;


            char cursorCopy[strlen(cursor) + 1];
            strcpy(cursorCopy, cursor);
            cursorCopy[groupArray[g].rm_eo] = 0;
            if (g == 1) {
                printf("target pid child pid:\n");

            }
            if (g == 2) {
                printf("target pid child ppid:\n");
            }
            if (g != 0) {
                printf("Match %u, Group %u: [%2u-%2u]: %s\n",
                       m, g, groupArray[g].rm_so, groupArray[g].rm_eo,
                       cursorCopy + groupArray[g].rm_so);
            }
            if (g == 1) {
                printf("recursing on child\n");
                run_regex_get_child(ps_output, cursorCopy + groupArray[g].rm_so);
            }
        }
        cursor += offset;
    }

    regfree(&regexCompiled);

    return 0;
}

int run_regex_get_target(char *ps_output, char *pid) {
    char *regexString = (char *) malloc(22 * sizeof(char) + sizeof(pid));;
    sprintf(regexString, "^.* +(%s) +([0-9]+) +.*$", pid);
    size_t maxGroups = 3;

    regex_t regexCompiled;
    regmatch_t groupArray[maxGroups];
    unsigned int m;
    char *cursor;

    if (regcomp(&regexCompiled, regexString, REG_EXTENDED)) {
        printf("Could not compile regular expression.\n");
        return 1;
    };

    m = 0;
    cursor = ps_output;
    for (m = 0;; m++) {
        if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
            break;  // No more matches

        unsigned int g = 0;
        unsigned int offset = 0;
        for (g = 0; g < maxGroups; g++) {
            if (groupArray[g].rm_so == (size_t) -1)
                break;  // No more groups

            if (g == 0)
                offset = groupArray[g].rm_eo;

            char cursorCopy[strlen(cursor) + 1];
            strcpy(cursorCopy, cursor);
            cursorCopy[groupArray[g].rm_eo] = 0;
            if (g == 1) {
                printf("target pid:\n");

            }
            if (g == 2) {
                printf("target ppid:\n");
            }
            if (g != 0) {
                printf("Match %u, Group %u: [%2u-%2u]: %s\n",
                       m, g, groupArray[g].rm_so, groupArray[g].rm_eo,
                       cursorCopy + groupArray[g].rm_so);
            }

            if (g == 1) {
                printf("recursing on parent\n");
                run_regex_get_child(ps_output, cursorCopy + groupArray[g].rm_so);
            }
        }
        cursor += offset;
    }

    regfree(&regexCompiled);

    return 0;
}


char *save_cmd() {
    char buf[100];
    char *str = NULL;
    char *temp = NULL;
    unsigned int size = 1;  // start with size of 1 to make room for null terminator
    unsigned int strlength;

    FILE *ls;
    if (NULL == (ls = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r"))) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    while (fgets(buf, sizeof(buf), ls) != NULL) {
        strlength = strlen(buf);
        temp = realloc(str, size + strlength);  // allocate room for the buf that gets appended
        if (temp == NULL) {
            // allocation error
        } else {
            str = temp;
        }
        strcpy(str + size - 1, buf);     // append buffer to str
        size += strlength;
    }
    pclose(ls);
    return str;
}

// todo
void parse_args(int argc, char **argv, char **pid, int *interval) {

    if (argc < 2) {
        printf("missing target pid");
    } else {
        *pid = argv[1];
    }

    if (argc == 2) {
        *interval = 3;
    }
    if (argc == 3) {
        *interval = (int) strtol(argv[2], (char **) NULL, 10);
    }
    if (argc > 3) {
        printf("unsupported arguments");
    }
}

int main(int argc, char **argv) {
    int interval = 3;
    char *pid;
    parse_args(argc, argv, &pid, &interval);
    printf("target pid: %s interval: %u", pid, interval);
    char *ps_output = save_cmd();
    run_regex_get_target(ps_output, pid);
    printf("exiting almon");
    exit(0);
}

void terminate_process(char *pid, char *cmd) {
    printf("terminating [%s, %s]\n", pid, cmd);
}
