#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum error_code {
    SUCCESS,
    MALLOC_FAILURE,
    UNDEFINED_ERROR
    };


typedef struct student_s{
    char* name;
    int nlen;
    int ID;
    int average;
    int year;
} student_t;

// forward declarations
student_t* new_student();
int init_student(student_t**, char*, int, int, int);
int set_student_name(student_t*,  char* );

int main(int argc, char* argv[]){
    //student_t* nstudent = new_student(); 
    student_t* nstudent = NULL;
    if ( init_student(&nstudent, "Charlie", 1, 90, 2021) == SUCCESS) {
        printf("%s %d\n",nstudent->name, nstudent->nlen);
    } else {
        printf("Error.\n");
    }
    return EXIT_SUCCESS;
}

student_t* new_student(){
    return (student_t *) malloc(sizeof(student_t));
}

/* init_student - takes a pointer to a student and fills in the data
*/
int init_student(student_t** student, char* name, int ID, int average, int year){
    if ( *student == NULL) (*student) = (student_t *)malloc(sizeof(student_t));
    else if ( *student == NULL) return MALLOC_FAILURE;
    (*student)->nlen = strlen(name);
    if( (*student)->nlen <= 1 ) return UNDEFINED_ERROR;
    (*student)->name = malloc(sizeof(char) * ((*student)->nlen + 1));
    if( (*student)->name != NULL) strcpy((*student)->name, name);
    return SUCCESS;
}
