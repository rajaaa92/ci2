// stego h <secret_msg_file>  <image_file>  <new_image_file> - hiding
// stego u <image_file> <recovered_secret_msg> - decoding

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv) {
  FILE *msg, *image_changed, *image;
  unsigned char pixel_part, pixel_changed_part, character, rgorb = 0; // r = 0, g = 1, b = 2;
  int i = 0, loaded = 0;

  // hiding option chosen
  if (argv[1][0] == 'h') {
    printf("hiding message in the image...\n");
    msg = fopen(argv[2], "r");
    image = fopen(argv[3], "r");
    image_changed = fopen(argv[4], "w");

    // copying the header
    while(fread(&pixel_part, sizeof(unsigned char), 1, image) && (i < 73)) {
      fwrite(&pixel_part, sizeof(unsigned char), 1, image_changed);
      ++i;
    } fwrite(&pixel_part, sizeof(unsigned char), 1, image_changed);

    // copying the rest of the image with changes
    while(fread(&pixel_part, sizeof(unsigned char), 1, image)) {
      pixel_changed_part = pixel_part;
      // take one letter from the msg if we are handling a new pixel
      if (rgorb == 0) { loaded = fread(&character, sizeof(unsigned char), 1, msg); }
      if (loaded == 0) { break; } // if there is no msg left - dont change the pixels
      // which part of the pixel will we change now?
      switch (rgorb) {
        case 0: // red, the first part - change 3 last bits
          // bierz trzy pierwsze bity charactera i wklej je do trzech ostatnich bitow pixel_changed_parta
        break;
        case 1: // green, the second part - change 3 last bits
          // bierz 4-6 bity charactera i wklej je do trzech ostatnich bitow pixel_changed_parta
        break;
        case 2: // blue, the third part - change 2 last bits
          // bierz 2 ostatnie bity charactera i wklej je do dwoch ostatnich bitow pixel_changed_parta
        break;
      }
      printf(".");
      fwrite(&pixel_changed_part, sizeof(unsigned char), 1, image_changed); // write the pixel's part with changes
      rgorb += 1; rgorb %= 3; // what will be the next pixel's part - r, g or b?
    } fwrite(&pixel_changed_part, sizeof(unsigned char), 1, image_changed);

    // copying the header
    while(fread(&pixel_part, sizeof(unsigned char), 1, image)) {
      fwrite(&pixel_part, sizeof(unsigned char), 1, image_changed);
    }

    fclose(msg);
    fclose(image);
    fclose(image_changed);

  // decoding option chosen
  } else if (argv[1][0] == 'u') {
    printf("u\n");
  }

  // unsigned char position;
  // int i;
  // dct* dict = new_dict();
  // wrd* P, *P_C, *C, *empty;
  // C = new_word(); empty = new_word();
  // while(fread(&position, sizeof(unsigned char), 1, input)) {
  //   alloc_word(C, 1); P = NULL; P_C = new_word();
  //   int c = fread(C->body, sizeof(char), 1, input);
  //   if (position) { P = &dict->words_array[position]; }
  //   else P = empty;
  //   sum_words(P_C, P, C);
  //   if (c) fwrite(P_C->body, sizeof(char), P_C->max_size, output);
  //   else fwrite(P->body, sizeof(char), P->max_size, output);
  //   add_word_to_dict(dict, P_C);
  //   free(C->body); free_word(C);
  //   free(P_C->body); free_word(P_C);
  // }
  // for(i = 0; i <= dict->size; ++i) free(dict->words_array[i].body);
  // free(dict->words_array);
  // free(dict);
  return 0;
}
