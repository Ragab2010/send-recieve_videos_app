#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Connect to the server on the loopback address and a specific port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(12345);  // Connect to the same port as the server
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Use the loopback address

    // Connect to the server
    connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));

    // Open a file for writing
    std::ofstream received_video("received_video.mp4", std::ios::binary);
    if (!received_video) {
        std::cerr << "Failed to open received video file." << std::endl;
        return 1;
    }

    char buffer[1024];
    int bytes_received;

    while (true) {
        // Send a synchronization signal to the server
        send(client_socket, "OK", 2, 0);

        // Receive video data from the server
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break;
        }

        // Write the received data to the video file
        received_video.write(buffer, bytes_received);
    }

    // Close the socket when done
    close(client_socket);

    return 0;
}
