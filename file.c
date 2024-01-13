#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
  // Stores the CURL handle used to manage the request and easy API session
  CURL *curl;

  // Stores the return value of the call to curl_easy_perform()
  CURLcode result;

  // File handle to save the response
  FILE *response_file;

  // Open the file for writing
  response_file = fopen("response.txt", "w");

  // Check if the file was opened successfully
  if (response_file == NULL)
  {
    fprintf(stderr, "Failed to open file for writing\n");
    return -1;
  }

  // Starts the session, return the curl handle we'll use to set up the request
  curl = curl_easy_init();

  // If curl_easy_init() fails the function returns an error, we exit with an
  // error message and status in this case
  if (curl == NULL)
  {
    fprintf(stderr, "HTTP request failed\n");
    fclose(response_file);
    return -1;
  }

  // Set the file handle as the target for the response data
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_file);

  // Set the URL for the request
  curl_easy_setopt(curl, CURLOPT_URL, "https://api.openweathermap.org/data/2.5/weather?q=africa&appid=a2730aab71711ac1da5145d01b652ae6");

  // Perform the request and write the response to the file
  result = curl_easy_perform(curl);

  // If the request is OK we'll get back CURLE_OK as a return value, so if we
  // don't get back CURLE_OK something has gone wrong and we'll exit with an
  // error message and status.
  if (result != CURLE_OK)
  {
    // The function curl_easy_strerror() will return an error message string
    // which we output, when it is supplied result as an argument (i.e. the
    // return value from the call to curl_easy_perform()).
    fprintf(stderr, "Error: %s\n", curl_easy_strerror(result));
    fclose(response_file);
    curl_easy_cleanup(curl);
    return -1;
  }

  // Close the file handle
  fclose(response_file);

  // Complete the session
  curl_easy_cleanup(curl);

  return 0;
}
