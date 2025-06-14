#include "Server.hpp"	// Include the Server class header file to define the server functionality.
#include <fcntl.h>		// For fcntl to set the socket to non-blocking mode.
#include <unistd.h>		// For close to close file descriptors.
#include <stdexcept>	// For std::runtime_error to handle exceptions.
#include <cstring>		// For strerror to get error messages from errno.
#include <iostream>		// For std::cerr and std::cout to print error messages and debug information.

Server::Server(const Config& cfg) : config(cfg) {
    server_fd = -1;
	// Inicializa el file descriptor del servidor a -1 para indicar que aún no se ha creado.
    setupSocket();
	// Configura el socket del servidor utilizando la configuración proporcionada.
}

Server::~Server() {
    if (server_fd != -1) {
        close(server_fd);
		// Cierra el socket del servidor si se ha creado.
    }
    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].fd != -1) {
			// Verifica que el file descriptor del socket del cliente sea válido.
            close(fds[i].fd);
			// Cierra cada socket de cliente que se haya abierto si no es -1.
        }
    }
}

void Server::setupSocket() {
    // Obtener puerto de la configuración
    std::string port_str = config.get("port");
    if (port_str.empty()) {
        throw std::runtime_error("No port specified in config");
		// Si no se especifica un puerto en la configuración, lanza una excepción.
    }
    int port = std::atoi(port_str.c_str());
	// Convierte el puerto de string a int.

    // Crear socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
		// Si falla la creación del socket, lanza una excepción con el mensaje de error.
    }

    // Configurar socket como no bloqueante
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1) {
		// Si falla al configurar el socket como no bloqueante, cierra el socket y lanza una excepción.
        close(server_fd);
        throw std::runtime_error("Failed to set socket to non-blocking: " + std::string(strerror(errno)));
    }

    // Configurar dirección
    std::memset(&address, 0, sizeof(address));		// Limpia la estructura de dirección
    address.sin_family = AF_INET;					// Establece la familia de direcciones a IPv4 que es AF_INET
    address.sin_addr.s_addr = INADDR_ANY;			// Permite que el socket escuche en todas las interfaces de red disponibles.
    address.sin_port = htons(port);	
	// Convierte el puerto a formato de red (big-endian) usando htons, para que el servidor pueda escuchar en el puerto especificado.
	// htons convierte el número de puerto de host a formato de red (big-endian) que es el formato utilizado en las redes para transmitir datos.

    // Vincular socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
		// Si falla la vinculación del socket a la dirección y puerto especificados, cierra el socket y lanza una excepción.
        close(server_fd);
        throw std::runtime_error("Failed to bind socket: " + std::string(strerror(errno)));
    }

    // Escuchar conexiones
    if (listen(server_fd, 10) == -1) {
		// Si falla al escuchar en el socket, cierra el socket y lanza una excepción.
        close(server_fd);
        throw std::runtime_error("Failed to listen on socket: " + std::string(strerror(errno)));
    }

    // Agregar socket del servidor a fds
    struct pollfd pfd;		// Estructura pollfd para manejar el socket del servidor.
    pfd.fd = server_fd;		// Asigna el file descriptor del socket del servidor a la estructura pollfd.
    pfd.events = POLLIN;	// Establece el evento POLLIN para indicar que el socket está listo para leer.
    fds.push_back(pfd);		// Agrega la estructura pollfd al vector fds para que pueda ser monitoreada por poll.
}

void Server::handleConnections() {
	// Maneja las conexiones entrantes utilizando poll
    while (true) {
        int ret = poll(&fds[0], fds.size(), -1);
		// Utiliza poll para esperar eventos en los file descriptors en fds.
        if (ret == -1) {
			// Si poll falla, imprime el error y continúa esperando.
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            continue;
        }
        for (size_t i = 0; i < fds.size(); ++i) {
			// Itera sobre los file descriptors en fds
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == server_fd) {
					// Si el evento es en el socket del servidor, significa que hay una nueva conexión entrante.
                    // Nueva conexión
                    struct sockaddr_in client_addr;
					// Estructura para almacenar la dirección del cliente.
                    socklen_t addr_len = sizeof(client_addr);
					// Inicializa la longitud de la dirección del cliente.
                    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
					// Acepta la nueva conexión y obtiene el file descriptor del cliente.
                    if (client_fd == -1) {
						// Si falla al aceptar la conexión, imprime el error y continúa esperando nuevas conexiones.
                        std::cerr << "Accept error: " << strerror(errno) << std::endl;
                        continue;
                    }
                    // Configurar cliente como no bloqueante
                    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
						// Si falla al configurar el socket del cliente como no bloqueante, cierra el socket del
						// cliente y continúa esperando nuevas conexiones.
                        close(client_fd);
                        continue;
                    }
                    // Agregar cliente a fds
                    struct pollfd client_pfd;		// Estructura pollfd para manejar el socket del cliente.
                    client_pfd.fd = client_fd;		// Asigna el file descriptor del socket del cliente a la estructura pollfd.
                    client_pfd.events = POLLIN;		// Establece el evento POLLIN para indicar que el socket del cliente está listo para leer.
                    fds.push_back(client_pfd);		// Agrega la estructura pollfd del cliente al vector fds para que pueda ser monitoreada por poll.
                    std::cout << "New connection: fd " << client_fd << std::endl;
					// Imprime un mensaje indicando que se ha aceptado una nueva conexión con el file descriptor del cliente.
                } else {
                    // Manejar datos del cliente
                    char buffer[1024];
					// Buffer para almacenar los datos recibidos del cliente.
                    ssize_t bytes = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
					// Recibe datos del cliente en el socket correspondiente.
                    if (bytes <= 0) {
                        // Si bytes es menor o igual a 0, significa que ha ocurrido un error o el cliente ha cerrado la conexión.
                        close(fds[i].fd);
						// Cierra el socket del cliente.
                        fds.erase(fds.begin() + i);
						// Elimina el socket del cliente de fds.
                        --i;		// Decrementa i para evitar saltar el siguiente socket en la iteración.
                        continue;	// Continúa con la siguiente iteración del bucle.
                    }
                    buffer[bytes] = '\0';

					// // Asegura que el buffer sea una cadena de caracteres válida al agregar un terminador nulo.
                    // std::cout << "Received: " << buffer << std::endl;
					// // Imprime los datos recibidos del cliente.
                    // // Respuesta simple
                    // std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
					// // Prepara una respuesta simple para enviar al cliente.
                    // send(fds[i].fd, response.c_str(), response.length(), 0);
					// // Envía la respuesta al cliente.

                    Request request;
                    // Crea un objeto Request para manejar la solicitud del cliente.
                    if (request.parse(std::string(buffer))) {
                        // Si la solicitud se parsea correctamente, crea una respuesta.
                        Response response(request, config);
                        // Crea un objeto Response utilizando la solicitud y la configuración del servidor.
                        std::string response_str = response.generate();
                        // Genera la respuesta completa como una cadena de caracteres.
                        send(fds[i].fd, response_str.c_str(), response_str.length(), 0);
                        // Envía la respuesta generada al cliente.
                    } else {
                        // Si la solicitud no se parsea correctamente, envía una respuesta de error.
                        std::string error_response = "HTTP/1.1 400 Bad Request\r\nContent-Length: 22\r\n\r\n<h1>400 Bad Request</h1>";
                        // Prepara una respuesta de error 400 Bad Request.
                        send(fds[i].fd, error_response.c_str(), error_response.length(), 0);
                        // Envía la respuesta de error al cliente.
                    }

                    close(fds[i].fd);
					// Cierra el socket del cliente después de enviar la respuesta.
                    fds.erase(fds.begin() + i);
					// Elimina el socket del cliente de fds.
                    --i;
					// Decrementa i para evitar saltar el siguiente socket en la iteración.
                }
            }
        }
    }
}

void Server::start() {
    handleConnections();
	// Inicia el manejo de conexiones, que se ejecuta indefinidamente.
}