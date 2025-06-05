#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <openssl/evp.h>

void decrypt(char* plain_txt, char* cipher_txt){

    char key[32] = "0123456789qwertyuiopasdfghjklzxc";
    char iv[16] = "0123456789abcdef";

    int plain_txt_len = 100;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx){
    perror("Error creating cipher context\n");
    exit(EXIT_FAILURE);
}
    
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    EVP_DecryptUpdate(ctx, plain_txt, &plain_txt_len, cipher_txt, 100);

    EVP_DecryptFinal_ex(ctx, plain_txt, &plain_txt_len);


}


int run_server(){

char buffer [1024];
char buffer_decrypted[1024];

int sockfd;
sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd == -1) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
}


struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP
server_addr.sin_port = htons(7777); // Port number

if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    perror("Bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
}

if (listen(sockfd, 5) == -1) { // Allow up to 5 pending connections
    perror("Listen failed");
    close(sockfd);
    exit(EXIT_FAILURE);
}
       

int client_sock;
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);

while(1) {
client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
if (client_sock == -1) {
    perror("Accept failed");
    close(sockfd);
    exit(EXIT_FAILURE);
}

int msg_len = 0;

recv(client_sock,&msg_len, sizeof(msg_len), 0);
msg_len=ntohl(msg_len); // Convert from network byte order to host byte order
printf("Message length received: %d\n", msg_len);
memset(buffer, 0, sizeof(buffer));
recv(client_sock, buffer, sizeof(buffer), 0);   
decrypt(buffer_decrypted, buffer);

char decrypt_msg [100];
strncpy(decrypt_msg, buffer_decrypted, 100);
printf("Here is the received string: %s of size %d - decrypted as: %s\n", buffer, (int)strlen(buffer), buffer_decrypted);


    strcat(buffer_decrypted, " String received, returning from client!");


int bytes_sent = 0;
while (bytes_sent < sizeof(buffer_decrypted)){
bytes_sent+=send(client_sock, buffer_decrypted, sizeof(buffer_decrypted), 0) < sizeof(buffer_decrypted);

}

}

    return 0;
}

int main() {

run_server();

    return 0;
}