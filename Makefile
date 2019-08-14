client:
	gcc client.c log.c -o client -g
server:
	gcc server.c log.c -o server -g
test:
	gcc linklist.c linklist_unittest.c -o linklist_unittest

log:
	gcc log.c log_unittest.c -o log_unittest

clean:
	rm server client linklist_unittest log_unittest *.txt
