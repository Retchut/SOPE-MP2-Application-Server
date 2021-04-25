/**
* @brief establishes the communication between the client and the server through FIFOs
* @param id task id
* @param load task load (1-9)
* @param fifoname name of the public FIFO created by the server
* @return 0 on success, 1 in file related error, 2 on reading  or writing error, 3 on closed server
*/
int fifo(int id, int load, char* fifoname);
