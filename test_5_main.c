int main(int argc, char *argv[]) {
  if (argc != 2)
  {
    printf("Input debe coindicir con: ./crfs <simdisk.bin>\n");
    return 0;
  }
  char* diskname = argv[1];

  cr_mount(diskname);

  if (!ruta_archivo) {
    printf("Disk not mounted");
    return 0;
  }

  create_dir_blocks();
  create_cr_bitmaps();

  // TEST 5
  cr_ls(1);
  printf("\n\n\n");

  crFILE* arch = cr_open(1, "guides.txt","r");
  char* buffer = calloc(1001, sizeof(char));
  int num = cr_read(arch, buffer, 1000);
  printf("num bytes leidos = %d\n", num);
  printf("Extracto 1 = %s\n", buffer);
  printf("\n\n\n");
  cr_close(arch);
  free(buffer);

  cr_hardlink(1, "guides.txt", "guides_copy.txt");
  
  cr_ls(1);
  printf("\n\n\n");

  crFILE* arch2 = cr_open(1, "guides.txt","r");
  char* buffer2 = calloc(1001, sizeof(char));
  int num2 = cr_read(arch2, buffer2, 1000);
  printf("num bytes leidos = %d\n", num2);
  printf("Extracto 1 = %s\n", buffer2);
  printf("\n\n\n");
  cr_close(arch2);
  free(buffer2);

  destroy_directories();
  destroy_bitmaps();

  return 0;
}