#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

char webpage[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
    "<!DOCTYPE html>\r\n"
    "<html><head><title>TestServer</title></head>\r\n"
    "<body><center><h1>My Server</h1><br>\r\n"
    "<p>Enter /index.html to view index page</p>\r\n"
    "<p>Enter /index.jpeg to view Photo</p>\r\n"
    "<p>Enter /about.html to view about page</p>"
    "<p>Enter /audio.mp3 to play audio</p></center></body></html>";

int main(int argc, char *argv[])
{

    int server = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;

    if (server < 0)
    {
        perror("server");
        exit(1);
    }
    else
    {
        printf("Socked created\n");
    }

    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);
    server_address.sin_port = htons(8080);

    if (bind(server, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("bind");
        exit(1);
    }
    else
    {
        printf("Socked binded\n");
    }

    if (listen(server, 10) < 0)
    {
        perror("listen");
        exit(1);
    }
    else
    {
        printf("Server listning\n");
    }

    int client;
    struct sockaddr_in client_address;
    socklen_t sin_len = sizeof(client_address);
    int buf = 2048;
    char *buffer = malloc(buf);
    int file;

    char imageheader[] =
        "HTTP/1.1 200 Ok\r\n"
        "Content-Type: image/jpeg\r\n\r\n";

    char http_header[2048] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n\r\n";

    char mp3_header[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: audio/mpeg; charset=UTF-8\r\n\r\n";


    while (1)
    {
        client = accept(server, (struct sockaddr *)&client_address, &sin_len);

        if (client == -1)
        {
            perror("connection failed");
        }

        printf("Got client connection\n");

        memset(buffer, 0, 2048);
        read(client, buffer, 2047);
        printf("%s\n", buffer);

        if(!strncmp(buffer,"GET /favicon.ico",13))
        {
            write(client, imageheader, sizeof(imageheader) - 1);
            file = open("favicon.ico", O_RDONLY);
            int sent = sendfile(client, file, NULL, 400);
            printf("sent: %d", sent);
            close(file);
            close(client);
        }

        if(!strncmp(buffer,"GET /index.jpeg",13))
        {
            write(client, imageheader, sizeof(imageheader) - 1);
            file = open("index.jpeg", O_RDONLY);
            int sent = sendfile(client, file, NULL, 7000);
            printf("sent: %d", sent);
            close(file);
            close(client);
        }

        if(!strncmp(buffer,"GET /index.html",13))
        {
            write(client, http_header, sizeof(http_header) - 1);
            file = open("index.html", O_RDONLY);
            int sent = sendfile(client, file, NULL, 200);
            printf("sent: %d", sent);
            close(file);
            close(client);
        }

        if(!strncmp(buffer,"GET /about.html",13))
        {
            write(client, http_header, sizeof(http_header) - 1);
            file = open("about.html", O_RDONLY);
            int sent = sendfile(client, file, NULL, 200);
            printf("sent: %d", sent);
            close(file);
            close(client);
        }

        if(!strncmp(buffer,"GET /audio.mp3",13))
        {
            write(client, mp3_header, sizeof(mp3_header) - 1);
            file = open("audio.mp3", O_RDONLY);
            int sent = sendfile(client, file, NULL, 800000);
            printf("sent: %d", sent);
            close(file);
            close(client);
        }


        write(client, webpage, sizeof(webpage) - 1);
        close(client);
    }
    return 0;
}
