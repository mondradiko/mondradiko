#include <iostream>

#include "log/log.hpp"
#include "session/client_session.hpp"

int main(int argc, const char* argv[])
{
    log_inf("Hello VR!");

    try {
        client_session_run("127.0.0.1", 10555);
    } catch(const std::exception& e) {
        log_err("Mondradiko client failed with message:");
        log_err(e.what());
        return 1;
    }

    return 0;
}
