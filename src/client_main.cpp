#include <iostream>

#include "log/log.hpp"
#include "session/client_session.hpp"

int main(int argc, const char* argv[])
{
    log_inf("Hello VR!");

    bool ranSuccessfully = client_session_run("127.0.0.1", 10555);

    if(ranSuccessfully) {
        return 0;
    } else {
        log_ftl("Mondradiko client failed.");
        return 1;
    }
}
