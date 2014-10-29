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
#include <iostream>

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
    pair<char*,size_t> msg_pair (msg, len);
    msg_content.push_front(msg_pair);
}

Message::~Message( )
{
    msg_content.clear();
}

void Message::msgAddHdr(char *hdr, size_t length)
{
    msglen += length;
    pair<char*,size_t> hdr_pair (hdr, length);
    msg_content.push_front(hdr_pair);
}

char *Message::msgStripHdr(int len)
{
    int n_len = 0;
    int t_len = len;
    char *new_msg_content;
    char *stripped_content;

    // If the len is too long, then return null
    if ((msglen < len) || (len == 0)){
        return NULL;
    }

    // If the len is the size of the full first buffer, then just return it
    if ((int)msg_content.front().second == len){
        // Save pointer to buffer
        stripped_content = msg_content.front().first;

        // Remove the front node with the buffer from the list
        msg_content.pop_front();

        // Adjust the length of the message
        msglen -= len;

        // Return the buffer to the calling function
        return stripped_content;
    }

    // Otherwise build appropariate message to return
    else{
        // Create buffer to store return
        stripped_content = new char[len];
        
        // Loop until header return has been built
        for(int i = 0; i < len;){
            // Initialize the length of the front node
            n_len = (int)msg_content.front().second;

            // Initialize the remaining lenth needs
            t_len = len - i;

            // If the node perfectly is perfectly sized or not large enough just add it all
            if (n_len <= t_len){
                // Copy the contents into the buffer
                memcpy(stripped_content+i, msg_content.front().first, n_len);

                // Remove the pointer from the list
                msg_content.pop_front();

                // Adjust i
                i += n_len;
            }

            // If the length of the node is larger than the desired length
            else{
                // Set pointer to return the old buffer
                memcpy(stripped_content+i, msg_content.front().first, t_len);

                // Save the old buffer pointer and adjust starting position
                msg_content.front().first += t_len;
                msg_content.front().second -= t_len;

                // Adjust i
                i += t_len;
            }  
        }
        // Update message length
        msglen -= len;

        // Return to calling function
        return stripped_content;
    }
}

int Message::msgSplit(Message &secondMsg, size_t len)
{
    list < pair<char*, size_t> >content;
    int t_len = 0;

    // If the len is less than 0, or the split length is less the
    // length of the message then error out
    if ((len < 0) || (len > msglen)){
        return 0;
    }

    // Loop through and build new messages
    for (int i = 0; i < (int)len;){
        // Reset remaining length tracker
        t_len = len - i;

        // For testing
        //cout << "Amount remaining: " << t_len << endl;
        //cout << "Begin i value: " << i << endl;

        // If the node perfectly fits, or is not enough, add it all
        if ((int)msg_content.front().second <= t_len){
            // Push the node to the new list
            pair<char*, size_t> move (msg_content.front().first, msg_content.front().second);
            content.push_back(move);

            // Remove the node from the old list
            msg_content.pop_front();

            // Increment i
            i += (int)content.back().second;

            // For testing
            //cout << "End i value: " << i << endl;
        }

        // If the node is too large, split into two new nodes
        else{
            // Create new node buffer
            char * one_node = new char[t_len];

            // Copy into buffer
            memcpy(one_node, msg_content.front().first, t_len);

            // Create new pair
            pair<char*, size_t> two_node (one_node, t_len);

            // Push to list
            content.push_back(two_node);

            // Update pointer on old list
            msg_content.front().first += t_len;
            msg_content.front().second -= t_len;

            // Increment i
            i += t_len;
        }
    }
    // Set second message
    secondMsg.msglen = msglen - len;
    secondMsg.msg_content = msg_content;

    // For testing
    //cout << "Second message length: " << secondMsg.msglen << endl;

    // Set first message
    msg_content = content;
    msglen = len;

    return 1;
}

void Message::msgJoin(Message &secondMsg)
{
    // Copy second message content to pointer
    list< pair<char*, size_t> >content = secondMsg.msg_content;

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
    list <pair<char*, size_t> >to_buff = msg_content;
    char * c_node;
    int c_len;

    // Loop and add to buffer
    for (int i = 0; i < msglen;){
        c_node = to_buff.front().first;
        c_len = (int)to_buff.front().second;

        // For testing
        //cout << "Current Node: " << c_node  << endl;
        //cout << "Current Node Length: " << c_len << endl;

        //Assume that sufficient memory has been allocated in buffer
        memcpy(buffer+i, c_node, c_len);

        // Increment i by node size
        i += c_len;

        // Remove front node
        to_buff.pop_front();

        // For testing
        //cout << "Curren Buffer: " << buffer << endl;
    }
}