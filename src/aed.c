/*
 The way this program works is simple, its just a linked list with
 all lines stored in a seperate node, which makes it easy to insert
 and remove lines.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define PRINT_SIZE 255
#undef BUFSIZ /*How disastrous could this possibly be*/
#define BUFSIZ 1024

/**/
struct text_line{
    struct text_line* next;
    struct text_line* prev;
    char text[BUFSIZ];
};

/**/
bool print_linenum = false;
bool auto_print = false;

long line_num = 1l;
FILE* f_in = NULL;
struct text_line* head = NULL;
char clipboard[BUFSIZ] = {'\n', 0};

/**/
int edit(void);
void init_file(void);

int insert_line(struct text_line* left, struct text_line* right, struct text_line* insert);
int delete_line(struct text_line** cur_line, char commands[16]);
void change_line(struct text_line** cur_line, long amount);
void swap_line(struct text_line* cur_line, char commands[16]);
void print_line(struct text_line* cur_line, char commands[16]);
void jump_line(struct text_line** cur_line, char commands[16]);

void switch_bool(bool* b);
long read_number(unsigned start, char commands[16]);

int main(int argc, char **argv)
{
    unsigned i;
    char file_name[ FILENAME_MAX ] = {0};
    void* free_this;
    
    if(argc > 1){
        strncpy(file_name, argv[1], FILENAME_MAX);
    }
    else{
        puts("Specify filename:");
        fgets(file_name, FILENAME_MAX, stdin);
    }
    
    for(i = 0; i < FILENAME_MAX && file_name[i] != '\n'; i++){ }
    file_name[i] = '\0';
    
    f_in = fopen(file_name, "r");
    if( !f_in ){ 
        f_in = fopen(file_name, "w+");
        if( !f_in ){ 
            return -1;
        }
    }
    
    init_file();
    fclose(f_in);
    
    edit();
    
    f_in = fopen(file_name, "w");
    if( !f_in ){ return -1; }
    
    rewind(f_in);
    while(head != NULL){
        fputs(head->text, f_in);
        
        free_this = head;
        head = head->next;
        free(free_this);
    }
    
    fclose(f_in);
    return 0;
}

void print_line(struct text_line* cur_line, char commands[16]){
    long number = 0, local_linenum = line_num;
    bool p_ln_local = print_linenum;
    bool surround_print = false;
    int i;
    
    for(i = 1; i < 16 && commands[i] != '\0'; i++){
        if(commands[i] == 'n'){
            p_ln_local = true;
        }
        if(commands[i] == 's'){
            surround_print = true;
        }
        
        if(commands[i] >= '0' && commands[i] <= '9'){
            number *= 10;
            number += commands[i] - '0';
        }
    }
    
    if(number == 0){number = 1;}
    
    if(surround_print == true){
        for(i = 0; i < number && cur_line->prev != NULL; i++){
            cur_line = cur_line->prev;
            local_linenum--;
        }
        number *= 2;
    }
    
    for(i = 0; i < number && cur_line != NULL; i++){
        
        if(p_ln_local == true){
            printf("%3ld", local_linenum + i);
        }
        if(local_linenum+i == line_num){
            putchar('>');
        }else{
            putchar('|');
        }
    
        fputs(cur_line->text, stdout);
        cur_line = cur_line->next;
    }
    putchar('\n');    
}

void switch_bool(bool* b){
    if( *b == true ){
        *b = false;
    }else{
        *b = true;
    }
}

void swap_line(struct text_line* cur_line, char commands[16]){
    int i = 0;
    bool auto_print_temp = auto_print;
    long line_tmp = line_num;
    long line1 = 0, line2 = 0;
    char buff1[BUFSIZ] = {0};
    char buff2[BUFSIZ] = {0};
    
    for(i = 1; i < 16 && commands[i] >= '0' && commands[i] <= '9'; i++){
        line1 *= 10;
        line1 += commands[i] - '0';
    }
    
    if(commands[i] == '&'){
        i++;
        for(; i < 16 && commands[i] >= '0' && commands[i] <= '9'; i++){
            line2 *= 10;
            line2 += commands[i] - '0';
        }
    }
    
    if( line1 == 0 ){ line1 = line_num; }
    if( line1 == line_num && line2 == 0 ){ line2 = line_num+1; }
    if( line2 == 0 ){ line2 = line_num; }
    
    printf("Swapping lines %ld and %ld\n", line1, line2);
    
    auto_print = false;
    
    change_line(&cur_line, (line1 - line_num));
    strncpy(buff1, cur_line->text, BUFSIZ);
    
    change_line(&cur_line, (line2 - line_num));
    strncpy(buff2, cur_line->text, BUFSIZ);
    strncpy(cur_line->text, buff1, BUFSIZ);
    
    change_line(&cur_line, (line1 - line_num));
    strncpy(cur_line->text, buff2, BUFSIZ);
    
    auto_print = auto_print_temp;
    line_num = line_tmp;
}

void change_line(struct text_line** cur_line, long amount){
    
    while( amount != 0 )
    {
        if( amount > 0 ){
            if((*cur_line)->next){
                *cur_line = (*cur_line)->next;
                amount--;
                line_num++;
            }else{
                puts("Reached end of the file");
                amount = 0;
            }
        }
        else{
            if((*cur_line)->prev){
                *cur_line = (*cur_line)->prev;
                amount++;
                line_num--;
            }else{
                puts("Reached start of the file");
                amount = 0;
            }
        }
        
    }
    if( auto_print ){
        if( print_linenum ){
            printf("%ld$ ", line_num);
        }
        fputs((*cur_line)->text, stdout);
    }
}


int edit(void)
{
    char commands[16] = {0};
    struct text_line* buff = NULL;
    struct text_line* cur_line = head;
    
    do{
        fgets(commands, 16, stdin);
        
        switch( commands[0] )
        {
            case 'q':/*quit*/
                break;
            default:
                puts("?");
                break;
            
            case 'n':
                if(commands[1] == 'n'){
                    switch_bool(&print_linenum); 
                }else{
                    printf("%ld\n", line_num);
                }
                break;
                
            case 'p':/*print*/
                if(commands[1] == 'p'){
                    switch_bool(&auto_print);
                }else{
                    buff = cur_line;
                    print_line(cur_line, commands);
                    cur_line = buff;
                }
                break;
            case 'j':
                jump_line(&cur_line, commands);
                break;
                
            case 'w':/*(over)write*/
                fgets(cur_line->text, BUFSIZ, stdin);
                break;
            case 'a':/*append*/
                buff = calloc(1, sizeof(struct text_line));
                if(!buff){ exit(errno); }
                
                fgets(buff->text, BUFSIZ, stdin);
                insert_line(cur_line, cur_line->next, buff);
                change_line(&cur_line, 1);
                break;
            case 'd':
                delete_line(&cur_line, commands);
                break;
                
            case 'c':/*copy to clipboard*/
                strncpy(clipboard, cur_line->text, BUFSIZ);
                puts(clipboard);
                break;
            case 'v':/*paste clipboard*/
                if(commands[1] != 'w'){
                    buff = calloc(1, sizeof(struct text_line));
                    if(!buff){ exit(errno); }
                
                    strncpy(buff->text, clipboard, BUFSIZ);
                    insert_line(cur_line, cur_line->next, buff);
                    change_line(&cur_line, 1);
                }else{
                    strncpy(cur_line->text, clipboard, BUFSIZ);                    
                }
                break;
            case 's':/*swap*/
                swap_line(cur_line, commands);
                break;
                
            case 'l':/*next line*/
                change_line(&cur_line, 1);
                break;
            case 'k':/*previous line*/
                change_line(&cur_line, -1);
                break;
        }
        
    } while( commands[0] != 'q');
    
    return 0;
}

struct text_line* new_line(struct text_line* in)
{
    struct text_line* tmp = calloc( 1, sizeof(struct text_line) );
    if( !tmp ){ exit(errno); }
    
    tmp->prev = in;
    in->next = tmp;
    fgets( tmp->text, BUFSIZ, f_in );
    return tmp;
}

void jump_line(struct text_line** cur_line, char commands[16]){
    long line = 0;
    
    if( commands[1] != '+' && commands[1] != '-' ){
        line = read_number(1, commands);
        
        change_line(cur_line, (line - line_num) );
    }else{
        line = read_number(2, commands);
        
        if(commands[1] == '-'){
            change_line(cur_line, -line);
        }else{
            change_line(cur_line, line);            
        }
    }
    if(auto_print == true){
        print_line(*cur_line, "p");
    }
}

int delete_line(struct text_line** cur_line, char commands[16])
{
    struct text_line* buff = NULL;
    struct text_line* left = NULL;
    struct text_line* right = NULL;
    long number = 0;
    
    number = read_number(1, commands);
    
    if( number != 0 ){
        change_line(cur_line, (number - line_num) );
    }
    
    if( (*cur_line)->next ){ buff = (*cur_line)->next; }else
    if( (*cur_line)->prev ){ buff = (*cur_line)->prev; }else{
        buff = calloc( 1, sizeof(struct text_line) );
        if( !buff ){ exit(errno); }
    }
    
    left =  (*cur_line)->prev;
    right = (*cur_line)->next;
    
    free(*cur_line);
    
    if( left && !right ){ 
        left->next = NULL;
    }
    else if( right && !left ){ 
        right->prev = NULL;
    }
    else if( left && right ){
        left->next = right;
        right->prev = left;
    }
    
    *cur_line = buff;
    return 0;
}

/*Literally used once, thought I'd use it more, may get rid of it entirely*/
long read_number(unsigned start, char commands[16]){
    unsigned i = 0;
    long number = 0;
    for(i = start; i < 16 && commands[i] >= '0' && commands[i] <= '9'; i++){
        number *= 10;
        number += commands[i] - '0';
    }
    return number;
}

int insert_line(struct text_line* left, struct text_line* right, struct text_line* insert)
{
    if( insert == NULL ){ return -1; } 
    
    if( left ){
        left->next = insert;
        insert->prev = left;
    }
    if( right ){
        insert->next = right;
        right->prev = insert;
    }
    
    return 0;
}


void init_file(void)
{
    long f_size = 0, f_pos = 0;
    struct text_line* tmp = calloc(1, sizeof(struct text_line));
    if( !tmp ){ exit(errno); }
    
    head = tmp;
    
    fseek(f_in, 0l, SEEK_END);
    f_size = ftell(f_in);
    rewind(f_in);
    f_pos = 0l;
    
    
    fgets( tmp->text, BUFSIZ, f_in );
    if(tmp->text[0] == '\0'){/*hoping this makes lines actually have a newline*/
        tmp->text[0] = '\n';
        tmp->text[1] = '\0';
    }
    tmp->next = new_line(tmp);
    tmp = tmp->next;
    
    while(f_pos < f_size){
        tmp->next = new_line(tmp);
        tmp = tmp->next;
        f_pos = ftell(f_in);
    }
}
