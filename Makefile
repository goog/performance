TARGET=send_performance

$(TARGET):main.c get_usage.c
	gcc -o $@ $^


install:
	#cp -f $(TARGET) /usr/local/bin/   

clean:
	rm -rf $(TARGET) *.o
