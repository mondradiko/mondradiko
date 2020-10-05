#pragma once

/**
 * @brief Executes a full client session lifetime.
 * @param serverAddress The address of the server to connect to.
 * @param serverPort The port to connect to on the server.
 * @returns True on a successful session, false on errors.
 */
bool client_session_run(const char* serverAddress, int serverPort);
