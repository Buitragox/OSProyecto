# Servidor Web - Proyecto Sistemas Operativos

Trabajo hecho por: Sebastian Arango y Jhoan Buitrago

### Cambios al proyecto original

* Se agregó una carpeta con los archivos necesarios para el funcionamiento de sqlite3.

* Se agregó msg.c y msg.h que manejan las funcionalidades del paso de mensajes y manejo de los requests HTTP.

* Se agregó list.c y list.h que manejan las funciones necesarias para una priority queue usando linked-list.

* Se le hicieron multiples cambios a wserver.c para agregar las nuevas funcionalidades requeridas.

* Se agregó testRequest.py que permite enviar multiples peticiones al servidor.

* Se cambió el makefile para la compilación de sqlite3, msg.c. Se agregaron banderas nuevas para que el linker reconozca las librerias necesarias.

### Ejecutar el servidor web

Se debe cambiar el directorio en msg.h al directorio absoluto donde tenga el proyecto, posteriormente se compilan los archivos.

Con la instrucción make se compilan todos los archivos necesarios y con make clean se eliminan.

Es posible que sea necesario instalar la librería __libuuid__ si no la tiene ya. Se puede obtener usando "sudo apt-get install uuid-dev".

Una vez compilado ejecutamos wserver con los siguientes argumentos:

- ./wserver -d DIRECTORIO -p PUERTO -b BUFFERS -s SCHEDALG

    - DIRECTORIO: se refiere a la ruta raiz donde se abrira el servidor web. Se recomienda usar ./website
    
    - PUERTO: puerto donde quedará el servidor. Ej: 8080

    - BUFFERS: Cuantas peticiones se trabajaran al tiempo por hilos.

    - SCHEDALG: Algoritmo de ordenamiento de las peticiones. Estan disponible FIFO y SFF.

El script de python testRequest.py permite enviar n peticiones al servidor web una vez este funcionando. Se ejecuta con los siguientes argumentos:

- python3 testRequest.py CANTIDAD_DE_PROCESOS PUERTO

