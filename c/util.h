// gets the size of a file (will return the cursor to the start of the file)
size_t get_file_size(FILE* file) {
    size_t size = 0;

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}