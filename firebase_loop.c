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

struct timelog_request_st {
  struct timelog_message_st *message;
  CURL *ch;
  CURLcode rcode;
  char *url;
  json_object *json;
  enum json_tokener_error jerr;
  struct curl_slist *headers;
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
  CURL *ch; //curl handle
  CURLcode rcode; //result code
  
  json_object *json; //json post body
  enum json_tokener_error jerr = json_tokener_success; // json parse error

  struct timelog_message_st curl_fetch;
  struct timelog_message_st *cf = &curl_fetch;
  struct curl_slist *headers = NULL;

  char *url = "https://glowing-inferno-9996.firebaseio.com/timelog/2015-10-19.json";
  
  //Init curl handle
  if ((ch = curl_easy_init()) == NULL) {
    fprintf(stderr, "ERROR: Failed to create curl handle in fetch_session");
    return 1;
  }

  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");

  json = json_object_new_object();
  json_object_object_add(json, "title", json_object_new_string("testies"));
  json_object_object_add(json, "body", json_object_new_string("testies...blah...blah"));
  json_object_object_add(json, "userId", json_object_new_int(123));

  curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, "PUT");
  curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers); 
  curl_easy_setopt(ch, CURLOPT_POSTFIELDS, json_object_to_json_string(json));

  printf("Posting data to: %s\n", url);
  
  rcode = timelog_fetch_url(ch, url, cf);

  curl_easy_cleanup(ch);

  json_object_put(json);

  if (rcode != CURLE_OK || cf->size < 1) {
    fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s", url, curl_easy_strerror(rcode));
    return 2;
  }

  if (cf->message != NULL) {
    printf("Curl returned: \n%s\n", cf->message);
    json = json_tokener_parse_verbose(cf->message, &jerr);
    free(cf->message);
  } else {
    fprintf(stderr, "ERROR: Failed to populate payload");
    return 3;
  }

  if (jerr != json_tokener_success) {
    fprintf(stderr, "ERROR: Failed to parse error string");
    return 4;
  }

  printf("Parsed JSON: %s\n", json_object_to_json_string(json));
  return 0;

}
