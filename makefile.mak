all:
	gcc src/order_gen.c -o order_gen -lpthread
	gcc src/kitchen.c -o kitchen
	gcc src/logger.c -o logger
	gcc src/main.c -o monitor

run:
	./monitor

clean:
	rm -f order_gen kitchen logger monitor orders.txt