/** 
* Mike Holcomb <holcmj@gmail.com> (c) Mike Holcomb
* Example C program to read  time data from  Firebase
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <json-c/json.h>
#include <curl/curl.h>

// Message struct
struct timelog_message_st {
  char *message;
  size_t size;
};

// Request struct
struct timelog_request_st {
  struct timelog_message_st *cf;
  CURL *ch;
  CURLcode rcode;
  const char *url;
  json_object *json;
  enum json_tokener_error jerr;
  struct curl_slist *headers;
};

// Reading struct
struct timelog_reading_st {
  double temp;
  double humidity;
  unsigned int light;
  struct tm *reading_time;
};

struct timelog_request_st *timelog_request_create(const char *url,json_object *json){
  struct timelog_request_st *request = malloc(sizeof (struct timelog_request_st));
 
  request->url = url;
  request->jerr = json_tokener_success;
  request->headers = NULL;

  request->ch = curl_easy_init();

  request->headers = curl_slist_append(request->headers, "Accept: application/json");
  request->headers = curl_slist_append(request->headers, "Content-Type: application/json");

  request->json = json;
  
  curl_easy_setopt(request->ch, CURLOPT_CUSTOMREQUEST, "PUT");
  curl_easy_setopt(request->ch, CURLOPT_HTTPHEADER, request->headers);
  curl_easy_setopt(request->ch, CURLOPT_POSTFIELDS, json_object_to_json_string(request->json));

  request->cf = malloc(sizeof (struct timelog_message_st));

  return request;
}

// Create reading pointer with given sensor readings, time defaults to reading creation
struct timelog_reading_st *timelog_reading_create(double temp, double humidity, unsigned int light){
  struct timelog_reading_st *reading = malloc(sizeof (struct timelog_reading_st));
  reading->temp = temp;
  reading->humidity = humidity;
  reading->light = light;

  time_t rawtime;
  time(&rawtime);
  reading->reading_time = (struct tm *)localtime(&rawtime);

  return reading;
}

// Create json from reading
json_object *timelog_json_create(struct timelog_reading_st *reading){
  json_object *json = json_object_new_object();
  json_object_object_add(json, "temp", json_object_new_double(reading->temp));
  json_object_object_add(json, "humidity", json_object_new_double(reading->humidity));
  json_object_object_add(json, "light", json_object_new_int(reading->light));
  
  return json;
}

// Create resource url from reading timing
char *timelog_url_create(struct timelog_reading_st *reading){
  int max_url_length = 1024;
  char *url = malloc(max_url_length);
  char reading_date[11];
  char reading_seconds[9];

  strftime(reading_date, 11, "%Y-%m-%d",reading->reading_time);
  strftime(reading_seconds,9, "%H:%M:%S",reading->reading_time);  

  snprintf(url, max_url_length,"https://glowing-inferno-9996.firebaseio.com/timelog/%s/%s.json",reading_date, reading_seconds);

  return url;
}

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
  //char *url = "https://glowing-inferno-9996.firebaseio.com/timelog/2015-10-19.json";


  struct timelog_reading_st *reading = timelog_reading_create(90.08,50.05,300);

  char *url = timelog_url_create(reading);
  json_object *json = timelog_json_create(reading);  


  struct timelog_request_st *rh = timelog_request_create(url, json);

  if ( !rh->ch) {
    fprintf(stderr, "ERROR: Failed to create curl handle in timelog_request_init");
    return 1;
  }

  printf("Posting data to: %s\n", url);
  
  rh->rcode = timelog_fetch_url(rh->ch, rh->url, rh->cf);

  curl_easy_cleanup(rh->ch);

  json_object_put(rh->json);

  if (rh->rcode != CURLE_OK || rh->cf->size < 1) {
    fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s", rh->url, curl_easy_strerror(rh->rcode));
    return 2;
  }

  if (rh->cf->message != NULL) {
    printf("Curl returned: \n%s\n", rh->cf->message);
    rh->json = json_tokener_parse_verbose(rh->cf->message, &rh->jerr);
    free(rh->cf->message);
  } else {
    fprintf(stderr, "ERROR: Failed to populate payload");
    return 3;
  }

  if (rh->jerr != json_tokener_success) {
    fprintf(stderr, "ERROR: Failed to parse error string");
    return 4;
  }

  printf("Parsed JSON: %s\n", json_object_to_json_string(rh->json));

  return 0;
}
