#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <openssl/evp.h>

#define MSG_SZE 100
#define CIPHER_TXT_LEN 128

void encrypt(char* msg, char* cipher_txt, int* cipher_txt_len){

    char key[32] = "0123456789qwertyuiopasdfghjklzxc";
    char iv[16] = "0123456789abcdef";
    

    EVP_CIPHER_CTX *ctx;
ctx = EVP_CIPHER_CTX_new();
if (!ctx){
    perror("Error creating cipher context\n");
    exit(EXIT_FAILURE);
}

if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1){

    perror("Error initalizing the cipher\n");
    exit(EXIT_FAILURE);
}

if(EVP_EncryptUpdate(ctx, cipher_txt, cipher_txt_len, msg, (int)strlen(msg) ) != 1){

    perror("Error encrypting");
    exit(EXIT_FAILURE);
}

if (EVP_EncryptFinal_ex(ctx, cipher_txt, cipher_txt_len) != 1){

    perror("Error encrypting");
    exit(EXIT_FAILURE);
}

}

int communicate(char* msg){

    char cipher_txt[CIPHER_TXT_LEN];
    int cipher_txt_len = CIPHER_TXT_LEN;
    printf("%s", msg);

    int sockfd;
    //create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {

        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //connect to remote server with socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); //convert IP to binary format

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    encrypt(msg, cipher_txt, &cipher_txt_len);
    printf("Encrypted message %s with size: %d,  as %s with encrypted size %d\n", msg, (int)strlen(msg), cipher_txt, cipher_txt_len);
    //send message to remote server

    int bytes_sent = 0;

    while(bytes_sent < sizeof(cipher_txt_len)){
        printf("Sending: %d\n", cipher_txt_len);
        bytes_sent += send(sockfd, &cipher_txt_len+bytes_sent, cipher_txt_len-bytes_sent, 0);
        printf("sent: %d bytes of %d bytes sent\n", bytes_sent, cipher_txt_len);
    }

    
    bytes_sent = 0;
    while(bytes_sent < cipher_txt_len){
        
        printf("Sending: %s\n", cipher_txt+bytes_sent);
        
        bytes_sent+=send(sockfd, cipher_txt+bytes_sent, cipher_txt_len-bytes_sent, 0);
        printf("sent: %d bytes of %d bytes sent\n", bytes_sent, cipher_txt_len);

    }

    
    recv(sockfd, msg, MSG_SZE, 0);
   

    printf("New received message: %s\n", msg);


    return 0;
}

int main(char ** argv, int argc){

char input[100];
printf("Enter string\n");
fgets(input, sizeof(input), stdin);


communicate(input);


    return 0;
}