#include "encode.h"
#include "prune.h"
#include "bit_pack.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>

void print_hash(hash_table *ht, char *file_name){
  hashed_node **table = ht->table;

  FILE *file = fopen(file_name, "w");

  for (int i = 0; i < ht->n_buckets; i++){
    hashed_node *node = table[i];
    if(node){
      fprintf(file,"\n[%d]\t", i);
      while(node){
        table_entry *entry = node->entry;
        if(isprint(entry->CHAR))
          fprintf(file,"(CODE: %d, PREF: %d, CHAR: %c)", entry->CODE, entry->PREF, entry->CHAR);
        else
          fprintf(file,"(CODE: %d, PREF: %d, CHAR: [%u])", entry->CODE, entry->PREF, entry->CHAR);
        if(node->next){
          fprintf(file," ---> ");
        }
        node = node->next;
      }
    }
  }

  fclose(file);
}

void table_insert(hash_table *ht, int pref, char c){
  //hash function: (((unsigned long)(PREF) << CHAR BIT) | (unsigned)(CHAR)) % SIZE
  unsigned long hash_code = (((unsigned long)(pref) << CHAR_BIT) | (unsigned)(c));
  int bucket = hash_code % ht->n_buckets;
  hashed_node *node = ht->table[bucket];

  hashed_node *new_node = malloc(sizeof(hashed_node));
  table_entry *new_entry = malloc(sizeof(table_entry));
  *new_entry = (table_entry){ht->next_code++, pref, c};
  *new_node = (hashed_node){new_entry, hash_code, NULL};

  if(!ht->table[bucket])
    ht->table[bucket] = new_node;
  else{
    new_node->next = node;
    ht->table[bucket] = new_node;
  }
}

hashed_node *table_search(hash_table *ht, int pref, char c){
  unsigned long hash_code = (((unsigned long)(pref) << CHAR_BIT) | (unsigned)(c));
  int bucket = hash_code % ht->n_buckets;
  hashed_node *node = ht->table[bucket];

  

  while(node){
    table_entry *entry = node->entry;
    if (entry->PREF == pref && entry->CHAR == c)
      return node;
    node = node->next;
  }

  return NULL;
}

void encode(int p, int max_bit_length) {
  //create table
  int n_buckets = 1 << max_bit_length;
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

  //print prune and max_bits
  char *stage = getenv("STAGE");

  if(stage && !strcmp(stage, "2")){
    printf("%d\n", p);
    printf("%d\n", max_bit_length);
  } else if(!stage || !strcmp(stage,"3")){
    printf("%c", (char)p);
    printf("%c", (char)max_bit_length);
  }

  //if packing
  li_code *codes = NULL;
  int n_bits = 9; // for each char
  int num_codes = 0;

  //LZW encode
  char K;
  int C = -1;
  while (fread(&K, 1, 1, stdin) == 1){
    hashed_node *node = table_search(ht, C, K);
    if(node){
      C = node->entry->CODE;
    } else {
      //insert new table entries
      if(ht->next_code<n_buckets){
        table_insert(ht,C,K);

        //update n_bits
        if((ht->next_code-1)>>n_bits && n_bits < max_bit_length)
          n_bits++;

      } else if(p){
        ht = prune(ht);

        //update n_bits
        n_bits = 9;
        while((ht->next_code)>>n_bits && (n_bits < max_bit_length))
          n_bits++;
      }
      //output code
      if(!stage || !strcmp(stage,"3")){
        codes = add_code(C, n_bits, codes); 
        num_codes++;
        printf("OUTPUT CODE %d NBITS %d\n", C, n_bits);
      } else 
        printf("%d\n", C);
      
      node = table_search(ht,-1,K);
      C = node->entry->CODE;
    }
  }
  
  //final output
  if (C!=-1){
    //output code
    if(!stage || !strcmp(stage,"3")){
      add_code(C, n_bits, codes); 
      num_codes++;
    } 
    else
      printf("%d\n", C);
  }

  //if packing, output number of codes and packed codes
  if(!stage || !strcmp(stage,"3")){
    fwrite(&num_codes, 4, 1, stdout);
    //pack_bits(codes, max_bit_length, num_codes);
  }

  //DBG Dump
  char *dbg = getenv("DBG");
  if(dbg){
    print_hash(ht, "DBG.encode");
  }
}

