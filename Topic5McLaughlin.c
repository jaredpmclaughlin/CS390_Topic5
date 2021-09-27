#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// error codes for centralized error handling
enum error_code {
    SUCCESS,
    MALLOC_FAILURE,
    UNDEFINED_ERROR,
    REINITIALIZATION,
    NULL_POINTER,
    ARRAY_INDEX_ERROR,
    PRINTF_ERROR,
    STDIN_ERROR
};
//error related forward declarations
int handle_error(const char *, int);

typedef struct {
    char* name;
    int nlen;
    int ID;
    float average;
    int year;
} student_t;

// student related forward declarations
int student_init(student_t**, char*, int, float, int);

typedef struct node_struct node_t; // forward declaration to sort out index_record_t refferences

typedef struct {
    node_t* nodep;
    int key;
} index_record_t;

// hashing forward declarations
int hashd(int);
int hashd_init(index_record_t*, int);
student_t* lookup(int, index_record_t *, int);
int hashd_print(index_record_t*, int);
int hashd_add(index_record_t*, int, node_t**, int);

typedef struct node_struct {
    student_t* record;
    struct node_struct* prev;
    struct node_struct* next;
}node_t;

int list_init(node_t**,node_t**,student_t**, index_record_t*, int);
int list_add(node_t**, student_t**, index_record_t*, int);
int list_print(node_t**);

// User Interface forward declarations 
int new_record(char *, int, int *, float *, int *,  index_record_t*, int);
void display_menu();
int get_menu_option();
int cleanup(node_t **);
 
int status(int status, int set){
	static int value = SUCCESS;
	
	if( set ) value = status;
	return value;
}
 
int main(){
    int ch;
    node_t* head = NULL;
    node_t* tail = NULL; 
    student_t* nstudent = NULL;
    const int hashes = 100;
    index_record_t hash[hashes];
	int menu_option = 0;

	const int bufsz = 50;
    char buf[bufsz]; // not super happy about this fixed buffer
    int ID;
    float avg;
    int year;

	int err = SUCCESS;

	if ( ( err = hashd_init(hash, hashes)) != SUCCESS){
		handle_error("hashd_init ", err);
		return EXIT_FAILURE;
	}
	
    while(1){
    	
        display_menu();
		if( (err = status(SUCCESS,0)) != SUCCESS){
			handle_error("display_menu", err);
			return EXIT_FAILURE;
		} 

		menu_option = get_menu_option();
		if( (err = status(SUCCESS,0)) != SUCCESS){
			handle_error("display_menu", err);
			return EXIT_FAILURE;
		} 
		
        switch( menu_option ){

            case 1:
                nstudent = NULL;
                err = new_record(buf, bufsz, &ID, &avg, &year, hash, hashes);
				if ( err != SUCCESS) {
                	handle_error("new_record ", err);
                	return EXIT_FAILURE;
				}
				
                err = student_init(&nstudent, buf, ID, avg, year);
				if ( err != SUCCESS) {
                	handle_error("student_init ", err);
                	return EXIT_FAILURE;
				}
                                
                if( head == NULL) {
                    err = list_init(&head, &tail, &nstudent, hash, hashes);
					if ( err != SUCCESS) {
    	            	handle_error("list_init ", err);
        	        	return EXIT_FAILURE;
					}

                } else {
                    err = list_add(&tail, &nstudent, hash, hashes);
					if ( err != SUCCESS) {
    	            	handle_error("list_add ", err);
        	        	return EXIT_FAILURE;
					}
                };
                break;
            
            case 2:
                err = list_print(&head);
                if( err != SUCCESS) {
                	handle_error("list_print ", err);
                	return EXIT_FAILURE;
				}
                break;
    
            case 3:
                err = cleanup(&head);
                if ( err != SUCCESS) {
                	handle_error("cleanup", err);
                	return EXIT_FAILURE;
				}
                return EXIT_SUCCESS;
            
            default:
                printf("That was not an option.\n");
                	while ((ch = getchar()) != '\n' && ch != EOF); // flush
                if ( ferror(stdout) ){				
					handle_error("printf ", PRINTF_ERROR);
					return EXIT_FAILURE;
				}

        }
    }
}

/* hashd takes an ID and returns the hash 
 * Sets global error status for error handling. Never sets anything but SUCCESS. 
 */ 
int hashd(int ID){
	const int bucket = 20;
	status(SUCCESS, 1);
	return ID % bucket;
}

/* hashd_init sets all keys in the hash to 0, so they can be identified as empty slots.
 * returns NULL_POINTER if you send it a null pointer, or SUCCESS otherwose.
 */
int hashd_init(index_record_t* index, int size){
	
	if( index == NULL ) return NULL_POINTER;
	
	for ( int i = 0; i<size; i++){
		index[i].key = 0;
		index[i].nodep = NULL;
	}	
	return SUCCESS;
}

/* hashd_add takes the index, a key, a node, and the hash size and adds a node to the hash based on the key
 * returns NULL_POINTER if any of the pointers passed are null, ARRAY_INDEX_ERROR if we are at the end, or SUCCESS otherwise
 */
int hashd_add(index_record_t* index, int key, node_t** tail, int hash_sz){
	int hashcode = hashd(key);
	
	if( index == NULL ) return NULL_POINTER;
	if( (*tail) == NULL ) return NULL_POINTER;
	
	while( index[hashcode].key != 0 && hashcode < hash_sz ) hashcode++;
		
	if ( hashcode == hash_sz) return ARRAY_INDEX_ERROR;
	
	index[hashcode].key = key;
	index[hashcode].nodep = (*tail);

	return SUCCESS;	
}

/*  lookup takes a key, the hashtable and hashtable size and returns a student record if one matches the key
 *  sets global status for error handling
 */
student_t* lookup(int key, index_record_t *index, int indexsz){
		int hashcode = hashd(key);
		
		if ( hashcode >= indexsz ) {
			status(ARRAY_INDEX_ERROR, 1);
			return NULL;
		}
		
		while(index[hashcode].key != key && hashcode < indexsz){
			if(index[hashcode].key == 0) {
				status(SUCCESS, 1);
				return NULL;
			}
			hashcode++;
		}
		
		if(index[hashcode].nodep == NULL){
			status(NULL_POINTER, 1);
			return NULL;
		}	
		
		status(SUCCESS, 1);
		return index[hashcode].nodep->record;
}

/*
 * hashd_print was developed and tested, but not used in favor of printing from the list
 */
int hashd_print(index_record_t* index, int indexsz){
 
    printf("list : \n");
   
    for ( int i = 0; i < indexsz; i++){
    	if ( index[i].key != 0){
    		printf("Name: %s\t\t", index[i].nodep->record->name);
        	printf("ID: %d\t", index[i].nodep->record->ID);
        	printf("Average: %.1f\t", index[i].nodep->record->average);
        	printf("Year %d\t", index[i].nodep->record->year);
        	printf("\n");
		}
	}
    printf("\n");
    if(ferror(stdout)) return PRINTF_ERROR;
    return SUCCESS;
}

/* cleanup crawls the list pointed to and frees all the malloc'd data 
 * returns NULL_POINTER if you give it a NULL, SUCCESS otherwise
 */
 
int cleanup(node_t **head){

    if ((*head) == NULL) return NULL_POINTER; 
    
    while((*head)->next != NULL){
		
		if( (*head)->record == NULL) return NULL_POINTER;
        free((*head)->record->name);
        free((*head)->record);
        (*head) = (*head)->next;
        free((*head)->prev);
    }
	
	if( (*head)->record == NULL) return NULL_POINTER;
    free((*head)->record->name);
    free((*head)->record);
    free(*head);

    return SUCCESS;
}

/* UI function that displays user options.
 */
void display_menu(){
	
	printf("\n\n\t\tRegis University Student Management System.\n\n");
	if ( ferror(stdout) ){
		status(PRINTF_ERROR,1);
		return;
    } 

    printf("Select\n");
	if ( ferror(stdout) ){
		status(PRINTF_ERROR,1);
		return;
    } 

	printf("\t1. To create a New Student Record.\n");
	if ( ferror(stdout) ){
		status(PRINTF_ERROR,1);
		return;
    } 

    printf("\t2. To view all records.\n");
	if ( ferror(stdout) ){
		status(PRINTF_ERROR,1);
		return;
    } 

    printf("\t3. To Exit\n");
	if ( ferror(stdout) ) status(PRINTF_ERROR,1);
}

/* get_menu_option returns an integer representing the user selection
 * Uses global status reporting for error reporting.
 */
int get_menu_option(){
	int input = 0;
    printf("\n>\t");
    scanf(" %d", &input);
	if ( ferror(stdout) ) status( PRINTF_ERROR, 1); 
	if ( ferror(stdin)) status(STDIN_ERROR,1);  
    return input;
}

/* new_record takes a pointer to a buffer, it's size, pointers to student data, a pointer to a hash table, and it's size and returns an error code or SUCCESS
 * 
 */
int new_record(char* buf, int bufsz, int *ID, float *avg, int* year, index_record_t* index, int index_size){
    int ch = 0; // int is bigger than char, and char is an integer value anyway
    int n = 0;
    int err = SUCCESS;
    char idbuf[50];

    printf("\n");
    if(ferror(stdout)) return PRINTF_ERROR;

    printf("Name : ");
    if(ferror(stdout)) return PRINTF_ERROR;
    if ( bufsz >1 )bufsz--;
	else return ARRAY_INDEX_ERROR; 
	
	while ((ch = getchar()) != '\n' && ch != EOF); // flush
	if(ferror(stdin)) return STDIN_ERROR;
    
	while (( ch = getchar()) != '\n' && n<bufsz ) {
	   if(ferror(stdin)) return STDIN_ERROR;
       buf[n] = (char)ch;
       n++; 
    }
    
	buf[n] = '\0';    

	invalid_ID:
    do{ 
    
    	printf("ID Number : ");
  	    if(ferror(stdout)) return PRINTF_ERROR;
    	n=0;
    	while (( ch = getchar()) != '\n' && n<50 ) {
		    if(ferror(stdin)) return STDIN_ERROR;
            idbuf[n] = (char)ch;
            n++; 
        }
        idbuf[n] = '\0';   
    }while ( ((*ID) = atoi(idbuf)) == 0 );


	//lookup
	if ( lookup((*ID), index, index_size) != NULL ) {
		if ( (err = status(SUCCESS,0)) != SUCCESS) handle_error("lookup", status(SUCCESS,0));
		printf("ID already exists.\n");
    	if(ferror(stdout)) return PRINTF_ERROR;
		goto invalid_ID;
	}
    	
	do {
		printf("Average Letter Grade : ");
		if(ferror(stdout)) return PRINTF_ERROR;
		n = 0;
		while((ch = getchar()) != '\n' && n<50) {
    		if(ferror(stdin)) return STDIN_ERROR;
			idbuf[n] = (char) ch;
			n++;
		}
		idbuf[n]='\0';
		
		switch ( idbuf[0]){
			case 'A':
			case 'a':
				(*avg) = 4;
				break;
			case 'B':
			case 'b':
				(*avg) = 3;
				break;
			case 'C':
			case 'c':
				(*avg) = 2;
				break;
			case 'D':
			case 'd':
				(*avg) = 1;
				break;
			case 'E':
			case 'e':
			case 'F':
			case 'f':
				(*avg ) = 0;
				break;
			default:
				(*avg) = -1;
		}
		if(idbuf[1]){
			if(idbuf[1] == '+') (*avg) = (*avg) + .3;
			if(idbuf[1] == '-') (*avg) = (*avg) - .3; 
		}
	}while ( (*avg) < 0 );

	if( (*avg) < 1 ) (*avg)=0; // there's no D-
    if( (*avg) > 4 ) (*avg)=4; // that's all there is
    
	(*year) = 0;
    // validate the input
    while ( (*year)<1 || (*year)>5){
        printf("Year ( 1 - 5): ");
        if(ferror(stdout)) return PRINTF_ERROR;
        scanf("%d", year);
    	if(ferror(stdin)) return STDIN_ERROR;
    };
        
    return SUCCESS;
}

/* handle_error takes a string constant for the function in which there was an error, and the error code and reports it to stderr
 * it returns an error if you pass it NULL for the string constant and SUCCESS otherwise
 */
int handle_error(const char *function, int err){
    if(function!=NULL) fprintf(stderr, "%s returned ", function);
    else return UNDEFINED_ERROR;
    
    switch(err){
        case MALLOC_FAILURE:
        	fprintf(stderr, "a malloc failure.\n");
        	break;
        case UNDEFINED_ERROR:
        	fprintf(stderr, "and undefined error.\n");
        	break;
        case REINITIALIZATION:
            fprintf(stderr, "an error associated with attemping to reinitialize already initialized data ( potential overwrite).\n");
            break;
        case STDIN_ERROR:
        case PRINTF_ERROR:
        	perror(function);
        	break;
        case SUCCESS:
        	fprintf(stderr, "success - it's unclear why this error message appears here.\n");
        	return SUCCESS;
        case NULL_POINTER:
        	fprintf(stderr, "a null pointer error.\n");
        	break;
        case ARRAY_INDEX_ERROR:
        	fprintf(stderr, "an array index error.\n");
        	break;        
        default:
            fprintf(stderr, "an unrecognized error.\n");
    }
    return SUCCESS;
}

/* list_init initializes the linked list with the first item. It takes pointers to the list, the data, and the hash table and hash table size and returns an error code or SUCCESS;
 *
 */
int list_init(node_t** head, node_t** tail, student_t** first, index_record_t* index, int index_size){
	int err = SUCCESS;

    if ( *head != NULL) return REINITIALIZATION; 
    (*head) = (node_t *)malloc(sizeof(node_t));
    if((*head) == NULL ) return MALLOC_FAILURE;
    (*head)->record = (*first);
    (*head)->prev = NULL;
    (*head)->next = NULL;
    (*tail) = (*head);

	if ( (err = hashd_add(index,(*head)->record->ID, head, index_size)) != SUCCESS) {
		handle_error("hashd_add", err);
		return UNDEFINED_ERROR;
	}

    return SUCCESS;    
}

/* list_add adds data to an already initialized list
 *
 */ 
int list_add(node_t** tail, student_t** new_student, index_record_t* index, int index_size){
	int err = SUCCESS;
	// check if we can index it first ?
	if ( tail == NULL || new_student == NULL || index == NULL) return NULL_POINTER;
    (*tail)->next = (node_t *)malloc(sizeof(node_t));
    if( (*tail)->next == NULL ) return MALLOC_FAILURE;
    (*tail)->next->record = (*new_student);
    (*tail)->next->prev = (*tail);
    (*tail)->next->next = NULL;
    (*tail) = (*tail)->next;
    
    if ( (err = hashd_add(index,(*tail)->record->ID, tail, index_size)) != SUCCESS) {
		handle_error("hashd_add", err);
		return UNDEFINED_ERROR;
	}
    
    return SUCCESS;
}

/* list_print takes a pointer to a list and prints the list to stdout, returning SUCCESS or an error code if something went wrong
 *
 */
int list_print(node_t** head){
    node_t *tmp = (*head);

    printf("\n\nCurrent Students in Database : \n\n");
    
    while( tmp != NULL){
    	// work on formatting for long names
    	if(tmp->record->name == NULL ) return NULL_POINTER;
        printf("Name: %s\n", tmp->record->name);
        if(ferror(stdout)) return PRINTF_ERROR;

        printf("\tID: %d\t", tmp->record->ID);
        if(ferror(stdout)) return PRINTF_ERROR;
        
        printf("Average: %.1f\t", tmp->record->average);
        if(ferror(stdout)) return PRINTF_ERROR;
        
        printf("Year %d\t", tmp->record->year);
        if(ferror(stdout)) return PRINTF_ERROR;
        
        printf("\n");
        if(ferror(stdout)) return PRINTF_ERROR;
        tmp = tmp->next;
    }
    printf("\n");
    if(ferror(stdout)) return PRINTF_ERROR;

    return SUCCESS;
}

/* init_student - takes a pointer to a student and fills in the data
*/
int student_init(student_t** student, char* name, int ID, float average, int year){
    if ( *student == NULL) (*student) = (student_t *)malloc(sizeof(student_t));
    else if ( *student == NULL) return MALLOC_FAILURE;
    (*student)->nlen = strlen(name);
    if( (*student)->nlen <= 1 ) return UNDEFINED_ERROR;
    (*student)->name = (char *) malloc(sizeof(char) * ((*student)->nlen + 1));
    if( (*student)->name != NULL) strcpy((*student)->name, name);
    (*student)->ID = ID;
    (*student)->average = average;
    (*student)->year = year;
    return SUCCESS;
}
