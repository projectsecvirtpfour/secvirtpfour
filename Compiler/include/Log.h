#ifndef LOG_H
#define LOG_H

class Log{
    public:
        Log(const Log&) = delete;
        void operator=(const Log&) = delete;

        static Log& Get();

        static void Push();
        static void Pop();

        static void MSG(const std::string& msg);

        /*
        Close file and reopen in append mode.
        */
        static void Save();

        static bool isEnable();
        static void Enable(const bool enable);
        static void EnablePush(const bool enable);
        static void EnablePop();
    private:
        Log();
        ~Log();

        FILE *m_fp;
        std::vector<int> m_stack;
        std::vector<bool> m_enableStack;
        bool enable;
};

#endif