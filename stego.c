// stego h <secret_msg_file>  <image_file>  <new_image_file> - hiding
// stego u <image_file> <recovered_secret_msg> - decoding

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

int main (int argc, char **argv) {
  FILE *msg, *image_changed, *image;
  struct stat st;
  unsigned char pixel_part, pixel_changed_part, character, a, b, rgorb = 0; // r = 0, g = 1, b = 2;
  int i = 0, loaded = 0, msg_size;
  char* msg_size_s[10], msg_size_ss[10];

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

    // najpierw sprawdz wielkosc wiadomosci
    stat(argv[2], &st);
    msg_size = st.st_size; // 118048

    // cipher the size of the message - first how many cyferek and than the number
    sprintf(msg_size_s, "%d", msg_size);
    sprintf(msg_size_ss, "%d", strlen(msg_size_s));
    strcat(msg_size_ss, msg_size_s);
    msg_size = strlen(msg_size_ss);

    // cipher the size of the message into the image
    i = 0;
    while((i < msg_size) && fread(&pixel_part, sizeof(unsigned char), 1, image)) {
      // take one letter from the size string if we are handling a new pixel
      if (rgorb == 0) { character = msg_size_ss[i++]; }
      switch (rgorb) {
        case 0: // red, the first part - change 3 last bits
          a = (0b11100000 & character);
          a = a >> 5;
          b = (0b11111000 & pixel_part);
        break;
        case 1: // green, the second part - change 3 last bits
          a = (0b00011100 & character);
          a = a >> 2;
          b = (0b11111000 & pixel_part);
        break;
        case 2: // blue, the third part - change 2 last bits
          a = (0b00000011 & character);
          b = (0b11111100 & pixel_part);
        break;
      }
      pixel_changed_part = (a | b);
      fwrite(&pixel_changed_part, sizeof(unsigned char), 1, image_changed); // write the pixel's part with changes
      rgorb += 1; rgorb %= 3; // what will be the next pixel's part - r, g or b?
    }

    // copying the rest of the image with changes
    while(fread(&pixel_part, sizeof(unsigned char), 1, image)) {
      // take one letter from the msg if we are handling a new pixel
      if (rgorb == 0) { loaded = fread(&character, sizeof(unsigned char), 1, msg); }
      if (loaded == 0) { break; } // if there is no msg left - dont change the pixels
      // which part of the pixel will we change now?
      switch (rgorb) {
        case 0: // red, the first part - change 3 last bits
          a = (0b11100000 & character);
          a = a >> 5;
          b = (0b11111000 & pixel_part);
        break;
        case 1: // green, the second part - change 3 last bits
          a = (0b00011100 & character);
          a = a >> 2;
          b = (0b11111000 & pixel_part);
        break;
        case 2: // blue, the third part - change 2 last bits
          a = (0b00000011 & character);
          b = (0b11111100 & pixel_part);
        break;
      }
      pixel_changed_part = (a | b);
      fwrite(&pixel_changed_part, sizeof(unsigned char), 1, image_changed); // write the pixel's part with changes
      rgorb += 1; rgorb %= 3; // what will be the next pixel's part - r, g or b?
    } fwrite(&pixel_part, sizeof(unsigned char), 1, image_changed);

    // copying the rest
    while(fread(&pixel_part, sizeof(unsigned char), 1, image)) {
      fwrite(&pixel_part, sizeof(unsigned char), 1, image_changed);
    }

    fclose(msg);
    fclose(image);
    fclose(image_changed);

  // decoding option chosen
  } else if (argv[1][0] == 'u') {
    printf("rescuing the message from the image...\n");
    image = fopen(argv[2], "r");
    msg = fopen(argv[3], "w");

    // skipping the header
    while(fread(&pixel_part, sizeof(unsigned char), 1, image) && (i < 73)) { ++i; }

    // read the size of the message size
    while(fread(&pixel_part, sizeof(unsigned char), 1, image)) {
      switch (rgorb) {
        case 0: // red, the first part - change 3 last bits
          character = 0b00000000;
          a = (0b00000111 & pixel_part);
          a <<= 5;
        break;
        case 1: // green, the second part - change 3 last bits
          a = (0b00000111 & pixel_part);
          a <<= 2;
        break;
        case 2: // blue, the third part - change 2 last bits
          a = (0b00000011 & pixel_part);
        break;
      }
      character |= a;
      rgorb += 1; rgorb %= 3; // what will be the next pixel's part - r, g or b?
      if (rgorb == 0) {
        msg_size = character - '0';
        printf("%d\n", msg_size);
        break;
      }
    }

    // read the message size
    // zbierz nastepne msg_size charow zeby poznac wielkosc wiadomosci

    // read every pixel part

    // read end of the pixel part

    // i sklej z tego chara

    // chara dodaj do wyjsciowego mesega

    fclose(msg);
    fclose(image);
  }
  return 0;
}
