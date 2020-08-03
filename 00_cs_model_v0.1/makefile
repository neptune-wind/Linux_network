src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

server_p: server_p.o wrap.o
	gcc server_p.o wrap.o -o server_p.out
server_t: server_t.o wrap.o
	gcc server_t.o wrap.o -o server_t.out -pthread
server_select: server_select.o wrap.o
	gcc server_select.o wrap.o -o server_select.out
server_poll: server_poll.o wrap.o
	gcc server_poll.o wrap.o -o server_poll.out
server_epoll: server_epoll.o wrap.o
	gcc server_epoll.o wrap.o -o server_epoll.out
client: client.o wrap.o
	gcc client.o wrap.o -o client.out
server_epoll_nonblock: server_epoll_nonblock.o wrap.o
	gcc server_epoll_nonblock.o wrap.o -o server_epoll_nonblock.out
client_epoll_nonblock: client_epoll_nonblock.o wrap.o
	gcc client_epoll_nonblock.o wrap.o -o client_epoll_nonblock.out	

%.o:%.c
	gcc -c $< -o $@ -Wall

.PHONY:clean all
clean:
	-rm -rf *.out *.o$(obj)

