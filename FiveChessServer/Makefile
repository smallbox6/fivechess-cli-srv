CC=g++
OBJECT=common.o mydatabase.o packetsprocess.o plahyagreethread.o fivechess_srv.o
fivechess:$(OBJECT)
	$(CC) -o fivechess $(OBJECT) -levent -pthread -lmysqlclient
.PHONY: clean
clean:
	-rm $(OBJECT)