
#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <time.h>

// Function to retrieve JSON response from a file
char *retrieveJsonResponseFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file %s for reading\n", filename);
        exit(EXIT_FAILURE);
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Allocate memory to store file content
    char *jsonResponse = (char *)malloc(fileSize + 1);
    if (jsonResponse == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Read file content into the buffer
    fread(jsonResponse, 1, fileSize, file);

    // Null-terminate the content
    jsonResponse[fileSize] = '\0';

    fclose(file);

    return jsonResponse;
}

// Function to write data to a file
void appendDataToFile(const char *filename, const char *data) {
    FILE *file = fopen(filename, "a"); // Open file in append mode
    if (file == NULL) {
        fprintf(stderr, "Failed to open file %s for writing\n", filename);
        exit(EXIT_FAILURE);
    }

    // Add timestamp
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    fprintf(file, "Timestamp: %s", asctime(timeinfo));

    // Write data to the file
    fprintf(file, "%s\n", data);

    fclose(file);
}

// Function to extract information from JSON
void processJsonData(const char *json_response) {
    json_t *root;
    json_error_t error;

    root = json_loads(json_response, 0, &error);

    if (!root) {
        fprintf(stderr, "JSON parsing error: %s\n", error.text);
        free((void *)json_response); // Free memory allocated for json_response
        exit(EXIT_FAILURE);
    }

    // Extract specific information from the JSON
    json_t *name = json_object_get(root, "name");
    const char *city_name = json_is_string(name) ? json_string_value(name) : "";



    json_t *main_object = json_object_get(root, "main");
    json_t *temperature = json_object_get(main_object, "temp");
    double temp_value = json_is_real(temperature) ? json_real_value(temperature) : 0.0;

    json_t *wind_object = json_object_get(root, "wind");
    json_t *wind_speed = json_object_get(wind_object, "speed");
    double wind_speed_value = json_is_real(wind_speed) ? json_real_value(wind_speed) : 0.0;

    // Print extracted information
    printf("City Name: %s\n", city_name);
    printf("Temperature: %.2f°C\n", temp_value);
    printf("Wind Speed: %.2f m/s\n", wind_speed_value);

    // Write extracted information to the file
    char outputData[256]; // Adjust the size as needed
    snprintf(outputData, sizeof(outputData), "City Name: %s\nTemperature: %.2f°C\nWind Speed: %.2f m/s\n", 
             city_name, temp_value, wind_speed_value);
    appendDataToFile("processoutput.txt", outputData);

    // Release resources
    json_decref(root);
    free((void *)json_response); // Free memory allocated for json_response
}

int main(void) {
    // Replace "json_response.txt" with the actual filename or path
    const char *json_response_filename = "response.txt";

    // Retrieve JSON response from a file
    const char *json_response = retrieveJsonResponseFromFile(json_response_filename);

    // Process and append data to the file
    processJsonData(json_response);

    return 0;
}


