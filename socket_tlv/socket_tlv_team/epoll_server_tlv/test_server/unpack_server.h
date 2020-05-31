#ifndef UNPACK_SERVER_H
#define UNPACK_SERVER_H
#include <sqlite3.h>
#define MAGIC_CRC 0x1E50
int unpack_server(unsigned char* unpack_buf,sqlite3* sqt_db;);
unsigned short  crc_check(unsigned short magic_crc,unsigned char *client_buf);
#endif
