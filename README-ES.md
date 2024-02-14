# Shell en Unix - UVash
Este proyecto se realizó para la asignatura de *Estructuras de Sistemas Operativos* en la Universidad de Valladolid, que consiste en desarrollar un shell completamente funcional en Unix con el lenguaje C. La explicación más detallada se encuentra en [laboratorio-eso](https://github.com/bsahelices/laboratorio-eso/tree/master/Practica2)..

También están disponibles (cada vez más) algunas pruebas para verificar si el código funciona; El marco de pruebas que está actualmente disponible se encuentra [aquí](https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/tester). Un script de prueba específico, que se encuentra en el directorio del proyecto, puede ser utilizado para ejecutar las pruebas contra el código.

El programa puede ser probado ejecutando el script test-UVash.sh. Esto se puede lograr mediante los siguientes comandos:
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
De lo contrario, si lo prefieres, simplemente puedes ejecutar el comando ./test-UVash.sh ya que he incluido el archivo compilado ```UVash``` en el repositorio.
