/**
 * @file client_session.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Runs the lifetime of a Mondradiko client session.
 * @date 2020-10-24
 * 
 * @copyright Copyright (c) 2020 Marceline Cramer
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 * 
 */

#ifndef SRC_SESSION_CLIENT_SESSION_H_
#define SRC_SESSION_CLIENT_SESSION_H_

void client_session_run(const char* serverAddress, int serverPort);

#endif  // SRC_SESSION_CLIENT_SESSION_H_
