cmd_/home/pi/emb-vt/hello/hello.ko := ld -EL -r  -T ./scripts/module-common.lds --build-id  -o /home/pi/emb-vt/hello/hello.ko /home/pi/emb-vt/hello/hello.o /home/pi/emb-vt/hello/hello.mod.o
