#include "header.h"
/*
 * @description
 * Write your server's send function here.
 * 
 * 1. Send cwnd sequences of data start with correct sequence number.
 * 2. Simulate packet loss.
 * 
 * @param
 * You can increase or decrease parameters by yourself.
 * Some useful information can be used either as global varaible or passed as parameters to functions:
 * 1. client_fd: The socket descriptor used for sending/receiving data  
 *               with a client that has connected to server.
 * 2. last_acked: Previous acknowledge packets.
 * 3. cwnd: congestion window size
*/
int last_acked = 1;
int cwnd = 1;
int ssthresh = 8;

void server_send(int client_fd) {
    if(ssthresh > cwnd){
        printf("State: slow start (cwnd = %d, ssthresh = %d)\n", cwnd, ssthresh);
    }
    else {
        printf("State: congestion avoidance (cwnd = %d, ssthresh = %d)\n", cwnd, ssthresh);
    }
    for (int i = 0; i < cwnd; i++) {
        Segment seg;
        seg.seq_num = last_acked + i;
        seg.loss = packet_loss();
        printf("Send: seq_num = %d\n", seg.seq_num);
        send(client_fd, &seg, sizeof(seg), 0);
    }
}

/*
 * @description
 * Write your server's receive function here.
 * ** We don't need to consider time-out in this lab. 
 *    loss will only occur when packet_loss() = true. **
 * 1. Receive ACKs from client.
 * 2. Detect if 3 duplicate ACK occurs.
 * 3. Update cwnd and ssthresh
 * 
 *
 * @param
 * You can increase or decrease parameters by yourself.
 * Some useful information can be used either as global varaible or passed as parameters to functions:
 * 1. client_fd: The socket descriptor used for sending/receiving data  
 *               with a client that has connected to server.
 * 2. last_acked: Previous acknowledge packets.
 * 3. ssthresh: slow start threshold
 * 4. cwnd: congestion window size
*/
void server_receive(int client_fd) {
    Segment ack;
    int dup_ack_count = 1;
    bool is_dup_ack = false;
    for(int i = 0; i < cwnd; i++) {
        recv(client_fd, &ack, sizeof(ack), 0);
        printf("ACK: ack_num = %d\n", ack.ack_num);
        if(ack.ack_num > last_acked) {
            last_acked = ack.ack_num;
            dup_ack_count = 1;
        } 
        else if(ack.ack_num == last_acked) {
            dup_ack_count++;
            if(dup_ack_count == 3) {
                printf("3 duplicate ACKs : ACK_num = %d, ssthresh = %d\n", ack.ack_num, ssthresh);
                is_dup_ack = true;
            }
        }
    }
    if(is_dup_ack) {
        ssthresh = cwnd / 2;
        cwnd = 1;
    }
    else {
        if(cwnd * 2 < ssthresh) {
            cwnd *= 2;
        }
        else if(cwnd < ssthresh) {
            cwnd = ssthresh;
        }
        else {
            cwnd++;
        }
    }
}

/*
 * @description
 * Task1
 * 1. Create a TCP socket bind to port 45525.
 * 2. Listen the TCP socket.
 * 3. Accept the connect and get the Client socket file descriptor
 * 4. Send 1 message "Hi I'm server {your_student_id}" to client
 * 5. Go finish the client.c TASK 1
 *
 * Task2:
 * Start data transmission with simulating congestion control.
 *
 * To be simple, we receive ACKs only after all cwnd sequences of data are sent, 
 * so we cannot react to 3-duplicate ACKs immediately. This is ok for this lab.
 * Ex: 
 *      while(ROUND--) {
 *          server_send();
 *          server_receive();
 *      }
*/
int main(int argc, char* argv[]) {
    // Create TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Set up server's address.
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(45525);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
    //Bind socket to the address.
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    //Listening the socket.
    listen(sockfd, 10);
    //Accept the connect request.
    int new_socket = accept(sockfd, NULL, NULL);
    // Send 1 message to client.
    char message[] = "Hi I'm server 111062109";
    send(new_socket, message, sizeof(message), 0);
    sleep(1);
    // Start congestion control
    int ROUND = 20;
    while (ROUND--) {
        server_send(new_socket);
        server_receive(new_socket);
    }
    // Close the socket.
    close(new_socket);
    return 0;
}