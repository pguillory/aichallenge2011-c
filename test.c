int main(int argc, char *argv[]) {
    FILE *f = fopen("test.txt", "w");
    fprintf(f, "%c[0;31mRed\n", 27);
    fprintf(f, "%c[1;31mRed (bold)\n", 27);
    fprintf(f, "%c[0;31;100mRed\n", 27);
    fprintf(f, "%c[1;31;100mRed (bold)\n", 27);
    fprintf(f, "%c[0;31;40mRed\n", 27);
    fprintf(f, "%c[1;31;40mRed (bold)\n", 27);
    fprintf(f, "%c[0;31;107mRed\n", 27);
    fprintf(f, "%c[1;31;107mRed (bold)\n", 27);

    puts("ok");
    return 0;
}
