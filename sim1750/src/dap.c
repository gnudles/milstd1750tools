#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <json-c/json.h>

#include "dap.h"

#define BUFFER_SIZE 8192

static void send_response(int client_sock, struct json_object *response) {
    const char *json_str = json_object_to_json_string_ext(response, JSON_C_TO_STRING_PLAIN);
    int len = strlen(json_str);
    char header[128];
    snprintf(header, sizeof(header), "Content-Length: %d\r\n\r\n", len);

    write(client_sock, header, strlen(header));
    write(client_sock, json_str, len);
}

static void handle_request(int client_sock, struct json_object *req) {
    struct json_object *seq_obj = NULL;
    struct json_object *command_obj = NULL;
    int seq = -1;
    const char *command = "";

    if (json_object_object_get_ex(req, "seq", &seq_obj)) {
        seq = json_object_get_int(seq_obj);
    }

    if (json_object_object_get_ex(req, "command", &command_obj)) {
        command = json_object_get_string(command_obj);
    }

    if (command == NULL) {
        command = "";
    }

    struct json_object *resp = json_object_new_object();
    json_object_object_add(resp, "type", json_object_new_string("response"));
    json_object_object_add(resp, "request_seq", json_object_new_int(seq));
    json_object_object_add(resp, "success", json_object_new_boolean(1));
    json_object_object_add(resp, "command", json_object_new_string(command));

    if (strcmp(command, "initialize") == 0) {
        struct json_object *body = json_object_new_object();
        json_object_object_add(body, "supportsConfigurationDoneRequest", json_object_new_boolean(1));
        json_object_object_add(resp, "body", body);
    } else if (strcmp(command, "disconnect") == 0) {
        // Just send success
    } else {
        // Stub for all other requests
        json_object_object_add(resp, "success", json_object_new_boolean(0));
        json_object_object_add(resp, "message", json_object_new_string("Not implemented"));
    }

    send_response(client_sock, resp);
    json_object_put(resp);
}

int dap_server_start(int port) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Error creating socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
    }

    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error on binding");
        close(server_sock);
        return -1;
    }

    listen(server_sock, 1);
    printf("DAP server listening on port %d...\n", port);

    client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_len);
    if (client_sock < 0) {
        perror("Error on accept");
        close(server_sock);
        return -1;
    }

    printf("DAP client connected.\n");

    char buffer[BUFFER_SIZE];
    int buffer_pos = 0;

    while (1) {
        int n = read(client_sock, buffer + buffer_pos, BUFFER_SIZE - 1 - buffer_pos);
        if (n <= 0) {
            printf("DAP client disconnected or error reading.\n");
            break;
        }
        buffer_pos += n;
        buffer[buffer_pos] = '\0';

        while (1) {
            char *header_end = strstr(buffer, "\r\n\r\n");
            if (!header_end) break; // Incomplete header

            int content_length = 0;
            char *content_length_str = strstr(buffer, "Content-Length: ");
            if (content_length_str && content_length_str < header_end) {
                content_length = atoi(content_length_str + 16);
            }

            if (content_length < 0 || content_length > BUFFER_SIZE - 4) {
                printf("DAP client error: Invalid content length %d.\n", content_length);
                goto cleanup;
            }

            int header_size = header_end - buffer + 4;
            if (buffer_pos < header_size + content_length) {
                break; // Incomplete body
            }

            // We have a full message
            char *body = buffer + header_size;
            char saved_char = body[content_length];
            body[content_length] = '\0';

            struct json_object *parsed_json = json_tokener_parse(body);
            if (parsed_json) {
                struct json_object *type_obj;
                if (json_object_object_get_ex(parsed_json, "type", &type_obj)) {
                    const char *type = json_object_get_string(type_obj);
                    if (type != NULL && strcmp(type, "request") == 0) {
                        handle_request(client_sock, parsed_json);

                        struct json_object *command_obj;
                        if (json_object_object_get_ex(parsed_json, "command", &command_obj)) {
                            const char *cmd = json_object_get_string(command_obj);
                            if (cmd != NULL && strcmp(cmd, "disconnect") == 0) {
                                json_object_put(parsed_json);
                                goto cleanup;
                            }
                        }
                    }
                }
                json_object_put(parsed_json);
            } else {
                printf("Error parsing JSON: %s\n", body);
            }

            body[content_length] = saved_char; // Restore

            // Shift remaining data to start of buffer
            int consumed = header_size + content_length;
            memmove(buffer, buffer + consumed, buffer_pos - consumed);
            buffer_pos -= consumed;
            buffer[buffer_pos] = '\0';
        }
    }

cleanup:
    close(client_sock);
    close(server_sock);
    printf("DAP server stopped.\n");
    return 0;
}
