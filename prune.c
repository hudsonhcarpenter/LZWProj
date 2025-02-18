#include "encode.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include "decode.h"

//given a code pref, inserts pref and all its prefs to new table, returning its
//new code
int insert_prefs(hash_table *ht, hash_table *ht_new, int old_code){
  table_entry *old_entry = code_search(ht, old_code)->entry;
  if(old_entry -> PREF == -1)
    return old_entry -> CODE;
  else {
    int new_pref = insert_prefs(ht, ht_new, old_entry -> PREF);
    hashed_node *new_node = table_search(ht_new, new_pref, old_entry -> CHAR);
    if(!new_node){
      table_insert(ht_new, new_pref, old_entry -> CHAR);
      
      //return the new code just inserted to new table
      return ht_new->next_code-1;
    }

    //return the new code already in the new table
    return new_node -> entry -> CODE;
  }
}

//returns a new pruned table
hash_table *prune(hash_table *ht){
  //create table
  int n_buckets = ht->n_buckets;
  hash_table *ht_new = (hash_table *)malloc(sizeof(hash_table));
  hashed_node **table = (hashed_node **)malloc(n_buckets*sizeof(hashed_node *));
  *ht_new = (hash_table){table, n_buckets, 0};

  for (int i=0; i<n_buckets; i++){
    ht_new -> table[i] = NULL;
  }

  //insert ascii
  for(int i=0; i<256; i++){
    table_insert(ht_new, -1, (char)i);
  }

  //go through array, recursively insert prefs in to pruned table
  for(int i = 0; i < ht->next_code; i++){
    table_entry *old_entry = code_search(ht, i)->entry;
    if(old_entry && old_entry->PREF != -1)
      insert_prefs(ht, ht_new, old_entry->PREF);
  }

  return ht_new;
}

//insert table entries into array
void array_insert(hash_table *ht, table_entry **arr, int pref, char c){
  table_entry *new_entry = malloc(sizeof(table_entry));
  int code = ht->next_code-1; //UPDATE ARRAY AFTER HASH
  *new_entry = (table_entry){code, pref, c}; 

  arr[code] = new_entry;
}