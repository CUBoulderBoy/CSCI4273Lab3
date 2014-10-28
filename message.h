/*
 * CSCI 4273/5273 Message Header File
 *
 * Written by: Christopher Jordan and Aaron Davis
 *
 * Adapted from Code Written by: Shiv Mishra
 *
 * Last updated: October 22, 2014
 */

#ifndef _MESSAGE_
#define _MESSAGE_

#include <list>
#include <string.h>

using namespace std;

class Message
{
public:
    Message( );
    Message(char *msg, size_t len);
    ~Message( );
    void msgAddHdr(char *hdr, size_t length);
    char *msgStripHdr(int len);
    int msgSplit(Message &secondMsg, size_t len);
    void msgJoin(Message &secondMsg);
    size_t msgLen( );
    void msgFlat(char *buffer);

private:
    size_t msglen;
    list<char *> msg_content;
};

#endif