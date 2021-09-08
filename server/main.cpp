#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
// #include <sys/types.h>
#include <sys/epoll.h>

#define BUFF_SIZE 1024
#define EPOLL_SIZE 1024

int main(int argc, char **argv) {
  // socket variables
  int server_socket;
  struct sockaddr_in server_addr;

  char buff_rcv[BUFF_SIZE];
  char buff_snd[BUFF_SIZE];
  // ~socket variables

  // epoll variables
  int epoll_fd = -1;
  struct epoll_event server_event;
  struct epoll_event *events;
  // ~epoll variables

  server_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (-1 == server_socket) {
    printf("socket 생성 실패\n");
    exit(1);
  }

  int optval = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

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
  // if ((epoll_fd = epoll_create1(100)) == -1) {
  if ((epoll_fd = epoll_create1(0)) == -1) {
    printf("server epoll 생성 실패\n");
    exit(1);
  }

  //
  server_event.events = EPOLLIN | EPOLLRDHUP;
  server_event.data.fd = server_socket;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &server_event);

  // alloc events
  events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

  while (1) {
    printf("\nStart Epoll Wait\n");
    int event_count = epoll_wait(epoll_fd, events, EPOLL_SIZE, -1);

    if (event_count == -1) {
      printf("실패\n");
      exit(1);
    }
    printf("Get Event Count : %d\n", event_count);
    for (int i = 0; i < event_count; i++) {
      // Server Socket Event
      if (events[i].data.fd == server_socket) {
        uint32_t what = events[i].events;
        printf("Start Server FD Event (%u)\n", what);

        if (what & EPOLLIN == EPOLLIN) {
          int client_socket;
          struct sockaddr_in client_addr;
          socklen_t client_addr_size;

          client_addr_size = sizeof(client_addr);
          client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);

          struct epoll_event client_event;
          client_event.events = EPOLLIN | EPOLLOUT;
          client_event.data.fd = client_socket;
          epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &client_event);
        } else if (what & EPOLLOUT == EPOLLOUT) {
        }

        printf("End Server FD Event\n");
      } else { // Client Socket Event
        uint32_t what = events[i].events;
        printf("Start Client FD Event (%u)\n", what);

        if (what & EPOLLIN == EPOLLIN) {
          memset(buff_rcv, 0x00, BUFF_SIZE);
          recv(events[i].data.fd, buff_rcv, BUFF_SIZE, 0);
          printf("Recv Data : %s\n", buff_rcv);

          memset(buff_snd, 0x00, BUFF_SIZE);
          snprintf(buff_snd, BUFF_SIZE, "Recv Data Is %s\n", buff_rcv);
          send(events[i].data.fd, buff_snd, BUFF_SIZE, 0);
        } else if (what & EPOLLOUT == EPOLLOUT) {
          sleep(1);
        }

        printf("End Client FD Event\n");
      }
    }
  }

  return 0;
}
