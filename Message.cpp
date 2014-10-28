/*
 * CSCI 4273/5273 Message CPP File
 *
 * Written by: Christopher Jordan and Aaron Davis
 *
 * Adapted from Code Written by: Shiv Mishra
 *
 * Last updated: October 22, 2014
 */

#include "message.h"
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string.h>

using namespace std;

Message::Message()
{
    msglen = 0;
    msg_content.clear();
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
        msglen -= len;

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
    list <char *> content;
    int t_len = 0;

    // If the len is less than 0, or the split length is less the
    // length of the message then error out
    if ((len < 0) || (len > msglen)){
        return 0;
    }

    // Loop through and build new messages
    for (int i = 0; i < len;){
        // Reset remaining length tracker
        t_len = len - i;

        // If the node perfectly fits, or is not enough, add it all
        if (sizeof(msg_content.front()) <= t_len){
            // Push the node to the new list
            content.push_front(msg_content.front());

            // Remove the node from the old list
            msg_content.pop_front();

            // Increment i
            i += t_len;
        }

        // If the node is too large, split into two new nodes
        if (sizeof(msg_content.front()) > t_len){
            // Create new node buffers
            char * one_node = new char[t_len];
            char * two_node = msg_content.front();

            // Copy into buffer
            memcpy(one_node, two_node, t_len);

            // Push to list
            content.push_front(one_node);

            // Update pointer on old list
            two_node += t_len;
            msg_content.pop_front();
            msg_content.push_front(two_node);

            // Increment i
            i += t_len;
        }
    }
    // Set second message
    secondMsg.msglen = msglen - len;
    secondMsg.msg_content = msg_content;

    // Set first message
    msg_content = content;
    msglen = len;

    return 1;
}

void Message::msgJoin(Message &secondMsg)
{
    // Copy second message content to pointer
    list<char *> content = secondMsg.msg_content;

    while (!content.empty()){
        msg_content.push_back(content.front());
        content.pop_front();
    }

    // Set local length
    msglen += secondMsg.msglen;

    // Fix second message length and contents
    secondMsg.msg_content.clear();
    secondMsg.msglen = 0;
}

size_t Message::msgLen( )
{
    return msglen;
}

void Message::msgFlat(char *buffer)
{
    // Variable to store current node
    list <char *> to_buff = msg_content;
    char * c_node;
    int c_len;

    // Loop and add to buffer
    for (int i = 0; i < msglen;){
        // Set current node and length
        c_node = to_buff.front();
        c_len = sizeof(c_node);

        //Assume that sufficient memory has been allocated in buffer
        memcpy(buffer+i, c_node, c_len);
    }
}