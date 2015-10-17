/** 
* Mike Holcomb <holcmj@gmail.com> (c) Mike Holcomb
* Example C program to load time data to up to Firebase
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

int main(int argc, char *argv[]) {
  return 0;

}
