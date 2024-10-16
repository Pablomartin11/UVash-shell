
# Unix Shell Project - UVash

[![es](https://img.shields.io/badge/lang-es-red.svg)](/README-ES.md)

This project was made for *Operating Systems Structures* subject at University of Valladolid part of the Computer Science Degree which consists on develop a completely functional shell in Unix using the C languange. 
The explanation with more detail resides at [laboratorio-eso](https://github.com/bsahelices/laboratorio-eso/tree/master/Practica2).

Also (increasingly) available are some tests to see if the code works; The testing framework that is
currently available is found [here](https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/tester).

The program can be tested by running the `test-UVash.sh` script.
This could be accomplished by the following commands:
```sh
prompt> git clone https://github.com/Pablomartin11/UVash-shell.git
prompt> cd UVash/
prompt> gcc -o UVash UVash.c -Wall -Werror
prompt> ./test-UVash.sh
test 1: passed
test 2: passed
test 3: passed
test 4: passed
test 5: passed
test 6: passed
test 7: passed
...
prompt> 
```
Otherwise, if you want, you can only run the ```./test-UVash.sh``` command given that i already included the compiled file ```UVash``` in the repository.




