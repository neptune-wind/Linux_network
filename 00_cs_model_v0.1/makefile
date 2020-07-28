src = $(wildcard *.c)
obj = $(patsubst %.c, %.o, $(src))

all: server_p server_t client

server_p: server_p.o wrap.o
	gcc server_p.o wrap.o -o server_p.out
server_t: server_t.o wrap.o
	gcc server_t.o wrap.o -o server_t.out -pthread
client: client.o wrap.o
	gcc client.o wrap.o -o client.out

%.o:%.c
	gcc -c $< -o $@ -Wall

.PHONY:clean all
clean:
	-rm -rf server_p.out client.out $(obj)

