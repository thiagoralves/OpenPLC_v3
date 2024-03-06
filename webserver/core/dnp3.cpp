//2017 Trevor Aron
//
//File contains code for DNP3
//
#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/PrintingSOEHandler.h>
#include <asiodnp3/PrintingChannelListener.h>
#include <asiodnp3/ConsoleLogger.h>
#include <asiodnp3/UpdateBuilder.h>

#include <asiopal/UTCTimeSource.h>
#include <opendnp3/outstation/SimpleCommandHandler.h>

#include <opendnp3/LogLevels.h>
//#include <opendnp3/outstation/Database.h>

#include <string>
#include <thread>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <fstream>

#include "ladder.h"

//some modbus defines
#define MAX_DISCRETE_INPUT      8192
#define MAX_COILS               8192
#define MAX_HOLD_REGS 			8192
#define MAX_INP_REGS			1024

#define MIN_16B_RANGE			1024
#define MAX_16B_RANGE			2047
#define MIN_32B_RANGE			2048
#define MAX_32B_RANGE			4095
#define MIN_64B_RANGE			4096
#define MAX_64B_RANGE			8191

#define OPLC_CYCLE              50000000

// Initial offset parameters (yurgen1975)
int offset_di = 0;
int offset_do = 0;
int offset_ai = 0;
int offset_ao = 0;

using namespace std;
using namespace opendnp3;
using namespace openpal;
using namespace asiopal;
using namespace asiodnp3;


IEC_BOOL dnp3_discrete_input[MAX_DISCRETE_INPUT];
IEC_BOOL dnp3_coils[MAX_COILS];
IEC_UINT dnp3_input_regs[MAX_INP_REGS];
IEC_UINT dnp3_holding_regs[MAX_HOLD_REGS];


// trim string from left
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim string from right
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim string from left and right
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}


//-----------------------------------------------------------------------------
// Class to handle commands from the master
//-----------------------------------------------------------------------------
class CommandCallback: public ICommandHandler {
public:
   
    //CROB - changed to support offsets (yurgen1975)
    virtual CommandStatus Select(const ControlRelayOutputBlock& command, uint16_t index) {
        index = index + offset_di;
        return CommandStatus::SUCCESS;
    }
    virtual CommandStatus Operate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType) {
        index = index + offset_di;
        auto code = command.functionCode;
        CommandStatus return_val;
            
           
        if(code == ControlCode::LATCH_ON || code == ControlCode::LATCH_OFF) {
            return_val = CommandStatus::SUCCESS;

            IEC_BOOL crob_val = (code == ControlCode::LATCH_ON);
                           
            pthread_mutex_lock(&bufferLock);
            if(bool_output[index/8][index%8] != NULL) {
                *bool_output[index/8][index%8] = crob_val;
            }
            pthread_mutex_unlock(&bufferLock);
        }
        else {
            return_val = CommandStatus::NOT_SUPPORTED;
        }
        return return_val;
    }

    //Analog Out - changed to support offsets (yurgen1975)
    virtual CommandStatus Select(const AnalogOutputInt16& command, uint16_t index) {
        index = index + offset_ao;
        return CommandStatus::SUCCESS;
    }
    virtual CommandStatus Operate(const AnalogOutputInt16& command, uint16_t index, OperateType opType) {
        index = index + offset_ao;
        auto ao_val = command.value;
        pthread_mutex_lock(&bufferLock);
        if(index < MIN_16B_RANGE && int_output[index] != NULL) {
            *int_output[index] = ao_val;
        }
        else if(index < MAX_16B_RANGE && 
                int_memory[index - MIN_16B_RANGE] != NULL) {
            *int_memory[index - MIN_16B_RANGE] = ao_val;
        }
        else if(index > MAX_16B_RANGE) {
            return CommandStatus::OUT_OF_RANGE;
        }
        pthread_mutex_unlock(&bufferLock);
        return CommandStatus::SUCCESS;
    }

    //AnalogOut 32 (Int)
    virtual CommandStatus Select(const AnalogOutputInt32& command, uint16_t index) {
        return CommandStatus::SUCCESS;
    }
    virtual CommandStatus Operate(const AnalogOutputInt32& command, uint16_t index, OperateType opType) {
        auto ao_val = command.value;

        if(index < MIN_32B_RANGE || index >= MAX_32B_RANGE)
            return CommandStatus::OUT_OF_RANGE;
        
        pthread_mutex_lock(&bufferLock);
        if(dint_memory[index - MIN_32B_RANGE] != NULL) {
            *dint_memory[index - MIN_32B_RANGE] = ao_val;
        }
        pthread_mutex_unlock(&bufferLock);

        return CommandStatus::SUCCESS;
    }

    //AnalogOut 32 (Float)
    virtual CommandStatus Select(const AnalogOutputFloat32& command, uint16_t index) {

        return CommandStatus::SUCCESS;
    }
    virtual CommandStatus Operate(const AnalogOutputFloat32& command, uint16_t index, OperateType opType) {
        auto ao_val = command.value;

        if(index < MIN_32B_RANGE || index >= MAX_32B_RANGE)
            return CommandStatus::OUT_OF_RANGE;
        
        pthread_mutex_lock(&bufferLock);
        if(dint_memory[index - MIN_32B_RANGE] != NULL) {
            *dint_memory[index - MIN_32B_RANGE] = ao_val;
        }
        pthread_mutex_unlock(&bufferLock);

        return CommandStatus::SUCCESS;
    }

    //AnalogOut 64
    virtual CommandStatus Select(const AnalogOutputDouble64& command, uint16_t index) {
        return CommandStatus::SUCCESS;
    }
    virtual CommandStatus Operate(const AnalogOutputDouble64& command, uint16_t index, OperateType opType) {
        auto ao_val = command.value;

        if(index < MIN_64B_RANGE || index >= MAX_64B_RANGE)
            return CommandStatus::OUT_OF_RANGE;
        
        pthread_mutex_lock(&bufferLock);
        if(lint_memory[index - MIN_64B_RANGE] != NULL) {
            *lint_memory[index - MIN_64B_RANGE] = ao_val;
        }
        pthread_mutex_unlock(&bufferLock);

        return CommandStatus::SUCCESS;
    }
protected:
    void Start() final {}
    void End() final {}
};

//------------------------------------------------------------------
// Function to update DNP3 values every time they may have changed
// Updated by Yurgen1975 to support slave devices: DI/DO address 800 and AI/AO address 100
//------------------------------------------------------------------
void update_vals(std::shared_ptr<IOutstation> outstation){
    UpdateBuilder builder;
    // Update Discrete input (Binary input) - changed to support offsets (yurgen1975)
    for(int i = offset_di; i < MAX_DISCRETE_INPUT; i++) {
        builder.Update(Binary((bool)(*bool_input[i/8][i%8])), i-offset_di);
    }

    // Update Coils (Binary Output) - changed to support offsets (yurgen1975)
    for(int i = offset_do; i < MAX_COILS; i++) {
        builder.Update(BinaryOutputStatus((bool)(*bool_output[i/8][i%8])), i-offset_do);
    }    

    // Update Input Registers (Analog Input) - changed to support offsets (yurgen1975)
    for (int i = offset_ai; i < MAX_INP_REGS; i++) {
        builder.Update(Analog((int)(*int_input[i])), i-offset_ai);
    }
    
    // Update Holding Registers (Analog Output) - changed to support offsets (yurgen1975)
    for (int i = offset_ao; i < MIN_16B_RANGE; i++) {
        builder.Update(AnalogOutputStatus((int)(*int_output[i])), i-offset_ao);
    }
    // Update Holding registers for memory
    for (int i = MIN_16B_RANGE; i < MAX_16B_RANGE; i++) {
        if(int_memory[i - MIN_16B_RANGE] != NULL)
            builder.Update(
                    AnalogOutputStatus((int)(*int_memory[i - MIN_16B_RANGE])),
                    i
            );
    } 
    // Update Holding registers for 32 b memory
    for (int i = MIN_32B_RANGE; i < MAX_32B_RANGE; i++) {
        if(dint_memory[i - MIN_32B_RANGE] != NULL)
            builder.Update(
                    AnalogOutputStatus((int)(*dint_memory[i - MIN_32B_RANGE])),
                    i
            );
    } 
    // Update Holding registers for 64 b memory
    for (int i = MIN_64B_RANGE; 
         (i < MAX_64B_RANGE && 
            i - MIN_64B_RANGE < sizeof(lint_memory) / sizeof(lint_memory[0])); 
         i++) {
        if(lint_memory[i - MIN_64B_RANGE] != NULL)
            builder.Update(
                    AnalogOutputStatus((int)(*lint_memory[i - MIN_64B_RANGE])),
                    i
            );
    } 
    outstation->Apply(builder.Build());
}

//----------------------------------------------------------------------
// Need to parse 'database_size' first
//----------------------------------------------------------------------
OutstationStackConfig create_config() {
    string line;
    ifstream cfgfile("dnp3.cfg");
    bool found = false;
    if(cfgfile.is_open()) {
        while (getline(cfgfile, line)) {
            if (line[0] == '#')
                continue;
            try {
                istringstream iss(line);
                string token;
                getline(iss, token, '=');
                token = trim(token);
                if (token == "database_size") {
                    getline(iss, token, '=');
                    return OutstationStackConfig(
                               DatabaseSizes::AllTypes(atoi(token.c_str()))
                           );
                }
                else 
                    continue;
            } catch(...) {
                cout << "Malformatted Line: " << line << endl;
                exit(1);
            } 

        }
    }
    return OutstationStackConfig(DatabaseSizes::AllTypes(10));
}

//----------------------------------------------------------------------
// parse dnp3.cfg and set dnp3 settings
//----------------------------------------------------------------------
OutstationStackConfig parseDNP3Config() {
    string line;
    ifstream cfgfile("dnp3.cfg");
    OutstationStackConfig config = create_config();
    if(cfgfile.is_open()) {
        while (getline(cfgfile, line)) {
            if (line[0] == '#')
                continue;
            try {
                istringstream iss(line);
                string token;
                getline(iss, token, '=');
                token = trim(token);
                if (token == "local_address") {
                    getline(iss, token, '=');     
                    config.link.LocalAddr = atoi(token.c_str());
                } else if (token == "remote_address") {
                    getline(iss, token, '=');     
                    config.link.RemoteAddr = atoi(token.c_str());
                } else if (token == "keep_alive_timeout") {
                    getline(iss, token, '=');     
                    if(token == "MAX") {
                        config.link.KeepAliveTimeout = 
                            openpal::TimeDuration::Max();
                    }
                    else {
                        config.link.KeepAliveTimeout = 
                            openpal::TimeDuration::Seconds(atoi(token.c_str()));
                    }
                } else if (token == "enable_unsolicited") {
                    getline(iss, token, '=');
                    if(token == "True")
                        config.outstation.params.allowUnsolicited = true;
                    else
                        config.outstation.params.allowUnsolicited = false;
                } else if (token == "select_timeout") {
                    getline(iss, token, '=');     
                    config.outstation.params.selectTimeout = 
                        openpal::TimeDuration::Seconds(atoi(token.c_str()));
                } else if (token == "max_controls_per_request") {
                    getline(iss, token, '=');
                    config.outstation.params.maxControlsPerRequest = 
                        atoi(token.c_str()); 
                } else if (token == "max_rx_frag_size") {
                    getline(iss, token, '=');     
                    config.outstation.params.maxRxFragSize = 
                        atoi(token.c_str());
                } else if (token == "max_tx_frag_size") {
                    getline(iss, token, '=');     
                    config.outstation.params.maxTxFragSize = 
                        atoi(token.c_str());
                } else if (token == "event_buffer_size") {
                    getline(iss, token, '=');     
                    config.outstation.eventBufferConfig =
                        EventBufferConfig::AllTypes(atoi(token.c_str()));

// get offsets from dnp.cfg (yurgen1975)
                } else if (token == "offset_di") {
                    getline(iss, token, '=');     
                    offset_di = atoi(token.c_str());
                        
                } else if (token == "offset_do") {
                    getline(iss, token, '=');     
                    offset_do = atoi(token.c_str());
                        
                } else if (token == "offset_ai") {
                    getline(iss, token, '=');     
                    offset_ai = atoi(token.c_str());
                        
                } else if (token == "offset_ao") {
                    getline(iss, token, '=');     
                    offset_ao = atoi(token.c_str());
// -------------------------------------------------------------------

                } else if (token == "sol_confirm_timeout") {
                    getline(iss, token, '=');     
                    config.outstation.params.solConfirmTimeout =
                        openpal::TimeDuration::Milliseconds(
                            atoi(token.c_str())
                        );
                } else if (token == "unsol_confirm_timeout") {
                    getline(iss, token, '=');     
                    config.outstation.params.unsolConfirmTimeout = 
                        openpal::TimeDuration::Milliseconds(
                            atoi(token.c_str())
                        );
                } else if (token == "unsol_retry_timeout") {
                    getline(iss, token, '=');
                    config.outstation.params.unsolRetryTimeout = 
                        openpal::TimeDuration::Milliseconds(
                            atoi(token.c_str())
                        );
                }
            }
            catch(...) {
                cout << "Malformatted Line: " << line << endl;
                exit(1);
            }
        }
    }

    return config;
} 

/*class ILogHandler
{
    public:
        virtual void Log( const LogEntry& entry )
        {
            printf("I have logged something\n");
        }
};
*/


void ConsoleLogger::Log(const openpal::LogEntry& entry)
{
    char log_msg[1000];
    sprintf(log_msg, "DNP3 ID %s: %s\n", entry.loggerid, entry.message);
    log(log_msg);
}


//------------------------------------------------------------------
//Function to begin DNP3 server functions
//------------------------------------------------------------------
void dnp3StartServer(int port) {

    const uint32_t FILTERS = levels::NORMAL;

    // Allocate a single thread to the pool since this is a single outstation
    // Log messages to the console
    DNP3Manager manager(1, ConsoleLogger::Create());

    // Create a listener server
    auto channel = manager.AddTCPServer("DNP3_Server", FILTERS, ChannelRetry::Default(), "0.0.0.0", port, PrintingChannelListener::Create());

    // Create a new outstation with a log level, command handler, and
    // config info this returns a thread-safe interface used for
    // updating the outstation's database.
    std::shared_ptr<ICommandHandler> cc = std::make_shared<CommandCallback>();
    auto outstation = channel->AddOutstation(
            "outstation",
            cc, 
            DefaultOutstationApplication::Create(), 
            parseDNP3Config()
    );

    // Enable the outstation and start communications
    outstation->Enable();
    printf("DNP3 Enabled \n");

    mapUnusedIO();

    // Continuously update
    struct timespec timer_start;
    clock_gettime(CLOCK_MONOTONIC, &timer_start);
    
    while(run_dnp3) 
    {
        pthread_mutex_lock(&bufferLock);
        update_vals(outstation);
        pthread_mutex_unlock(&bufferLock);
        sleep_until(&timer_start, OPLC_CYCLE);
    }
    
    printf("Shutting down DNP3 server\n");
    channel->Shutdown();
    printf("DNP3 Server deactivated\n");
}
