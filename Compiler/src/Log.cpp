#include "../include/pcheaders.h"
#include "../include/Log.h"

#define F_NAME "output/log.txt"

Log::Log(){
    this->m_stack.push_back(0);
    this->enable = true;
    m_fp = fopen(F_NAME, "w");
    //m_fp = stdout;

    if(!m_fp){
        fprintf(stderr, "Error: Could not open file \"%s\"\n", F_NAME);
        exit(1);
    }
}

Log::~Log(){
    if(m_fp != stdout)
        fclose(m_fp);
}

Log& Log::Get(){
    static Log instance;
    return instance;
}

void Log::Push(){
    Log::Get().m_stack.push_back(0);
}

void Log::Pop(){
    Log::Get().m_stack.pop_back();
}

void Log::MSG(const std::string& msg){
    if(!Log::Get().enable)
        return;
    
    Log::Get().m_stack[Log::Get().m_stack.size() - 1]++;
    for(int i = 0; i < (int) Log::Get().m_stack.size() - 1; i++)
        fprintf(Log::Get().m_fp, "%d.", Log::Get().m_stack[i]);
    fprintf(Log::Get().m_fp, "%d) ", Log::Get().m_stack[Log::Get().m_stack.size() - 1]);
    fprintf(Log::Get().m_fp, "%s\n", msg.c_str());
}

void Log::Save(){
    if(Log::Get().m_fp != stdout){
        fclose(Log::Get().m_fp);
        Log::Get().m_fp = fopen(F_NAME, "a");
    }
}

bool Log::isEnable(){
    return Log::Get().enable;
}

void Log::Enable(const bool enable){
    Log::Get().enable = enable;
}

void Log::EnablePush(const bool enable){
    Log::Get().m_enableStack.push_back(Log::Get().enable);
    Log::Get().enable = enable;
}

void Log::EnablePop(){
    if(Log::Get().m_enableStack.size() > 0){
        Log::Get().enable = Log::Get().m_enableStack[Log::Get().m_enableStack.size() - 1];
        Log::Get().m_enableStack.pop_back();
    }
}