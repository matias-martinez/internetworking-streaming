
int passiveTCPSocket(int port);
int connectTCP();
int receiveall ( int sd, char * buffer, int total );
int sendall ( int sd, char *buf, int len);
int pack();
int unpack();