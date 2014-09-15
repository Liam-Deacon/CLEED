gcc -I"../include" -Wall latt.c basis.c coord.c lattice.c latt_help.c latt_parse_args.c -lm -o new_latt.exe -std=c11 -lstdc++ -DDEBUG=1 -g3 
