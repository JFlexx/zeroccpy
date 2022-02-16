# practica Zerocpy
Práctica nota: 6.
Pasan todas las pruebas de las 2 primeras fases de 3. 
Si quieres realizar la 3 fase para el 10, únicamente necesitas terminar el GET Bloqueante, tengo un pequeño código que está tapado como comentario.
                                              --> ///** GET bloqueante SIN TERMINAR **/ a partir de la línea 355. 
En mi caso no pasaba esa prueba pero creo que fue por culpa de libzerocopy.Mq que no estaba controlando del todo bien los tamaños de mensaje que se enviaban. Como ya tenía 16 puntos de prácticas no continue intentándolo ya que no valía la pena "en mi caso". 

# Datos
- Para este práctica el envío de mensaje es a libre elección, yo obté por mensaje del tiepo http, tratándolos como cadenas de caracteres.
- Y para cuando existe mensajes en cola usé números: -1: no existe cola, 1: existe cola.
- Práctica compleja sobre todo por el control de mensaje y socktes, infórmate en la documentación pero probalemente es la práctica más dura juntos a la de mutex.

# Comentario de conducta 
Si estas leyendo esto, es por que yo confío en ti o, en su defecto, alguien en quien yo confío confía a la vez en ti.
__La cuestión es:__ Creo firmemente en la libertad de la información, y más si el objetivo es aprender, como debería ser en una universidad. Obviamente, esto no es código profesional, ninguna empresa se verá perjudicada por ello. Pero siguen existiendo correctores de prácticas, ya que desgraciadamente, sigue existiendo gente que copia en vez de basarse en otras cosas. Si subo esto aquí es por que confío en que aquel que lo esté leyendo, lo usará legítimamente. Llámame idealista. Pero... ¿Entonces es un repositorio público?. No. De nuevo, confío en que __TÚ__ usarás esto correctamente. El problema es que si hago este repositorio público, correremos el riesgo de que algún idiota copie y pegue este código, y empiecen a saltar copias por todos lados. A mí me da igual, yo ya tengo aprobado esto, pero imagino que a tí no.

Un saludo.

# Memoria 

********Día 22 febrero:
He empezado con la lectura de la práctica, es decir, la descripción que se proporciona, de dicha práctica, además de ver los enlaces donde muestran ejemplos de socker y demás.
He intentado entender todo lo posible para empezar a programar lo antes posible.
Dado la dificultad de que nunca hemos realizado una práctica de sockets he enviado un correo a Fernanad Costoya.

********Día 25 debrero: Con el documento que se ha subido para la práctica, la cual ha sido de gran ayuda, he empezado ha programar en el broker.c y en libzerocopyMQ.c, empezando con createMq.
He tardado bastante en comprender algunas cosas, además de que he decido seguir los consejos del documento de apoyo. Es decir, realizaré la práctica haciendo streams secuenciales, utilizando writev y read/reacv. 
He tenido problemas a la hora de enviar mensajes del broker al "cliente", ya que nose como deberían ser.

********Día 27 febrero: He empezado intentando solucionar/entender como enviar mensajes al cliente, ya que nose como o que clase de mensaje se deberían enviar, y tampoco se cuando, así que por ahora enviaré mensajes del tipo "httCorrecto" o "httperror" si hay algun problema.
He terminado de realizar createMQ.
He comprobado que funcione correctamente.

********Día 28 febrero: He usado las libs que se nos proporciona para saber si realmente se crean las entradas en el diccionario usando dic_visit() como lo realizan en demo_dic.c.
Parece que funciona así que doy paso a realizar destroyMq.

********Día 3 de marzo: He empezado con destroyMq que parece bastante similar a createMQ ya que la metodología es la misma, ya que en el broker solo he tenido que borrar la cola y la entrada de la cola del diccionario que se nos proporciona.
En la parte de LibzerocopyMQ.c es literalmente el mismo código que en la función createMq.c solo cambia el char que le paso para saber el tipo de operación, es decir, "C".

********Día 5 de marzo: He empezado a comprobar la función destroyMq y por los mensajes en consola de cola creada y borrada correctamente doy por echo que está bien.

********Día 7 de marzo: envío mi primera correción, la cual, he obtenido un 0:(.
El error es el siguiente: UNA COLA CON NOMBRE CORTO Y ENVÍO Y RECEPCIÓN DE MENSAJE CORTO
	 CREATEMQ ERROR EN CONEXIÓN CON EL BROKER: 
Ese mismo día envié otra prueba.
 
Solución: al realizar algunas pruebas, toque las variables de entorno sin querer y me percate con la segunda prueba.
Cuando solucioné este tipo de error, volví a obtener un 0,  el error era el siguiente: UNA COLA CON NOMBRE CORTO Y ENVÍO Y RECEPCIÓN DE MENSAJE CORTO ERROR POR CAÍDA DEL BROKER
Este error no lo comprendía. Finalmente localicé el error, y es que había gente con el mismo error que yo, diciendome que el error estaba probablemente en que realizaba un "return -1" en el broker, y que bastaba con solo cerrar la conexión, pero siempre manteniendome en el bucle while.
Envíe otra prueba al final del día.


********Día 8 de marzo: Volví a obtener un 0.
Esta vez el error estaba en: UNA COLA CON NOMBRE CORTO Y ENVÍO Y RECEPCIÓN DE MENSAJE CORTO CREATEMQ ERROR POR NO RECIBIR RESPUESTA DEL BROKER

Esto me costó darme cuenta, y es que simplemente cuando realizaba dic_put() poniendo la cola en la entrada del diccionario, no envía la contestación del broker de que todo había ido correcto, es decir, no estaba usando en ningún momento writev(), lo cual, pensaba que si lo había puesto. solo usaba writev() cuando había algún error, pero no cuando todo iba correcto.

Envié otra prueba el mismo día, y me volvió a dar el mismo error, pero estaba vez era porque el char que le pasaba para saber la operación era del tipo "C"(string) cuando debería ser 'C', este cambio lo hice en el libzerocopyMQ.c.

********Día 9 de marzo: Volví a obtener un 0.
error: UNA COLA CON NOMBRE CORTO Y ENVÍO Y RECEPCIÓN DE MENSAJE CORTO PUT ERROR EN ENVÍO AL BROKER

Al parecer para la primera prueba según me comentaron en el grupo de clase, es necesario tener el resto de operaciones hechas. Así que he empezado con el resto de operaciones que me quedaba, PUT y GET. 

/***esta semana entera le dediqué al día entre 2/3 horas a la práctica, por eso he avanzado lento, ya que había otra práctica de otra asignatura que era más urgente.
Horas dedicadas en total: 2x7=14 h, se espera mayor redimiento esta semana dado que tengo mucho más tiempo y entiendo mucho mejor la práctica, así como los errores que me pueden dar.**/

********Día 10 de marzo: como tengo muchas más tiempo, le dedicaré a terminar todas las operaciones que me quedan para enviar otra correción.
-Terminé de realizar put tanto del broker como del "cliente" 
-Empecé con el Get no bloqueante en la parte del broker

*******Día 11 de marzo: 
-Terminé de realizar el Get en la parte de biblioteca.
-Envié una prueba al corrector.
-Obtuve por fín algo de nota, exactamente un 2, y el error era el siguiente: PRUEBA 3: SE CREAN INICIALMENTE VARIAS COLAS Y SE ENVÍAN VARIOS MENSAJES, GET ERROR EN TAMAÑO DE MENSAJE RECIBIDO POR CLIENTE
-Dado el error en el tamaño recibido, fui directamente a la varibale que contenía el tamaño del mensaje "tamMsg" en el broker,  y me di cuenta que no la estaba reseteando/limpiando cada vez que entraba a la operación Get lo cual provocaba conflicto en el tamaño. En resumen solo tuve que poner al principio de la operación GET del broker "tamMsg=NULL".
-Buscaba algún otro error que tuviera que ver con el tamaño del mensaje. pero no lo encontré.

- Volví a pasar otra prueba por el corrector.
- Volví a obtener un 2 y el error volía a ser el mismo. Así que volví a recibir la parte del cliente y me percaté que cuando realizaba en envío de petición al broker al usar la estructura iovec no envíaba la dirección de inicio de la longitud de la cola, es decir, en vez de enviar "iov[1].iov_base=&longitud" enviaba "iov[1].iov_base=longitud", sin su dirección. 
-Con esto volví a enviar otra prueba.

*******Día 12 de marzo:
-Consulté el resultado de la prueba,  y volve a dar el mismo error -_-, es decir, volví a obtener un 2.
-Volví a consultar el código del cliente(de la biblioteca), y encontré otro error, y es que cuando esperaba la respuesta no estaba cumpliendo con los requisitos del mensaje, es decir, no eran del tipo uint32_t el tamaño de los mensaje recibidos, sino int.
- Lo solucione cambiando en el read(de espera de respuesta) que está después de hacer el writev, haciendo lo siguiente--> "if(read(s, tam, sizeof(int))<0)" lo cambié por esto "if(read(s, tam, sizeof(uint32_t))<0)".

-Con estos errores del tamaño también me percaté que no estaba teniendo encuenta que se cumpliera el tamaño del nombre de la cola de 2¹⁶, ni tampoco del tamaño del mensaje de 2³², ni en Create, ni en Destroy, ni Get ni Put. Por tanto supuse que las próximas pruebas comprobarían esto.

-Envié la siguiente prueba y lo comprobé esa misma noche, donde finalmente obtuve un 6, es decir, terminé la primera fase del proyecto.

***** Día 13 de marzo 
-Como ya superé la primera fase, antes de empezar con la segunda fase, decidí dedicar tiempo a factorizar mi código usando funciones auxiliares, en general arreglar un poco mi código, ya que tenía variables que las declaré pero que finalmente no las usé, o código que se repetía demasiado, como en el cliente que para realizar la operación antes siempre se conectaba al broker de la misma manera, asi que pretendo implementar funciones para que no sea tan largo el código.

****** Día 14 de marzo
- Empecé también poniendo comentarios explicando lo que hacía cada variable auxiliar o algún aspecto que veía necesario explicar con comentarios.
- También borré comentarios innecesarios que solo me servían para mi.
- Realice pruebas mías para comprobar que todo seguía funcionando correctamente.
- Como parecía que no había ningún fallo decidí realizar otra correción por se acaso.

**** Día 15 de marzo: Obtuve nuevamente un 6, es decir, primera fase terminada:).

**** Día 16 de marzo tengo empezado empezar con la segunda fase...
**** Día 29 de marzo empezaŕe la siguiente práctica grupal y en caso de estar bien de tiempo intentaré realizar la siguiente fase de este proyecto ya que las prácticas grupales pretendo hacerlas solo*****

**** Día 14 de mayo, Finalmente no me dio tiempo a realizar la segunda
- Pero realizaré otro envío de la práctica para comprobar que todo esté bien.
- Nose si borrar el código que estaba intentando de la segunda fase o dejarlo.

*** Día 20, 21 y 22** intentando llegar al 10 o al 8, ya que necesito 4 puntos en las prácticas
para aprobar



