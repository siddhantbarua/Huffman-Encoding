#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>


#define MAX_CHAR 128

//typedef uint8_t BYTE;

struct node{		//structure to store each node
	char data;				
	unsigned freq;
	struct node* left,*right;
};

struct heap{		//heap: a list of nodes
	unsigned size;
	unsigned capacity;
	struct node** list;
};

struct map{		//maps characters to corresponding huffman codes
	char data;
	int * code;
	int code_size;
};

//creates new node
struct node* new_node( char data, unsigned freq ){
	struct node* temp = (struct node*)malloc(sizeof(struct node));
	temp->data = data;
	temp->freq = freq;
	temp->left = temp->right = NULL;

	return temp;
}

//creates heap of nodes
struct heap* create_heap( unsigned capacity ){
	struct heap* heap = (struct heap*)malloc(sizeof(struct heap));
	heap->size = 0 ;
	heap->capacity = capacity;
	heap->list = (struct node**)malloc(heap->capacity*sizeof(struct node*));
	return heap;
}

//fills heap with nodes of characters read from file
struct heap* fill_heap(char data[], unsigned freq[], int size){
	struct heap* heap = create_heap(size);
	for ( int i = 0 ; i < size ; i ++ ){
		heap->list[i] = new_node(data[i], freq[i]);
	}

	heap->size = size;
		
	return heap;
}

//Utility function for swapping
void swap( struct node** a, struct node** b){
	struct node* temp = *a;
	*a = *b;
	*b = temp;
}

//function for sorting heap
void sort ( struct heap* heap ){
	int i, j, min_idx;  
  	int n = heap->size;
    	for ( i = 0; i < n-1; i++){ 
		min_idx = i;  
		for (j = i+1; j < n; j++){  
        		if (heap->list[j]->freq < heap->list[min_idx]->freq){  
           			 min_idx = j;  
			}
		}
        swap(&heap->list[min_idx], &heap->list[i]);  
    	}  
}

//merging leftmost nodes
struct node* merge( struct node *a, struct node*b){
	struct node* temp = new_node('$', a->freq + b->freq );
	temp->left = a;
	temp->right = b;

	return temp;
}

//creates tree from heap
void create_tree ( struct heap* heap ){
	while( heap->size!=1 ){
		sort (heap);
		heap->list[0] = merge ( heap->list[0], heap->list[1] );
		heap->list[1] = heap->list[heap->size-1];
		--heap->size;
	}
}

//Utility function for copying array
void copy( int * code, int *arr, int size){
	for ( int i = 0 ; i < size ; i++){
		code[i] = arr[i];
	}
}

//fills map with huffman codes for the characters
void fill_map(struct map* MAP, struct node* root, int arr[], int ind){
	static int i = 0;
    	if (root->left){
        	arr[ind] = 0;
		fill_map(MAP, root->left, arr, ind + 1);
   	}

     	if (root->right){
		arr[ind] = 1; 
        	fill_map(MAP, root->right, arr, ind + 1);
	}

	else{
         	MAP[i].data = root->data;
		MAP[i].code = (int *)malloc(ind*sizeof(int));
		MAP[i].code_size = ind;
        	copy(MAP[i].code, arr, ind);
		i++;
    	}
}

//main calls this to implement a huffman tree, returns map of characters
struct map* huffman(char data[], unsigned freq[], int size){
	struct map* MAP = (struct map*)malloc(size*sizeof(struct map));
	struct heap* heap = fill_heap( data, freq, size );
	create_tree ( heap );
	int arr[size];
	fill_map(MAP, heap->list[0], arr, 0);
	return MAP;
}	

//utility function for checking existence of a character in a string
int char_in_string( char ch, char data[], int size ){
	for ( int i = 0 ; i < size; i++){
		if ( ch == data[i] )
			return i;
	}
	return -1;
}

//reading from file
void file_read_stuff( char filename[], char data[], int freq[], int* size){
	FILE *fp = fopen(filename, "r");
	char ch; int i = 0;
	while (1){  
		ch = fgetc ( fp );
		if( ch == EOF ){
			break;
		}
		if(char_in_string(ch, data,*size) == -1){
			data[i] = ch;
			freq[i] = 1;
			i++;
			++*size;
		}
		else{
			freq[char_in_string(ch, data, *size)]++;
		}		
	}  
	fclose (fp) ;
}

void print_code( char ch, struct map* MAP, int size){
	for ( int i = 0 ; i < size ; i ++ ){
		if ( ch == MAP[i].data ){
			for ( int j = 0 ; j < MAP[i].code_size ; j++){
				printf("%d", MAP[i].code[j]);
			}
			break;
		}
	}
}

/*void file_write_stuff( char filename[], struct map* MAP, int size){
	FILE *inptr = fopen( filename, "r");
	FILE *outptr = fopen( filename, "w");
	char ch;
}*/


int main(int argc, char** argv){
	if( argc!= 3 ){
		fprintf(stderr, "Usage: ./hufmanfile infile outfile.\n");
		return 1;
	}
	char *infile = argv[1];
	char *outfile = argv[2];
	char arr[MAX_CHAR] = {'\0'};
    	int freq[MAX_CHAR] = {0};
	int size = 0;
	file_read_stuff(infile, arr, freq, &size);
	struct map* MAP = huffman(arr, freq, size);
	for ( int i = 0 ; i < size ; i ++ ){
		printf ("%c ", MAP[i].data);
        	for ( int j = 0 ; j < MAP[i].code_size ; j++ ){
			printf("%d", MAP[i].code[j]);
		}
		printf("\n");
        }
        	
	//file_write_stuff(outfile, MAP, size);
	free(MAP);
	return 0;
}

