main.out:main.o scsi.o link.o test_speed.o net.o
	gcc main.o test_speed.o scsi.o link.o net.o -o main.out
main.o:main.c
	gcc -c -g main.c -o main.o -march=rv64imafd -mabi=lp64d

link.o:link.c link.h scsi.h
	gcc -c -g link.c -o link.o -march=rv64imafd -mabi=lp64d

scsi.o:scsi.c scsi.h
	gcc -c -g scsi.c -o scsi.o -march=rv64imafd -mabi=lp64d

test_speed.o:test_speed.c link.h
	gcc -c -g test_speed.c -o test_speed.o -march=rv64imafd -mabi=lp64d
net.o:net.h
	gcc -c -g net.c -o net.o -march=rv64imafd -mabi=lp64d
clean:
	rm *.o