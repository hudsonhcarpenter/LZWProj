#include "decode.h"
#include "encode.h"
#include "prune.h"
#include "bit_pack.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

hashed_node *code_search(hash_table *ht, int c){
  for (int i=0; i<ht->n_buckets; i++){
    hashed_node *node = ht->table[i];
    while(node){
      if (node->entry->CODE == c)
        return node;
      node = node->next;
    }
  }
  return NULL;
}

//inserts new chars right where str points to in a linked list, returns finalK
char concat_char(hash_table *ht, hashed_node *node, linked_char **str){
  if(!node){
    fprintf(stderr, "Node is null");
    exit(1);
  }
  table_entry *entry = node->entry;
  
  linked_char *new_lc = malloc(sizeof(linked_char));
  *new_lc = (linked_char){entry->CHAR, NULL};
  
  if(!(*str)->next){
    (*str)->next = new_lc;
  } else {
    new_lc->next = (*str)->next;
    (*str)->next = new_lc;
  }
  
  if(entry->PREF != -1){
    return concat_char(ht,code_search(ht, entry->PREF),str);
  } else {
    return entry->CHAR;
  }
}

void decode() {
  char *stage = getenv("STAGE");

  //get prune and max_bits
  int p = 0;
  int max_b = 9;
  int num_codes = 0;

  if(stage && !strcmp(stage, "2")){
    scanf("%d", &p);
    scanf("%d", &max_b);
  } else if(!stage || !strcmp(stage, "3")){
    p = getchar();
    max_b = getchar();
    
    //scanf("%d", &num_codes);
    fread(&num_codes, 4, 1, stdin);
  }

  //create table 
  int n_buckets = 1 << max_b;
  hash_table *ht = (hash_table *)malloc(sizeof(hash_table));
  hashed_node **table = (hashed_node **)malloc(n_buckets*sizeof(hashed_node *));
  *ht = (hash_table){table, n_buckets, 0};
 
  for (int i=0; i<n_buckets; i++){
   ht -> table[i] = NULL;
  }

  //insert every char into table
  for(int i=0; i<256; i++){
    table_insert(ht, -1, (char)i);
  }

  //start decoding and appending
  linked_char *output_start = malloc(sizeof(linked_char));
  *output_start = (linked_char){'\0', NULL};
  linked_char *output_end = output_start;
  
  int oldC = -1;
  char finalK = 0;
  hashed_node *node = NULL;
  int newC;

  if(stage && strcmp(stage, "3")){
    while (scanf("%d", &newC) != EOF) {
      //if pruning
      if(ht->next_code>=n_buckets && p){
        ht = prune(ht);
        oldC = -1;
      }

      //not pruning
      node = code_search(ht, newC);
      if(node){
        finalK = concat_char(ht, node, &output_end);

        if(oldC != -1 && ht->next_code<n_buckets){
          table_insert(ht, oldC, finalK);
        } 
      } else {
        if(ht->next_code<n_buckets)
          table_insert(ht, oldC, finalK);
      
        node = code_search(ht, newC);
        finalK = concat_char(ht, node, &output_end);
      }
      
      while(output_end->next)
          output_end = output_end->next;
      
      oldC = newC;
    }
  } else {
    // --------------------- STAGE 3 ---------------------
    //get packed data
    int num_bytes = (num_codes * max_b + 7) / 8;
    char *packed_data = malloc(num_bytes);

    fread(packed_data, 1, num_bytes, stdin);

    //initialize packed data bit and char pointers
    int b_pos = 0; int c_pos = 0;

    //initialize n_bits
    int n_bits = 9;

    for(int i=0; i<num_codes; i++){
      //update n_bits
      if((ht->next_code+1)>>n_bits && (n_bits < max_b))
        n_bits++;

      //if pruning
      if(ht->next_code>=n_buckets && p){
        fprintf(stderr,"\n\nPRUNING NOW CODE JUST READ %d\n\n", oldC);
        ht = prune(ht);
        oldC = -1;

        //update n_bits
        n_bits = 9;
        while((ht->next_code-1)>>n_bits && (n_bits < max_b))
          n_bits++;

        fprintf(stderr,"\n\nN_BITS %d\n\n", n_bits);
      }

      //get new code
      newC = unpack_bits(n_bits, &c_pos, &b_pos, packed_data);

      //not pruning
      node = code_search(ht, newC);
      if(node){
        finalK = concat_char(ht, node, &output_end);

        if(oldC != -1 && ht->next_code<n_buckets){
          table_insert(ht, oldC, finalK);
          if((ht->next_code)>>n_bits && (n_bits < max_b))
            n_bits++;
        }
      } else {
        if(ht->next_code<n_buckets){
          table_insert(ht, oldC, finalK);
          if((ht->next_code)>>n_bits && (n_bits < max_b))
            n_bits++;
        }
          
        node = code_search(ht, newC);

        finalK = concat_char(ht, node, &output_end);
      }
      
      while(output_end->next)
          output_end = output_end->next;
      
      oldC = newC;
    }
  }

  //print output
  while(output_start->next){
    printf("%c",output_start->next->c);
    output_start = output_start->next;
  }

  //DBG dump
  char *dbg = getenv("DBG");
  if(dbg)
    print_hash(ht, "DBG.decode");
}