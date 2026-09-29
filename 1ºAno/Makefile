CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -g
TARGET  = logAnalyzer
SRCS    = main.c cli.c worker.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Dependências de cabeçalhos
main.o:   main.c loganalyzer.h cli.h worker.h
cli.o:    cli.c  loganalyzer.h cli.h
worker.o: worker.c loganalyzer.h worker.h

# Teste rápido: cria logs sintéticos e executa
test: $(TARGET)
	@mkdir -p test_logs
	@echo '192.168.1.1 - - [13/Feb/2024:10:00:00 +0000] "GET / HTTP/1.1" 200 512' \
		> test_logs/access.log
	@echo '192.168.1.2 - - [13/Feb/2024:10:00:01 +0000] "POST /login HTTP/1.1" 401 128' \
		>> test_logs/access.log
	@echo '{"timestamp":"2024-02-13T10:00:02Z","level":"ERROR","service":"api","message":"timeout","metadata":{"ip":"10.0.0.1"}}' \
		> test_logs/app.log
	@./$(TARGET) test_logs 2 full --verbose
	@rm -f results_*.txt

clean:
	rm -f $(OBJS) $(TARGET) results_*.txt
	rm -rf test_logs
