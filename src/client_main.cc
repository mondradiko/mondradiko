#include <iostream>

#include <physfs.h>

#include "log/log.h"
#include "session/client_session.h"

int main(int argc, const char* argv[])
{
    log_inf("Hello VR!");

    PHYSFS_init(argv[0]);

    try {
        client_session_run("127.0.0.1", 10555);
    } catch(const std::exception& e) {
        log_err("Mondradiko client failed with message:");
        log_err(e.what());
        PHYSFS_deinit();
        return 1;
    }

    PHYSFS_deinit();
    return 0;
}
