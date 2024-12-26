#include <stdlib.h>
#include <string.h>

#include "http.h"

struct HTTPRequest {
  enum HTTPMethod method;
  char path[HTTP_REQUEST_PATH_MAX_SIZE];
  char *headers[HTTP_HEADER_MAX_COUNT];
  int headersc;
  FILE *body;
};

struct HTTPResponse {
  enum HTTPStatus status;
  FILE *body;
};

struct HTTPRequest * HTTPRequest_new() {
  struct HTTPRequest *req = malloc(sizeof(struct HTTPRequest));
  req->method = HTTP_METHOD_UNKNOWN;
  req->headersc = 0;
  req->body = NULL;
  return req;
}

enum HTTPMethod HTTPRequest_method(struct HTTPRequest *req) {
  return req->method;
}

enum HTTPMethod HTTP_method(const char *method) {
  if (strcmp(method, "GET") == 0) {
    return HTTP_GET;
  } else if (strcmp(method, "POST") == 0) {
    return HTTP_POST;
  } else if (strcmp(method, "PUT") == 0) {
    return HTTP_PUT;
  } else if (strcmp(method, "DELETE") == 0) {
    return HTTP_DELETE;
  } else if (strcmp(method, "OPTIONS") == 0) {
    return HTTP_OPTIONS;
  } else if (strcmp(method, "PATCH") == 0) {
    return HTTP_PATCH;
  } else if (strcmp(method, "HEAD") == 0) {
    return HTTP_HEAD;
  } else if (strcmp(method, "CONNECT") == 0) {
    return HTTP_CONNECT;
  } else if (strcmp(method, "TRACE") == 0) {
    return HTTP_TRACE;
  } else {
    return HTTP_METHOD_UNKNOWN;
  }
}

const char *HTTPMethod_word(enum HTTPMethod method) {
  switch (method) {
    case HTTP_GET:
      return "GET";
    case HTTP_POST:
      return "POST";
    case HTTP_PUT:
      return "PUT";
    case HTTP_DELETE:
      return "DELETE";
    case HTTP_OPTIONS:
      return "OPTIONS";
    case HTTP_PATCH:
      return "PATCH";
    case HTTP_HEAD:
      return "HEAD";
    case HTTP_CONNECT:
      return "CONNECT";
    case HTTP_TRACE:
      return "TRACE";
    default:
      return NULL;
  }
}

const char *HTTPRequest_path(struct HTTPRequest *req) {
  return req->path;
}

int HTTPRequest_headers(struct HTTPRequest *req, char **keys) {
  for (int i = 0; i < req->headersc; i++) {
    strcpy(keys[i], req->headers[i]);
  }
  return req->headersc;
}

const char *HTTPRequest_getheader(struct HTTPRequest *req, const char *key) {
  for (int i = 0; i < req->headersc; i++) {
    if (strcmp(req->headers[i], key) == 0) {
      return req->headers[i];
    }
  }
  return NULL;
}

FILE *HTTPResponse_body(struct HTTPResponse *resp) {
  fseek(resp->body, 0, 0);
  return resp->body;
}

void HTTPResponse_setstatus(struct HTTPResponse *resp, enum HTTPStatus status) {
  resp->status = status;
}

int HTTPRequest_fscan(FILE *in, struct HTTPRequest *req) {
  char method[HTTP_REQUEST_PATH_MAX_SIZE];
  if (fscanf(in, "%s %s", method, req->path) != 2) {
    return 0;
  }
  req->method = HTTP_method(method);
  char header[HTTP_REQUEST_PATH_MAX_SIZE];
  while (fscanf(in, "%s", header) == 1) {
    req->headers[req->headersc] = malloc(strlen(header) + 1);
    strcpy(req->headers[req->headersc], header);
    req->headersc++;
  }
  return 1;
}

void HTTPRequest_free(struct HTTPRequest *req) {
  for (int i = 0; i < req->headersc; i++) {
    free(req->headers[i]);
  }
  free(req);
}

void HTTPResponse_free(struct HTTPResponse *resp) {
  free(resp);
}

struct HTTPResponse * HTTPResponse_new() {
  struct HTTPResponse *resp = malloc(sizeof(struct HTTPResponse));
  resp->status = HTTP_STATUS_UNKNOWN;
  resp->body = tmpfile();
  return resp;
}
