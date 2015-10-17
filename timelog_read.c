/** 
* Mike Holcomb <holcmj@gmail.com> (c) Mike Holcomb
* Example C program to read  time data from  Firebase
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <json-c/json.h>
#include <curl/curl.h>

// Message struct
struct timelog_message_st {
  char *message;
  size_t size;
};

// Callback
size_t timelog_callback (void *message, size_t size, size_t nmemb, void *userp) {
  size_t buffer_size = size * nmemb;
  struct timelog_message_st *p = (struct timelog_message_st *)userp;

  p->message = (char *) realloc(p->message, p->size + buffer_size + 1);

  if (p->message == NULL) {
    fprintf(stderr, "ERROR: Failed to expand buffer in timelog_callback" );
    free(p->message);
    return -1;
  }
  
  memcpy(&(p->message[p->size]), message, buffer_size);
  
  p->size += buffer_size;

  p->message[p->size] = 0;

  return buffer_size;
}

CURLcode timelog_fetch_url(CURL *ch, 
                           const char *url,
                           struct timelog_message_st *fetch) 
{
  CURLcode rcode;  


  fetch->message = (char *) calloc(1, sizeof(fetch->message));

  if (fetch->message == NULL) {
    fprintf(stderr, "ERROR: Failed to allocate payload in timelog_fetch_url");
    return CURLE_FAILED_INIT;
  }

  fetch->size = 0;

  curl_easy_setopt(ch, CURLOPT_URL, url);
  curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, timelog_callback);
  curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *) fetch);
  curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  curl_easy_setopt(ch, CURLOPT_TIMEOUT, 5);
  curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

  rcode = curl_easy_perform(ch);

  return rcode;
}


int main(int argc, char *argv[]) {
  return 0;

}
