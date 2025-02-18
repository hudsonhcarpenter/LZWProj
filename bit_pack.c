#include "bit_pack.h"
#include <stdio.h>
#include <stdlib.h>

//creates a new linked code list with the new code appended
li_code *add_code(int code, int n_bits, li_code *code_list){
  li_code *new_li_code = (li_code *)malloc(sizeof(li_code));
  *new_li_code = (li_code){code, n_bits, NULL};

  if(code_list){
    li_code *end = code_list;
    while(end->next)
      end = end->next;
    end->next = new_li_code;
  } else {
    code_list = new_li_code;
  }

  return code_list;
}

//pack codes into bits and print
void pack_bits(li_code *codes, int max_bits, int num_codes){
  char *large_pack = malloc((num_codes * max_bits + 7)/8);
  for(int i=0; i<(num_codes * max_bits + 7)/8; i++)
    large_pack[i] = 0;

  int total_bits = 0;
  int char_pos = 0;
  int bit_pos = 0;

  while(codes){
    //how many bits will be packed
    int num = codes -> code;
    int bits_to_pack = codes -> n_bits;
    
    total_bits += bits_to_pack;

    //pack "bits_to_pack" number of bits
    while(bits_to_pack > 0){
      int free_bits = 8 - bit_pos;
      int n_curr_pack = bits_to_pack <= free_bits ? bits_to_pack : free_bits;

      //(first free bits in num) << (amount needed to pack)
      char chunk = (num>>(bits_to_pack-n_curr_pack))<<(free_bits-n_curr_pack);
      large_pack[char_pos] |=  chunk;
      //slice off bits just packed
      num &= (1 << (bits_to_pack-n_curr_pack)) - 1;
      
      bit_pos += n_curr_pack;
      bits_to_pack -= n_curr_pack;
      if(bit_pos == 8){
        bit_pos = 0;
        char_pos++;
      }
    }
    codes = codes->next;
  }
  //print relevant bytes
  for(int i=0; i<(total_bits + 7)/8; i++)
      printf("%c", large_pack[i]);

  free(large_pack);
}

int unpack_bits(int n_bits, int *char_pos, int *bit_pos, char *packed_data){
  if (n_bits <= 0 || n_bits > 32) {
        return -1; // Error for invalid bit size
    }

    int result = 0;
    int bits_read = 0;

    while (bits_read < n_bits) {
        // Fetch the current byte
        int current_byte = packed_data[*char_pos];

        // Determine bits available to read in the current byte
        int available_bits = 8 - *bit_pos;

        // Calculate how many bits to read in this step
        int bits_to_read = (n_bits - bits_read <= available_bits) ? (n_bits - bits_read) : available_bits;

        // Extract the bits
        int mask = (1 << bits_to_read) - 1;
        int extracted_bits = (current_byte >> (available_bits - bits_to_read)) & mask;

        // Add extracted bits to the result
        result <<= bits_to_read;
        result |= extracted_bits;

        // Update counters
        bits_read += bits_to_read;
        *bit_pos += bits_to_read;

        // Move to the next byte if needed
        if (*bit_pos >= 8) {
            *bit_pos = 0;
            (*char_pos)++;
        }
    }

    return result;
}

// int main(){
//   //li_code *codes = NULL;
//   // codes = add_code(66, 9, codes);
//   // codes = add_code(65, 9, codes);
//   // codes = add_code(256, 9, codes);
//   // codes = add_code(257, 9, codes);
//   // codes = add_code(65, 9, codes);
//   // codes = add_code(260, 9, codes);
//   // codes = add_code(10, 9, codes);

//   // pack_bits(codes, 10, 7);

//   int b_pos = 1; int c_pos = 1;
//   char *packed = "\x21\x10\x60\x10\x12\x0c\x10\x14";
//   for(int i=0; i<7; i++){
//     int code = unpack_bits(9, &c_pos, &b_pos, packed);
//     printf("%d\n", code);
//   }
  
//   // printf("%d\n", unpack_bits(9, &c_pos, &b_pos, packed));
//   // printf("%d, %d", b_pos, c_pos);

//   return 0;
// }
