#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Bind the socket to the loopback address and a specific port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(12345);  // Change the port as needed
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Use the loopback address
    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    // Listen for incoming connections
    listen(server_socket, 1); // Allow only one client to connect at a time

    // Accept a connection from a client
    int client_socket = accept(server_socket, NULL, NULL);

    // Open and read the video file
    std::ifstream video_file("video.mp4", std::ios::binary);
    if (!video_file) {
        std::cerr << "Failed to open video file." << std::endl;
        return 1;
    }

    char buffer[1024];
    int bytes_read;

    while (!video_file.eof()) {
        // Wait for a synchronization signal from the client
        recv(client_socket, buffer, sizeof(buffer), 0);

        video_file.read(buffer, sizeof(buffer));
        bytes_read = video_file.gcount();

        // Send video data to the client
        send(client_socket, buffer, bytes_read, 0);
    }

    // Close the sockets when done
    close(client_socket);
    close(server_socket);

    return 0;
}
