#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define USERNAME "mygmail@gmail.com"
#define APP_PASSWORD "**** **** **** ****"

struct UploadStatus {
    const char *data;
    size_t size;
};

static size_t ReadCallback(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct UploadStatus *upload_ctx = (struct UploadStatus *)userp;

    size_t to_copy = size * nmemb;
    if (to_copy > upload_ctx->size)
        to_copy = upload_ctx->size;

    if (to_copy) {
        memcpy(ptr, upload_ctx->data, to_copy);
        upload_ctx->data += to_copy;
        upload_ctx->size -= to_copy;
        return to_copy;
    }

    return 0;
}

// Function to read content from a file
char *readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file %s for reading\n", filename);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char *content = (char *)malloc(fileSize + 1);
    if (content == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    fread(content, 1, fileSize, file);
    content[fileSize] = '\0';

    fclose(file);
    return content;
}

int main(void) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set email parameters
        const char *from = USERNAME;
        const char *to = "recipientgmail@gmail.com";
        const char *subject = "ALERT";

        // Read content 
        char *anomaliesContent = readFile("processoutput.txt");

        // Construct the email payload
        char payload[4096];
        snprintf(payload, sizeof(payload), "From: %s\r\nTo: %s\r\nSubject: %s\r\n\r\n%s", from, to, subject, anomaliesContent);

        struct UploadStatus upload_ctx = {payload, strlen(payload)};

        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);

        struct curl_slist *recipients = NULL;
        recipients = curl_slist_append(recipients, to);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_USERNAME, from);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, APP_PASSWORD);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        free(anomaliesContent); // Free allocated memory for anomalies content
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}
