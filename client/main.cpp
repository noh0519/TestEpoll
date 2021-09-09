#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024
#define EPOLL_SIZE 1024

int main(int argc, char **argv) {
  // socket variable
  int client_socket = -1;
  struct sockaddr_in server_addr;
  // ~socket variable

  // epoll variables
  int epoll_fd = -1;
  struct epoll_event client_event;
  struct epoll_event *events;
  // ~epoll variables

  client_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    printf("socket create fail\n");
    exit(1);
  }

  memset(&server_addr, 0x00, sizeof(sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(4000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    printf("socket connect fail\n");
    exit(1);
  }

  /* epoll create */
  if ((epoll_fd = epoll_create1(0)) == -1) {
    printf("epoll create fail\n");
    exit(1);
  }

  /**/
  client_event.events = EPOLLIN;
  client_event.data.fd = client_socket;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &client_event);

  /* alloc event */
  events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

  while (1) {
    int event_count = epoll_wait(epoll_fd, events, EPOLL_SIZE, -1);

    if (event_count == -1) {
      printf("epoll wait fail\n");
      exit(1);
    }

    for (int i = 0; i < event_count; i++) {
      int who = events[i].data.fd;
      uint32_t what = events[i].events;

      if (who == client_socket) {
        if (what & EPOLLIN) {
        }
        if (what & EPOLLOUT) {
        }
      } else {
      }
    }
  }

  return 0;
}