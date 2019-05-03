//-----------------------------------------------------------------------------
// Copyright 2015 Thiago Alves
//
// Based on the LDmicro software by Jonathan Westhues
// This file is part of the OpenPLC Software Stack.
//
// OpenPLC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenPLC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenPLC.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// This file is the hardware layer for the OpenPLC. If you change the platform
// where it is running, you may only need to change this file. All the I/O
// related stuff is here. Basically it provides functions to read and write
// to the OpenPLC internal buffers in order to update I/O state.
// Thiago Alves, Dec 2015
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <string>
#include <vector>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <jsoncpp/json/json.h>

#include "ladder.h"
#include "custom_layer.h"

#define MAX_SIZE 1024

unsigned char log_msg[1024];

// Structure for message queue
typedef struct msg_buffer {
    long msg_type;
    char msg_text[MAX_SIZE];
} Message;
static Message inputMessage, outputMessage;

// I/O queue
static int input_qid, output_qid;
static int sequence_number = 0;

// Flag to keep track of queue status
static std::vector<IEC_BOOL *> output_request;

// JSON
static Json::Reader jsonReader;
static Json::Value jsonData;

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
    key_t input_q_key, output_q_key;

    // ftok to generate unique key and returns identifier
    //input_q_key = ftok("input_q", 65);

    // msgget creates a message queue and returns identifier
    //input_qid = msgget(input_q_key, 0666 | IPC_CREAT);
    input_qid = msgget(317245633, 0666 | IPC_CREAT);

    // ftok to generate unique key and returns identifier
    //output_q_key = ftok("output_q", 65);

    // msgget creates a message queue and returns identifier
    //output_qid = msgget(output_q_key, 0666 | IPC_CREAT);
    output_qid = msgget(1982670785, 0666 | IPC_CREAT);

    memset(&inputMessage, 0, sizeof(inputMessage));
    memset(&outputMessage, 0, sizeof(outputMessage));
}

static bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Input state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
uint8_t updateBuffersIn()
{
    uint8_t shouldExecute = 1;

    pthread_mutex_lock(&bufferLock); //lock mutex

    /*********READING AND WRITING TO I/O**************

    *bool_input[0][0] = read_digital_input(0);
    write_digital_output(0, *bool_output[0][0]);

    *int_input[0] = read_analog_input(0);
    write_analog_output(0, *int_output[0]);

    **************************************************/
    
    // msgrcv to receive message
    int ret = msgrcv(input_qid, &inputMessage, sizeof(inputMessage), 1, IPC_NOWAIT);
    if (ret > 0) {
        // Display the message
        sprintf(log_msg, "Data received: %s\n", inputMessage.msg_text);
        log(log_msg);
    
        jsonReader.parse(inputMessage.msg_text, jsonData);
        if (jsonData.isMember("sequence") && isNumber(jsonData["sequence"].asString())) {
            sequence_number = jsonData["sequence"].asInt();
            sprintf(log_msg, "Sequence number: %d\n", sequence_number);
            log(log_msg);
        }
        if (!jsonData.isMember("input")) {
            log("JSON format error! No input was specified!\n");
            shouldExecute = 0;
        }
        else {
            if (!jsonData.isMember("output")) {
                log("JSON format error! No output was specified!\n");
                shouldExecute = 0;
            }
            else {
                // Parse the input settings
                for (Json::Value::iterator inputRowIt = jsonData["input"].begin();
                        inputRowIt != jsonData["input"].end();
                        ++inputRowIt) {
                    std::string inputRow = inputRowIt.key().asString();
                    if (!isNumber(inputRow)) {
                        sprintf(log_msg, "Parsing error! Input row is not a number: %s\n", inputRow);
                        log(log_msg);
                        shouldExecute = 0;
                        break;
                    }
                    for (Json::Value::iterator inputColumnIt = jsonData["input"][inputRow].begin();
                            inputColumnIt != jsonData["input"][inputRow].end();
                            ++inputColumnIt) {
                        std::string inputColumn = inputColumnIt.key().asString();
                        if (!isNumber(inputColumn)) {
                            sprintf(log_msg, "Parsing error! Input column is not a number: %s\n", inputColumn);
                            log(log_msg);
                            shouldExecute = 0;
                            break;
                        }
                        std::string inputValue = (*inputColumnIt).asString();
                        if (!isNumber(inputColumn)) {
                            sprintf(log_msg, "Parsing error! Input value is not a number: %s\n", inputValue);
                            log(log_msg);
                            shouldExecute = 0;
                            break;
                        }
                        // Set the digital I/O
                        if (std::stoi(inputValue)) {
                            *bool_input[std::stoi(inputRow)][std::stoi(inputColumn)] = 1;
                            sprintf(log_msg, "Setting bool_input[%d][%d] to True\n", std::stoi(inputRow), std::stoi(inputColumn));
                            log(log_msg);
                        }
                        else {
                            *bool_input[std::stoi(inputRow)][std::stoi(inputColumn)] = 0;
                            sprintf(log_msg, "Setting bool_input[%d][%d] to False\n", std::stoi(inputRow), std::stoi(inputColumn));
                            log(log_msg);
                        }
                    }
                }
                if (shouldExecute) {
                    output_request.clear();
                    // Parse the output settings
                    for (Json::Value::iterator outputRowIt = jsonData["output"].begin();
                            outputRowIt != jsonData["output"].end();
                            ++outputRowIt) {
                        std::string outputRow = outputRowIt.key().asString();
                        if (!isNumber(outputRow)) {
                            sprintf(log_msg, "Parsing error! Output row is not a number: %s\n", outputRow);
                            log(log_msg);
                            shouldExecute = 0;
                            break;
                        }
                        for (Json::Value::iterator outputColumnIt = jsonData["output"][outputRow].begin();
                                outputColumnIt != jsonData["output"][outputRow].end();
                                ++outputColumnIt) {
                            std::string outputColumn = outputColumnIt.key().asString();
                            if (!isNumber(outputColumn)) {
                                sprintf(log_msg, "Parsing error! Output column is not a number: %s\n", outputColumn);
                                log(log_msg);
                                shouldExecute = 0;
                                break;
                            }
                            // Add the output rows and columns to the vector
                            output_request.push_back(bool_output[std::stoi(outputRow)][std::stoi(outputColumn)]);
                        }
                    }
                }
            }
        }
    }
    else {
        shouldExecute = 0;
        output_request.clear();
        sequence_number = 0;
    }

    pthread_mutex_unlock(&bufferLock); //unlock mutex
    return shouldExecute;
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Output state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
	pthread_mutex_lock(&bufferLock); //lock mutex

	/*********READING AND WRITING TO I/O**************

	*bool_input[0][0] = read_digital_input(0);
	write_digital_output(0, *bool_output[0][0]);

	*int_input[0] = read_analog_input(0);
	write_analog_output(0, *int_output[0]);

	**************************************************/

        if (!output_request.empty()) {
            int index = 0;
            for(std::vector<IEC_BOOL *>::iterator it = output_request.begin(); it != output_request.end(); ++it) {
                if (**it) {
                    outputMessage.msg_text[index] = '1';
                }
                else {
                    outputMessage.msg_text[index] = '0';
                }
                ++index;
            }
            output_request.clear();
            outputMessage.msg_text[index] = '\0';
            outputMessage.msg_type = 1;
            msgsnd(output_qid, &outputMessage, sizeof(outputMessage), 0);
            sprintf(log_msg, "Sent output %s for sequence number: %d\n", outputMessage.msg_text, sequence_number);
            log(log_msg);
        }

	pthread_mutex_unlock(&bufferLock); //unlock mutex
}

