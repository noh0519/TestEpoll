#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>

#include <iostream>
// #include <sys/types.h>
#include <sys/epoll.h>

#define BUFF_SIZE 1024
#define EPOLL_SIZE 1024

int main(int argc, char **argv) {
  // socket variables
  int server_socket;
  // int client_socket;

  struct sockaddr_in server_addr;
  // struct sockaddr_in client_addr;
  // socklen_t client_addr_size;

  char buff_rcv[BUFF_SIZE + 5];
  char buff_snd[BUFF_SIZE + 5];
  // ~socket variables

  // epoll variables
  int server_epoll_fd = -1;
  int get_event_count = 0;
  struct epoll_event server_event;
  struct epoll_event *client_events;
  // ~epoll variables

  server_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (-1 == server_socket) {
    printf("socket 생성 실패\n");
    exit(1);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(4000);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (-1 == bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
    printf("bind 실패\n");
    exit(1);
  }

  if (-1 == listen(server_socket, 5)) {
    printf("listen 모드 설정 실패\n");
    exit(1);
  }

  // server epoll create
  if ((server_epoll_fd = epoll_create1(1)) == -1) {
    printf("server epoll 생성 실패\n");
    exit(1);
  }

  //
  server_event.events = EPOLLIN;
  server_event.data.fd = server_socket;
  epoll_ctl(server_epoll_fd, EPOLL_CTL_ADD, server_socket, &server_event);

  // alloc client_events
  client_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

  while (1) {
    get_event_count = epoll_wait(server_epoll_fd, client_events, EPOLL_SIZE, -1);

    if (get_event_count == -1) {
      printf("sdxzdefsdafdsafsadfdaskljfaskdjhflk jasd jasfdk fjsadkfj skdajf sdakjf sd jfsdakj fepoll_wait 실패\n");
      exit(1);
    }
  }

  return 0;
}