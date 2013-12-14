// stego h <secret_msg_file>  <image_file>  <new_image_file> - hiding
// stego u <image_file> <recovered_secret_msg> - decoding

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv) {
  FILE *msg, *image_changed, *image;
  unsigned char pixel_part, pixel_changed_part, character, a, b, interval = 0, rgorb = 0; // r = 0, g = 1, b = 2;
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
      // take one letter from the msg if we are handling a new pixel
      if ((rgorb == 0) && (interval % 9 == 0)) { loaded = fread(&character, sizeof(unsigned char), 1, msg); }
      if (loaded == 0) { break; } // if there is no msg left - dont change the pixels
      // which part of the pixel will we change now?
      if (interval < 3) {
        switch (rgorb) {
          case 0: // red, the first part - change 3 last bits
            a = (0b11100000 & character);
            a >>= 5;
            b = (0b11111000 & pixel_part);
          break;
          case 1: // green, the second part - change 3 last bits
            a = (0b00011100 & character);
            a >>= 2;
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
        ++interval;
      } else {
        fwrite(&pixel_part, sizeof(unsigned char), 1, image_changed);
        ++interval;
      }
      interval %= 9;
    } fwrite(&pixel_part, sizeof(unsigned char), 1, image_changed);

    // copying the header
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

    fclose(msg);
    fclose(image);
  }
  return 0;
}
