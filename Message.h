/*
 * CSCI 4273/5273 Message Header File
 *
 * Written by: Christopher Jordan and Aaron Davis
 *
 * Adapted from Code Written by: Shiv Mishra
 *
 * Last updated: October 22, 2014
 */

#include <list>

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
    list <char*> msg_content;
};

Message::Message()
{
    msglen = 0;
    msg_content = NULL;
}

Message::Message(char *msg, size_t len)
{
    msglen = len;
    msg_content.push_front(msg);
}

Message::~Message( )
{
    msg_content.clear();
}

void Message::msgAddHdr(char *hdr, size_t length)
{
    msglen += length;
    msg_content.push_front(hdr);
}

char *Message::msgStripHdr(int len)
{
    int n_len = 0;
    int r_len = 0;
    int t_len = len;
    char *new_msg_content;
    char *stripped_content;

    // If the len is too long, then return null
    if ((msglen < len) || (len == 0)){
        return NULL;
    }

    // If the len is the size of the full first buffer, then just return it
    if ((int)sizeof(msg_content.front()) == len){
        // Save pointer to buffer
        stripped_content = msg_content.front();
        
        // Remove the front node with the buffer from the list
        msg_content.pop_front();
        
        // Adjust the length of the message
        msglen -= eln;

        // Return the buffer to the calling function
        return stripped_content;
    }

    // Otherwise build appropariate message to return
    else{
        // Initialize the length of the front node
        n_len = (int)sizeof(msg_content.front());

        // If the length of the node is larger than the desired length
        if( n_len > len ){
            // Set pointer to return the old buffer
            stripped_content = msg_content.front();
            
            // Save the old buffer pointer and adjust starting position
            new_msg_content = msg_content.front();
            msg_content.pop_front();
            new_msg_content += len;
            
            // Push updated buffer pointer to the list
            msg_content.push_front(new_msg_content);
        }

        // If the first node is not larger enough to fill the reply buffer
        else if ( n_len < len){
            // Create buffer to store return
            stripped_content = new char[len];

            // Loop through until the length has been stripped
            for(r_len = 0; r_len < len;){
                // Initialize the length of the front node
                n_len = (int)sizeof(msg_content.front());

                // Reset t_len variable (amount of len remaining)
                t_len = len - r_len;

                // If the node perfectly is perfectly sized or not large enough just add it all
                if (n_len <= t_len){
                    // Make pointer set to the first point in the list
                    new_msg_content = msg_content.front();

                    // Copy the contents into the buffer
                    memcpy(stripped_content+r_len, new_msg_content, n_len);

                    // Remove the pointer from the list
                    msg_content.pop_front();

                    // Update r_len (length of return buffer)
                    r_len += n_len;
                }

                // If the node doesn't over-flow, but has extras update pointer
                else if (n_len > t_len){
                    // Make pointer set to the first point in the list
                    new_msg_content = msg_content.front();

                    // Copy the contents into the buffer
                    memcpy(stripped_content+r_len, new_msg_content, t_len);

                    // Remove pointer, update and push back to list
                    msg_content.pop_front();
                    new_msg_content += t_len;
                    msg_content.push_front(new_msg_content);

                    // Update r_len (length of return buffer)
                    r_len += t_len;
                }
            }

            // Update message length
            msglen -= len;

            // Return to calling function
            return stripped_content;
        }
    }
}

int Message::msgSplit(Message &secondMsg, size_t len)
{
    char *content = msg_content;
    size_t length = msglen;

    if ((len < 0) || (len > msglen)) return 0;

    msg_content = new char[len];
    msglen = len;
    memcpy(msg_content, content, len);
    secondMsg.msglen = length - len;
    secondMsg.msg_content = new char[secondMsg.msglen];
    memcpy(secondMsg.msg_content, content + len, secondMsg.msglen);
    delete content;
    return 1;
}

void Message::msgJoin(Message &secondMsg)
{
    char *content = msg_content;
    size_t length = msglen;

    msg_content = new char[msglen + secondMsg.msglen];
    msglen += secondMsg.msglen;
    memcpy(msg_content, content, length);
    memcpy(msg_content + length, secondMsg.msg_content, secondMsg.msglen);
    delete content;
    delete secondMsg.msg_content;
    secondMsg.msg_content = NULL;
    secondMsg.msglen = 0;
}

size_t Message::msgLen( )
{
    return msglen;
}

void Message::msgFlat(char *buffer)
{
    //Assume that sufficient memory has been allocated in buffer
    memcpy(buffer, msg_content, msglen);
}

