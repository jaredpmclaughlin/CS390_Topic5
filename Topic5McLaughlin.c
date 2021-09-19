#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define KEYSZ 3

enum error_code {
    SUCCESS,
    MALLOC_FAILURE,
    UNDEFINED_ERROR,
    REINITIALIZATION
    };
//error related forward declarations
int handle_error(char *, int);

typedef struct {
    char* name;
    int nlen;
    int ID;
    int average;
    int year;
} student_t;
// student related forward declarations
int student_init(student_t**, char*, int, int, int);

typedef struct node_struct {
    student_t *record;
    struct node_struct *prev;
    struct node_struct *next;
}node_t;

int list_init(node_t**,node_t**,student_t**);
int list_add(node_t**, student_t**);
int list_print(node_t**);

typedef struct {
    node_t *nodep;
    char key[KEYSZ];
} index_record_t;

typedef struct {
    index_record_t *records;
    int count;
} index_t;
   
// forward declarations for UI
int new_record(char *, int *, int *, int *);
void display_menu();
int get_menu_option();
 
int main(int argc, char* argv[]){
    //index_t index;
    node_t* head = NULL;
    node_t* tail = NULL; 
    student_t* nstudent = NULL;

    char buf[50]; // not super happy about this fixed buffer
    int ID;
    int avg;
    int year;

    while(1){
        display_menu();
        switch( get_menu_option() ){
            case 1:
                nstudent = NULL;
                new_record(buf, &ID, &avg, &year);
                student_init(&nstudent, buf, ID, avg, year);
                if( head == NULL) {
                    list_init(&head, &tail, &nstudent);
                } else {
                    list_add(&tail, &nstudent);
                };
                break;
            
            case 2:
                list_print(&head);
                break;
    
            case 3:
//                cleanup();
                return EXIT_SUCCESS;
            
            default:
                printf("That was not an option.\n");
        }
    }
}

void display_menu(){
    printf("Press\n");
    printf("\t1. To create a New Student Record.\n");
    printf("\t2. To view all records.\n");
    printf("\t3. To Exit\n");
}

int get_menu_option(){
    int input = 0;
    printf("\n>\t");
    scanf("%d", &input);
    return input;
}

int new_record(char* buf, int *ID, int *avg, int* year){
    printf("Name : ");
    scanf("%s", buf);
    printf("ID : ");
    scanf("%d", ID);
    printf("Average : ");
    scanf("%d", avg);
    printf("Year : ");
    scanf("%d", year);
    return SUCCESS;
}

int handle_error(char *function, int err){
    if(function!=NULL) printf("%s returned ", function);
    else return UNDEFINED_ERROR;
    
    switch(err){
        case MALLOC_FAILURE:
        case UNDEFINED_ERROR:
        case REINITIALIZATION:
            printf("a recognized error.\n");
            break;
        default:
            printf("an unrecognized error.\n");
    }
    
    return SUCCESS;
}

int list_init(node_t** head, node_t** tail, student_t** first){
    //this is a failure because it creates memory leaks
    if ( *head != NULL) return REINITIALIZATION; 
    (*head) = (node_t *)malloc(sizeof(node_t));
    (*head)->record = (*first);
    (*head)->prev = NULL;
    (*head)->next = NULL;
    (*tail) = (*head);
    return SUCCESS;    
}

int list_add(node_t** tail, student_t** new){
    (*tail)->next = (node_t *)malloc(sizeof(node_t));
    (*tail)->next->record = (*new);
    (*tail)->next->prev = (*tail);
    (*tail)->next->next = NULL;
    (*tail) = (*tail)->next;
    return SUCCESS;
}

int list_print(node_t** head){
    node_t *tmp = (*head);
    printf("list : \n");
    while( tmp->next != NULL){
        printf("%s\t", tmp->record->name);
        tmp = tmp->next;
    }
    printf("%s\t", tmp->record->name);
    printf("\n");
    return SUCCESS;
}

/* init_student - takes a pointer to a student and fills in the data
*/
int student_init(student_t** student, char* name, int ID, int average, int year){
    if ( *student == NULL) (*student) = (student_t *)malloc(sizeof(student_t));
    else if ( *student == NULL) return MALLOC_FAILURE;
    (*student)->nlen = strlen(name);
    if( (*student)->nlen <= 1 ) return UNDEFINED_ERROR;
    (*student)->name = malloc(sizeof(char) * ((*student)->nlen + 1));
    if( (*student)->name != NULL) strcpy((*student)->name, name);
    return SUCCESS;
}
