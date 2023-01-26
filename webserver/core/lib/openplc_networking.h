//client.cpp
int connect_to_tcp_server(uint8_t *ip_address, uint16_t port, int method);
int send_tcp_message(uint8_t *msg, size_t msg_size, int socket_id);
int receive_tcp_message(uint8_t *msg_buffer, size_t buffer_size, int socket_id);
int close_tcp_connection(int socket_id);
