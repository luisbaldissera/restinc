#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Http {{{
#define HTTP_REQUEST_PATH_MAX_SIZE (1 << 8)

#define HTTP_HEADER_MAX_COUNT (1 << 8)
#define HTTP_HEADER_KEY_MAX_SIZE (1 << 8)
#define HTTP_HEADER_VALUE_MAX_SIZE (1 << 8)

#define HTTP_SERVER_PORT atoi(getenv("HTTP_SERVER_PORT"))

struct HHeader {
  char key[HTTP_HEADER_KEY_MAX_SIZE];
  char value[HTTP_HEADER_VALUE_MAX_SIZE];
};

struct HReq {
  char path[HTTP_REQUEST_PATH_MAX_SIZE];
  struct HHeader head[HTTP_HEADER_MAX_COUNT];
  int headc;
  FILE *body;
};

char *HTTP_header(struct HReq *req, const char *key, const char *defval);

struct HResp {
  int status;
  struct HHeader head[HTTP_HEADER_MAX_COUNT];
  int headc;
  FILE *body;
};

void HTTP_handle(struct HReq *req, struct HResp *resp);

// }}}

// Server {{{
static volatile sig_atomic_t stop_server = 0;
void SIGINT_handler(int _);
void handle_request(FILE *req, FILE *resp);
void HReq_read(struct HReq *req, FILE *in);
void HResp_write(struct HResp *resp, FILE *out);

int main() {
  char str[256];
  const char *test = str;
  stop_server = 0;
  signal(SIGINT, SIGINT_handler);
  int err, server_sck, session_sck, read_size;
  struct sockaddr_in addr;
  socklen_t addrlen;
  if ((server_sck = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creation failure");
    exit(EXIT_FAILURE);
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(HTTP_SERVER_PORT);
  addrlen = sizeof(addr);
  if ((err = bind(server_sck, (const struct sockaddr *)&addr, sizeof(addr))) !=
      EXIT_SUCCESS) {
    perror("socket bind failure");
    exit(EXIT_FAILURE);
  }
  if ((err = listen(server_sck, 0)) != EXIT_SUCCESS) {
    perror("socket listen failure");
    exit(EXIT_FAILURE);
  }
  printf("Server started on port %i...\n", HTTP_SERVER_PORT);
  while (!stop_server &&
         (session_sck =
              accept(server_sck, (struct sockaddr *)&addr, &addrlen)) >= 0) {
    FILE *req = fdopen(session_sck, "r");
    FILE *resp = fdopen(session_sck, "a");
    handle_request(req, resp);
    fflush(resp);
    fclose(req);
    fclose(resp);
    close(session_sck);
  }
  perror("server exited");
  close(server_sck);
}

void handle_request(FILE *req, FILE *resp) {
  char respbody[32];
  struct HReq http_req;
  struct HResp http_resp;
  HReq_read(&http_req, req);
  http_resp.body = fmemopen(respbody, sizeof(respbody), "a+");
  HTTP_handle(&http_req, &http_resp);
  fflush(http_resp.body);
  HResp_write(&http_resp, resp);
  fclose(http_resp.body);
}

const char *HTTP_status_message(int status) {
  switch (status) {
  case 200:
    return "OK";
  case 201:
    return "Created";
  case 202:
    return "Accepted";
  case 204:
    return "No Content";
  case 400:
    return "Bad Request";
  case 401:
    return "Unauthorized";
  case 403:
    return "Forbidden";
  case 404:
    return "Not Found";
  case 405:
    return "Method Not Allowed";
  case 500:
    return "Internal Server Error";
  case 501:
    return "Not Implemented";
  case 503:
    return "Service Unavailable";
  }
  return "OK";
}

void HReq_print(struct HReq *req, FILE *out) {
  fprintf(out, "path: %s\n", req->path);
  fprintf(out, "headc: %i\n", req->headc);
  for (int i = 0; i < req->headc; i++) {
    fprintf(out, "head[%i]: %s: %s\n", i, req->head[i].key, req->head[i].value);
  }
  fprintf(out, "body: %p\n", req->body);
}

void HReq_read(struct HReq *req, FILE *in) {
  char method[16], path[256], proto[16];
  fscanf(in, "%s %s %s\r\n", method, path, proto);
  strcpy(req->path, path);
  req->headc = 0;
  fflush(in);
  char key[HTTP_HEADER_KEY_MAX_SIZE], val[HTTP_HEADER_VALUE_MAX_SIZE];
  while (fscanf(in, "%[^:]: %s\r\n", key, val) == 2) {
    fflush(in);
    for (int i = 0; key[i]; i++) {
      key[i] = tolower(key[i]);
    }
    strcpy(req->head[req->headc].key, key);
    strcpy(req->head[req->headc].value, val);
    req->headc++;
  }
  if (fscanf(in, "\n") == EOF) {
    req->body = NULL;
  } else {
    req->body = in;
  }
}

void HResp_write(struct HResp *resp, FILE *out) {
  fprintf(out, "HTTP/1.1 %i %s\r\n", resp->status,
          HTTP_status_message(resp->status));
  for (int i = 0; i < resp->headc; i++) {
    fprintf(out, "%s: %s\r\n", resp->head[i].key, resp->head[i].value);
  }
  fprintf(out, "\r\n");
  if (resp->body) {
    char buf[1 << 8];
    while (fgets(buf, sizeof(buf), resp->body)) {
      fprintf(out, "%s", buf);
    }
  }
  fflush(out);
}

void HTTP_handle(struct HReq *req, struct HResp *resp) {
  resp->status = 200;
  strcpy(resp->head[resp->headc].key, "x-response-header");
  strcpy(resp->head[resp->headc].value, "response-value");
  resp->headc = 1;
  HReq_print(req, stdout);
  HReq_print(req, resp->body);
  fflush(resp->body);
}

void SIGINT_handler(int _) {
  if (!stop_server) {
    printf("graceful shutdown\n");
    stop_server = 1;
  } else {
    exit(EXIT_FAILURE);
  }
}

// }}}

// vim: fdm=marker
