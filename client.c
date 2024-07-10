#include "header.h"

/*
 * @description
 * Task1
 * 1. create a socket and connect to server.
 *    (server's IP address = "127.0.0.1")
 *    (server's Port number = 45525)
 * 2. Receive 1 message from server and print it out.
 *    (The message you sent from server)
 * 
 * Task2
 * Continuously receive data from Server and send back ACK.
*/


void client_receive(int socket_fd) {
    int ROUND = 10;
    bool recv_ack[1024] = {0};
    int ack_num = 1;
    while (1) {
        Segment seg;
        recv(socket_fd, &seg, sizeof(seg), 0);
        if (seg.loss) {
            printf("Loss: seq_num = %d\n", seg.seq_num);
            Segment ack;
            ack.ack_num = ack_num;
            ack.seq_num = seg.ack_num;
            send(socket_fd, &ack, sizeof(ack), 0);
            continue;
        }
        printf("Received: seq_num = %d\n", seg.seq_num);
        recv_ack[seg.seq_num] = true;
        if (seg.seq_num == ack_num) {
            for(int i = ack_num + 1; i < 1024; i++) {
                ack_num = i;
                if (!recv_ack[i]) break;
            }
            Segment ack;
            ack.ack_num = ack_num;
            //ack.seq_num = seg.seq_num;
            ack.loss = false;
            send(socket_fd, &ack, sizeof(ack), 0);
        }
        else {
            Segment ack;
            ack.ack_num = ack_num;
            ack.seq_num = seg.ack_num;
            send(socket_fd, &ack, sizeof(ack), 0);
        }
    }
    close(socket_fd);
}


int main(int argc , char *argv[])
{   
    //Create socket.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //Set up server's address.
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(45525);
    //Connect to server's socket.
    connect(socket_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    // Receive 1 message and print it out.
    char buffer[1024];
    recv(socket_fd, buffer, 1024, 0);
    printf("%s\n", buffer);
    // Receive data and send ACK.
    client_receive(socket_fd);
    return 0;
}