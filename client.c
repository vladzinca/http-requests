#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

/* functie care verifica daca un sir contine doar cifre */
int stringIsInt(char *s)
{
    for (int i = 0; i < strlen(s); i++)
        if (s[i] < 48 || s[i] > 57)
            return 0;
    return 1;
}

int main(int argc, char *argv[])
{
    char *message, *response, *command, *cookie, *jwt_token;
    int sockfd;

    command = calloc(128, sizeof(char));
    cookie = calloc(3, sizeof(char));
    strcpy(cookie, "-1");
    jwt_token = calloc(3, sizeof(char));
    strcpy(jwt_token, "-1");

    while (1)
    {
        /* deschide conexiunea si citeste comanda */
        sockfd = open_connection("34.241.4.235", 8080,
                                 AF_INET, SOCK_STREAM, 0);
        scanf("%s", command);
        /* comanda register */
        if (strcmp(command, "register") == 0)
        {
            char *arg1 = calloc(128, sizeof(char));
            char *arg2 = calloc(128, sizeof(char));

            char **form_data = calloc(1, sizeof(char *));
            form_data[0] = calloc(LINELEN, sizeof(char));

            /* formateaza mesajul */
            strcat(form_data[0], "{\n\t\"username\": \"");
            printf("username=");
            scanf("%s", arg1);
            strcat(form_data[0], arg1);
            strcat(form_data[0], "\",\n\t\"password\": \"");
            printf("password=");
            scanf("%s", arg2);
            strcat(form_data[0], arg2);
            strcat(form_data[0], "\"\n}");

            message = compute_post_request("34.241.4.235",
                                           "/api/v1/tema/auth/register",
                                           "application/json", form_data, 1,
                                           NULL, 1, NULL, 0);
            /* trimite mesajul catre server */
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            /* in functie de raspunsul serverului,
               afiseaza un mesaj la stdout */
            if (strncmp(response, "HTTP/1.1 201", 12) == 0)
                puts("Created");
            else if (strncmp(response, "HTTP/1.1 400", 12) == 0)
                printf("\"The username %s is taken!\"\n", arg1);
            else if (strncmp(response, "HTTP/1.1 429", 12) == 0)
                puts("\"Too many requests, please try again later.\"");
            else
                puts("err");

            /* elibereaza memoria */
            free(arg1);
            free(arg2);
            free(form_data);
            free(message);
            free(response);
        }
        /* comanda login */
        else if (strcmp(command, "login") == 0)
        {
            char *arg1 = calloc(128, sizeof(char));
            char *arg2 = calloc(128, sizeof(char));

            char **form_data = calloc(1, sizeof(char *));
            form_data[0] = calloc(LINELEN, sizeof(char));

            strcat(form_data[0], "{\n\t\"username\": \"");
            printf("username=");
            scanf("%s", arg1);
            strcat(form_data[0], arg1);
            strcat(form_data[0], "\",\n\t\"password\": \"");
            printf("password=");
            scanf("%s", arg2);
            strcat(form_data[0], arg2);
            strcat(form_data[0], "\"\n}");

            message = compute_post_request("34.241.4.235",
                                           "/api/v1/tema/auth/login",
                                           "application/json", form_data, 1,
                                           NULL, 1, NULL, 0);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strncmp(response, "HTTP/1.1 200", 12) == 0)
            {
                puts("OK");
                /* salveaza cookie */
                char *aux = calloc(1024, sizeof(char));
                strcpy(aux, response + 277);
                char *token = strtok(aux, ";");
                cookie = realloc(cookie, strlen(token) * sizeof(char));
                strcpy(cookie, token);
                free(aux);
                strcpy(jwt_token, "-1");
            }
            else if (strncmp(response, "HTTP/1.1 400", 12) == 0)
            {
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, "}");
                printf("%s\n", token);
            }
            else if (strncmp(response, "HTTP/1.1 429", 12) == 0)
            {
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, "}");
                printf("%s\n", token);
            }
            else
                puts("err");

            free(arg1);
            free(arg2);
            free(form_data);
            free(message);
            free(response);
        }
        /* comanda enter_library */
        else if (strcmp(command, "enter_library") == 0)
        {
            char **cookies = calloc(1, sizeof(char *));
            cookies[0] = calloc(LINELEN, sizeof(char));
            strcpy(cookies[0], cookie);

            message = compute_get_request("34.241.4.235",
                                          "/api/v1/tema/library/access", NULL,
                                          cookies, 1, NULL, 0);
            /* trimite mesajul catre server, folosind cookie */
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strncmp(response, "HTTP/1.1 200", 12) == 0)
            {
                puts("OK");
                /* salveaza tokenul JWT */
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, "\"");
                jwt_token = realloc(jwt_token, strlen(token) * sizeof(char));
                strcpy(jwt_token, token);
            }
            else if (strncmp(response, "HTTP/1.1 401", 12) == 0)
            {
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, "}");
                printf("%s\n", token);
            }
            else
                puts("err");

            free(cookies);
            free(message);
            free(response);
        }
        /* comanda get_books */
        else if (strcmp(command, "get_books") == 0)
        {
            char **authorizations = calloc(1, sizeof(char *));
            authorizations[0] = calloc(LINELEN, sizeof(char));
            strcpy(authorizations[0], jwt_token);

            message = compute_get_request("34.241.4.235",
                                          "/api/v1/tema/library/books", NULL,
                                          NULL, 0, authorizations, 1);
            /* trimite mesajul catre server, folosind tokenul JWT */
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strncmp(response, "HTTP/1.1 200", 12) == 0)
            {
                int response_length = strlen(response);
                char *token = strtok(response, "{");
                if (strlen(token) == response_length)
                    puts("\"No books yet.\"");
                else
                {
                    while (token != NULL)
                    {
                        token = strtok(NULL, ":");
                        if (token != NULL)
                            token = strtok(NULL, ",");
                        else
                            continue;
                        printf("ID: %s, ", token);
                        token = strtok(NULL, ":");
                        token = strtok(NULL, "}");
                        printf("Title: %s\n", token);
                        token = strtok(NULL, "{");
                    }
                }
            }
            else if (strncmp(response, "HTTP/1.1 500", 12) == 0)
            {
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, "}");
                printf("%s\n", token);
            }
            else
                puts("err");

            free(authorizations);
            free(message);
            free(response);
        }
        /* comanda get_book */
        else if (strcmp(command, "get_book") == 0)
        {
            char *arg1 = calloc(128, sizeof(char));
            printf("id=");
            scanf("%s", arg1);

            char *route = calloc(1024, sizeof(char));
            strcpy(route, "/api/v1/tema/library/books/");
            strcat(route, arg1);

            char **authorizations = calloc(1, sizeof(char *));
            authorizations[0] = calloc(LINELEN, sizeof(char));
            strcpy(authorizations[0], jwt_token);

            message = compute_get_request("34.241.4.235", route, NULL,
                                          NULL, 0, authorizations, 1);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strncmp(response, "HTTP/1.1 200", 12) == 0)
            {
                printf("ID: %s\n", arg1);
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, ",");
                printf("Title: %s\n", token);
                token = strtok(NULL, ":");
                token = strtok(NULL, ",");
                printf("Author: %s\n", token);
                token = strtok(NULL, ":");
                token = strtok(NULL, ",");
                printf("Publisher: %s\n", token);
                token = strtok(NULL, ":");
                token = strtok(NULL, ",");
                printf("Genre: %s\n", token);
                token = strtok(NULL, ":");
                token = strtok(NULL, "}");
                printf("Page count: %s\n", token);
            }
            else if (strncmp(response, "HTTP/1.1 404", 12) == 0)
            {
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, "}");
                printf("%s\n", token);
            }
            else if (strncmp(response, "HTTP/1.1 500", 12) == 0)
            {
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, "}");
                printf("%s\n", token);
            }
            else
                puts("err");

            free(arg1);
            free(route);
            free(authorizations);
            free(message);
            free(response);
        }
        /* comanda add_book */
        else if (strcmp(command, "add_book") == 0)
        {
            char *arg1 = calloc(128, sizeof(char));
            char *arg2 = calloc(128, sizeof(char));
            char *arg3 = calloc(128, sizeof(char));
            char *arg4 = calloc(128, sizeof(char));
            char *arg5 = calloc(128, sizeof(char));

            char **form_data = calloc(1, sizeof(char *));
            form_data[0] = calloc(LINELEN, sizeof(char));

            /* formateaza mesajul */
            strcat(form_data[0], "{\n\t\"title\": \"");
            printf("title=");
            fgets(arg1, 128, stdin);
            fgets(arg1, 128, stdin);
            strtok(arg1, "\n");
            if (arg1[0] == '\n')
                arg1[0] = 0;
            strcat(form_data[0], arg1);

            strcat(form_data[0], "\",\n\t\"author\": \"");
            printf("author=");
            fgets(arg2, 128, stdin);
            strtok(arg2, "\n");
            if (arg2[0] == '\n')
                arg2[0] = 0;
            strcat(form_data[0], arg2);

            strcat(form_data[0], "\",\n\t\"genre\": \"");
            printf("genre=");
            fgets(arg3, 128, stdin);
            strtok(arg3, "\n");
            if (arg3[0] == '\n')
                arg3[0] = 0;
            strcat(form_data[0], arg3);

            strcat(form_data[0], "\",\n\t\"publisher\": \"");
            printf("publisher=");
            fgets(arg4, 128, stdin);
            strtok(arg4, "\n");
            if (arg4[0] == '\n')
                arg4[0] = 0;
            strcat(form_data[0], arg4);

            strcat(form_data[0], "\",\n\t\"page_count\": \"");
            printf("page_count=");
            fgets(arg5, 128, stdin);
            strtok(arg5, "\n");
            if (arg5[0] == '\n')
                arg5[0] = 0;
            strcat(form_data[0], arg5);
            strcat(form_data[0], "\"\n}");

            /* daca page_count contine doar cifre */
            if (stringIsInt(arg5))
            {
                /* daca toate campurile au fost completate */
                if (strlen(arg1) != 0 && strlen(arg2) != 0 &&
                    strlen(arg3) != 0 && strlen(arg4) != 0 &&
                    strlen(arg5) != 0)
                {
                    char **authorizations = calloc(1, sizeof(char *));
                    authorizations[0] = calloc(LINELEN, sizeof(char));
                    strcpy(authorizations[0], jwt_token);

                    message = compute_post_request("34.241.4.235",
                                                  "/api/v1/tema/library/books",
                                                  "application/json",
                                                  form_data, 1,
                                                  NULL, 1,
                                                  authorizations, 1);
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);

                    if (strncmp(response, "HTTP/1.1 200", 12) == 0)
                        puts("OK");
                    else if (strncmp(response, "HTTP/1.1 429", 12) == 0)
                        puts("\"Too many requests, please try again later.\"");
                    else if (strncmp(response, "HTTP/1.1 500", 12) == 0)
                    {
                        char *token = strtok(response, "{");
                        token = strtok(NULL, ":");
                        token = strtok(NULL, "}");
                        printf("%s\n", token);
                    }
                    else
                        puts("err");

                    free(arg1);
                    free(arg2);
                    free(arg3);
                    free(arg4);
                    free(arg5);
                    free(form_data);
                    free(authorizations);
                    free(message);
                    free(response);
                }
                /* daca nu au fost completate toate campurile */
                else
                {
                    printf("\"Invalid input.\"\n");

                    free(arg1);
                    free(arg2);
                    free(arg3);
                    free(arg4);
                    free(arg5);
                    free(form_data);
                }
            }
            /* daca page_count nu contine doar cifre */
            else
            {
                printf("\"Page count will not work.\"\n");
                free(arg1);
                free(arg2);
                free(arg3);
                free(arg4);
                free(arg5);
                free(form_data);
            }
        }
        /* comanda delete_book */
        else if (strcmp(command, "delete_book") == 0)
        {
            char *arg1 = calloc(128, sizeof(char));
            printf("id=");
            scanf("%s", arg1);

            char *route = calloc(1024, sizeof(char));
            strcpy(route, "/api/v1/tema/library/books/");
            strcat(route, arg1);

            char **authorizations = calloc(1, sizeof(char *));
            authorizations[0] = calloc(LINELEN, sizeof(char));
            strcpy(authorizations[0], jwt_token);

            message = compute_delete_request("34.241.4.235", route, NULL,
                                             NULL, 0, authorizations, 1);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strncmp(response, "HTTP/1.1 200", 12) == 0)
                puts("OK");
            else if (strncmp(response, "HTTP/1.1 404", 12) == 0)
            {
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, "}");
                printf("%s\n", token);
            }
            else if (strncmp(response, "HTTP/1.1 429", 12) == 0)
                puts("\"Too many requests, please try again later.\"");
            else if (strncmp(response, "HTTP/1.1 500", 12) == 0)
            {
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, "}");
                printf("%s\n", token);
            }
            else
                puts("err");

            free(arg1);
            free(route);
            free(authorizations);
            free(message);
            free(response);
        }
        /* comanda logout */
        else if (strcmp(command, "logout") == 0)
        {
            char **cookies = calloc(1, sizeof(char *));
            cookies[0] = calloc(LINELEN, sizeof(char));
            strcpy(cookies[0], cookie);

            message = compute_get_request("34.241.4.235",
                                          "/api/v1/tema/auth/logout", NULL,
                                          cookies, 1, NULL, 0);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strncmp(response, "HTTP/1.1 200", 12) == 0)
            {
                puts("OK");
                /* se asigura ca clientul nu mai are acces la biblioteca */
                strcpy(cookie, "-1");
                strcpy(jwt_token, "-1");
            }
            else if (strncmp(response, "HTTP/1.1 400", 12) == 0)
            {
                char *token = strtok(response, "{");
                token = strtok(NULL, ":");
                token = strtok(NULL, "}");
                printf("%s\n", token);
            }
            else
                puts("err");

            free(cookies);
            free(message);
            free(response);
        }
        /* comanda exit */
        else if (strcmp(command, "exit") == 0)
            break;
        /* comanda invalida */
        else
            puts("\"Unknown command.\"");
    }

    /* elibereaza memoria */
    free(command);
    free(cookie);
    free(jwt_token);
    close(sockfd);

    return 0;
}
