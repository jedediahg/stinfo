#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#define MAX_JSON_SIZE (2 * 1024 * 1024) // 2MB

void print_help(const char* program_name) {
    printf("Usage: %s [options] <filename>\n", program_name);
    printf("Options:\n");
    printf("  -v         Verbose output, prints the JSON header size.\n");
    printf("  -p         Prettify the JSON output with indentation.\n");
    printf("  -m         Print only the __metadata__ object from the JSON.\n");
    printf("  --?        Show this help message and exit.\n");
}

void print_json(const char* json, int prettify) {
    if (!prettify) {
        printf("%s\n", json);
        return;
    }

    int indentTab = 0;
    int inQuote = 0; // Flag to check if inside a quote

    for (int i = 0; json[i] != '\0'; i++) {
        if (json[i] == '"') {
            inQuote = !inQuote; // Toggle inQuote flag
            putchar(json[i]);
        }
        else if (json[i] == '{' && !inQuote) {
            printf("{\n");
            indentTab++;
            for (int j = 0; j < indentTab; j++) printf("   ");
        }
        else if (json[i] == '}' && !inQuote) {
            indentTab--;
            printf("\n");
            for (int j = 0; j < indentTab; j++) printf("   ");
            printf("}");
        }
        else if (json[i] == ',' && !inQuote) {
            printf(",\n");
            for (int j = 0; j < indentTab; j++) printf("   ");
        }
        else {
            putchar(json[i]);
        }
    }
    printf("\n");
}

void extract_metadata(const char* json, char* metadata) {
    const char* metadata_start = strstr(json, "\"__metadata__\"");
    if (!metadata_start) {
        printf("No __metadata__ object found.\n");
        return;
    }

    int braces = 0;
    int i = 0, j = 0;

    // Adjust the starting point to the beginning of the metadata object
    while (metadata_start[i] != '{' && metadata_start[i] != '\0') {
        i++;
    }

    if (metadata_start[i] == '{') {
        braces++;
        metadata[j++] = '{';
        i++;
    }

    while (metadata_start[i] != '\0' && braces > 0) {
        metadata[j++] = metadata_start[i];
        if (metadata_start[i] == '{') {
            braces++;
        }
        else if (metadata_start[i] == '}') {
            braces--;
        }
        i++;
    }
    metadata[j] = '\0';
}

int main(int argc, char* argv[]) {
    int verbose = 0, prettify = 0, metadata = 0;
    char* filename = NULL;

    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        }
        else if (strcmp(argv[i], "-p") == 0) {
            prettify = 1;
        }
        else if (strcmp(argv[i], "-m") == 0) {
            metadata = 1;
        }
        else if (strcmp(argv[i], "--?") == 0) {
            print_help(argv[0]);
            return 0;
        }
        else {
            filename = argv[i];
        }
    }

    if (filename == NULL) {
        print_help(argv[0]);
        return 1;
    }

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Read the first 8 bytes to get the length of the JSON header
    uint64_t json_length;
    if (fread(&json_length, sizeof(uint64_t), 1, file) != 1) {
        perror("Error reading JSON length");
        fclose(file);
        return 1;
    }

    if (verbose) {
        printf("JSON header size is %" PRIu64 "\n", json_length);
    }

    // Ensure json_length does not exceed buffer size
    if (json_length >= MAX_JSON_SIZE) {
        printf("JSON header too large.\n");
        fclose(file);
        return 1;
    }

    // Read the JSON header
    char* json_header = (char*)malloc(json_length + 1);
    if (json_header == NULL) {
        perror("Memory allocation error");
        fclose(file);
        return 1;
    }

    if (fread(json_header, 1, json_length, file) != json_length) {
        perror("Error reading JSON header");
        free(json_header);
        fclose(file);
        return 1;
    }
    json_header[json_length] = '\0';

    fclose(file);

    // Print the JSON header or metadata
    if (metadata) {
        char* metadata_json = (char*)malloc(MAX_JSON_SIZE);
        if (metadata_json == NULL) {
            perror("Memory allocation error");
            free(json_header);
            return 1;
        }
        extract_metadata(json_header, metadata_json);
        print_json(metadata_json, prettify);
        free(metadata_json);
    }
    else {
        print_json(json_header, prettify);
    }

    free(json_header);

    return 0;
}
