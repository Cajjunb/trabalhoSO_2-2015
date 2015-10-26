Elaborado por:
Leandro Ramalho - 10/0033571
Eduardo Furtado - 09/0111575


versões do ambiente:

gcc --version
gcc (SUSE Linux) 4.8.3 20140627 [gcc-4_8-branch revision 212064]
Copyright (C) 2013 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

g++ --version
g++ (SUSE Linux) 4.8.3 20140627 [gcc-4_8-branch revision 212064]
Copyright (C) 2013 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.






compilação e execução:


killall -9 print;killall -9 servidor;clear;g++ -Wall remove_postergado.c -o remove_postergado;g++ -Wall shutdown_postergado.c -o shutdown_postergado;g++ -Wall executa_postergado.c -o executa_postergado ;gcc -Wall print.c -o print; g++ -Wall servidor.c -o servidor;./executa_postergado ./print 0 0 1; ./executa_postergado ./print 0 1 3; ./executa_postergado ./print 0 1 2;./servidor


COMANDO DE REMOVER JOB
./remove_postergado jobID


gcc -Wall print.c -o print;


util: 
git add . --all ; git commit -m ""; git push origin usandoVector;


watch -n 1 'ps -aux -e | head -15 | grep print'

ps -aux | grep print

