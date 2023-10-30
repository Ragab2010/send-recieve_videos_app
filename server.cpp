#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ncurses.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <video_path>" << std::endl;
        return 1;
    }

    // Open and read the video file
    std::ifstream video_file(argv[1], std::ios::binary);
    if (!video_file) {
        std::cerr << "Failed to open video file: " << argv[1] << std::endl;
        return 1;
    }

    // Determine the total file size
    video_file.seekg(0, std::ios::end);
    int total_size = video_file.tellg();
    video_file.seekg(0, std::ios::beg);

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    WINDOW *progress_win = newwin(3, 50, 1, 1);
    box(progress_win, 0, 0);
    refresh();
    wrefresh(progress_win);

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

    char buffer[1024];
    int bytes_read;
    int total_sent = 0;

    while (!video_file.eof()) {
        // Wait for a synchronization signal from the client
        recv(client_socket, buffer, sizeof(buffer), 0);

        video_file.read(buffer, sizeof(buffer));
        bytes_read = video_file.gcount();

        // Send video data to the client
        send(client_socket, buffer, bytes_read, 0);
        
        total_sent += bytes_read;
        
        // Display progress
        int percent = (total_sent * 100) / total_size;
        wclear(progress_win);
        box(progress_win, 0, 0);
        mvwprintw(progress_win, 1, 1, "Progress: %d%%", percent);
        wrefresh(progress_win);
    }

    // Close the sockets and ncurses when done
    close(client_socket);
    close(server_socket);
    delwin(progress_win);
    endwin();

    return 0;
}
