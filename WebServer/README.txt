Web Server

El web server funciona de una manera muy facil.

Instrucciones para compilar:

1.-El archivo Makefile ya esta preparado para compilar, solo se necesita
el comando make y el programa se compilara.

Instrucciones de uso:

1.- Ejecutar el programa, solo se requiere el numero de puerto.
Ejemplo:
./WebServer 80

2.- Con el servidor ejecutando se accesa desde el navegador por medio
de la direccion 127.0.0.1 o localhost seguido del puerto.
Ejemplo:
127.0.0.1:80

3.-Si se ingresa al servidor sin ninguna solicitud extra se enviara
a la pagina de inicio.

4.-El servidor esta programado para descargar un archivo ISO, 
sin embargo debido al tamaño no se ha podido subir, para ingresar 
al iso seria la direccion: 127.0.0.1:80/WindowsXP.iso
De esta manera iniciara con la descarga y la descarga indicara el 
tamaño del archivo.

5.- Si se solicita una pagina no existente se mostrara la pagina de
error.

6.-Para cerrar el Web Server se utiliza el comando ^C 
