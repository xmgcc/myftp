client:
	gcc client.c log.c -o client -g utils.c
server:
	gcc server.c log.c -o server -g utils.c linklist.c
test:
	gcc linklist.c linklist_unittest.c -o linklist_unittest -g

log:
	gcc log.c log_unittest.c -o log_unittest -g

clean:
	rm server client linklist_unittest log_unittest *.txt -rf
