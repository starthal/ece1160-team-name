PROC=atmega168

all: lcd_example.o

lcd_example.o: *.c *.h
	avr-gcc -DF_CPU=8000000 -mmcu=$(PROC) *.c -o lcd_example.o
	./avr-mem.sh lcd_example.o $(PROC)

clean:
	rm *.o

install: lcd_example.o
	sudo avarice -2 -j usb -w -p -B 125k --file lcd_example.o
