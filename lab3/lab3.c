#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>


#define NUM_PAGES 256
#define NUM_FRAMES 256
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define NUM_TLB 128
#define ADDRESSES "addresses.txt"
#define REFERENCE "correct.txt"

FILE *file_ptr;
int read_max = 65536;
int array_of_addresses[1000];
int array_of_untampered_addresses[1000];
int array_of_physical_addresses[1000];
int array_of_references[1000];
char *array_of_chars[1000];
char array_of_formatted_addresses[100*1000];
int num_addresses = 0;
int num_page_faults = 0;
int num_tlb_hits = 0;
int num_tlb_miss = 0;

char *virtual_address = "Virtual address: ";
char *physical_address = "Physical address: ";
char *value = "Value: ";

struct page_table;
struct free_frame;
struct free_frame *init_frame_list(int size);
struct page_table *init_page_table();
struct TLB;
struct TLB *init_TLB();
void allocate(int v_addr,struct page_table *ptable, struct free_frame *free, struct TLB *tlb);
void store_virtual_as_physical(int v_addr, int p_addr);
void read_addresses_to_array(char *filename);        
void format_for_comparison();
void print_statistics();

//code
int main(int argc, char const *argv[])
{
        struct free_frame *free = init_frame_list(NUM_FRAMES);
        struct page_table *page_table = init_page_table();
        struct TLB *tlb = init_TLB();

        read_addresses_to_array(ADDRESSES);
        for(int i = 0; i < 1000; i++){
                allocate(array_of_addresses[i], page_table, free, tlb);
        }        
        format_for_comparison();
        print_statistics();
        return 0;
}

void print_statistics(){
        printf("The statistics are as such:\nAmount of addresses:\t%d\nAmount of Page Faults:\t%d\nAmount of TLB hits:\t%d\nAmount of TLB misses:\t%d\n", num_addresses, num_page_faults, num_tlb_hits, num_tlb_miss);
}


struct TLB{
        int page[NUM_TLB];
        int frame[NUM_TLB];
        int valid[NUM_TLB];
        int index;
};

struct TLB *init_TLB(){
        struct TLB *tlb = malloc(sizeof(struct TLB));
        tlb->index = 0;
        for (int i = 0; i< NUM_TLB; i++){
                tlb->valid[i] = 0;
        }
}

int TLB_lookup(struct TLB *tlb, int v_addr){
        for(int i = 0; i < NUM_TLB; i ++){
                int page = v_addr / PAGE_SIZE;
                if(tlb->page[i] == page && tlb->valid[i] == 1){
                        num_tlb_hits++;
                        return tlb->frame[i];

                }
        }
        num_tlb_miss++;
        return -1;
}


void tlb_insert(int page_num, int frame_num, struct TLB *tlb){
        tlb->page[tlb->index] = page_num;
        tlb->frame[tlb->index] = frame_num;
        tlb->valid[tlb->index] = 1;
        tlb->index = (tlb->index +1) % NUM_TLB; 
}

struct page_table{
        int page[NUM_PAGES];
        int valid[NUM_PAGES];
        // def
};
struct page_table *init_page_table(){
        struct page_table *ptab = malloc(sizeof(struct page_table));
        for(int i; i < NUM_PAGES; i++){
                ptab->valid[i] = 0;
        }
        return ptab;
}


struct free_frame{
        int f_num;
        struct free_frame *next;
};

struct free_frame *init_frame_list(int size){
        struct free_frame *head;
        struct free_frame *p;
        head = malloc(sizeof(struct free_frame));
        p = head;
        for(int i; i < size; i++){
                p->f_num = i;
                p->next = malloc(sizeof(struct free_frame));
                p=p->next;
        }
        return(head);
}

int get_free_frame(struct free_frame *free_f){
        int ret = free_f->f_num;
        struct free_frame *next = free_f->next;
        free_f->f_num = free_f->next->f_num;
        free_f->next = free_f->next->next;
        free(next);
        return ret;
}

void allocate(
                int v_addr,
                struct page_table *ptable,
                struct free_frame *free,
                struct TLB *tlb
                ){
        int offset = v_addr % PAGE_SIZE;
        int p_num = v_addr / 256;
        int p_addr;
        int f_num = TLB_lookup(tlb, v_addr);
        if(f_num != -1){
                //TLB hit
                NULL;
        }else{
                //TLB miss
                if(ptable->valid[p_num]){
                        //Page hit
                        f_num = ptable->page[p_num];
                }else{
                        //Page Fault
                        num_page_faults++;
                        ptable->valid[p_num] = 1;
                        ptable->page[p_num] = get_free_frame(free);
                        f_num = ptable->page[p_num];
                }
                tlb_insert(p_num, f_num, tlb);
        }
        p_addr = ptable->page[p_num]*256+offset;

        //used for correctness checking against "correct.txt"
        store_virtual_as_physical(v_addr, p_addr);
}

void store_virtual_as_physical(int v_addr, int p_addr){
	for(int i = 0; i < sizeof(array_of_addresses)/sizeof(int); i++){
		if(v_addr == array_of_addresses[i]){
			array_of_physical_addresses[i] = p_addr;
		}
	}
}

void read_addresses_to_array(char *filename){
    file_ptr = fopen(filename, "r");
    // checks that file is opened properly
    if(file_ptr == NULL){
        printf("Unable to open file");
        exit(1);
    }    
    // init some variables used while reading file
    int i = 0;
    char temp_read_buffer[1000];
    int string_as_int;
    // while file is not at end of file keep reading line into buffer, convert
    // to int and then store the 16 LSb in an array of ints.
    // use fgets (for line by line) or fgetc (for char by char)
    while(fgets(temp_read_buffer, read_max, file_ptr)){
        num_addresses++;
        string_as_int = atoi(temp_read_buffer);
        array_of_untampered_addresses[i] = string_as_int;
        string_as_int = string_as_int & 0xFFFF;
        array_of_addresses[i] = string_as_int;
        i++;
    }
    fclose(file_ptr);
}

void format_for_comparison(){
        int j = 0;
        for (int i = 0; i < sizeof(array_of_formatted_addresses); i = i + 100) {
                char temp_virtual[100];
                char temp_physical[100];
                sprintf(temp_virtual, "%d ", array_of_untampered_addresses[j]);
                sprintf(temp_physical, "%d", array_of_physical_addresses[j]);
                strcpy(&array_of_formatted_addresses[i], virtual_address);
                strcat(&array_of_formatted_addresses[i], temp_virtual);
                strcat(&array_of_formatted_addresses[i], physical_address);
                strcat(&array_of_formatted_addresses[i], temp_physical);
                strcat(&array_of_formatted_addresses[i], "\n");
                j++;
        }
        file_ptr = fopen(REFERENCE, "r");
        if (file_ptr == NULL) {
                printf("unable to open %s", REFERENCE);
                exit(1);
        }
        char temp_read_buffer[1000];
        j = 0;
        int cmp;
        int counter = 0;
        while (fgets(temp_read_buffer, read_max, file_ptr)) {
                //char temp[sizeof(array_of_formatted_addresses[j*100])];
                //sprintf(temp, "%s", array_of_formatted_addresses[j*100]);
                //printf("%s\n",&array_of_formatted_addresses[j*100]);
                cmp = strcmp(&array_of_formatted_addresses[j*100], temp_read_buffer);
                if(cmp != 0){
                        counter++;
                        printf("the strings\n%sand\n%sdo not match\ncmp=%d\n", &array_of_formatted_addresses[j*100], temp_read_buffer,cmp);
                }
                j++;
        }
        if(counter > 0){
                printf("There are %d incorrect virtual and physical memory mappings", counter);
        } else {
                printf("The input matches the reference output file.");
        }
}
