#pragma once


const char code200[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
const char code201[] = "HTTP/1.1 201 Created\r\nContent-Type: text/html\r\nContent-Length: ";
const char code204[] = "HTTP/1.1 204 No Content\r\nContent-Type: text/html\r\nContent-Length: ";

const char code400[] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: ";
const char code404[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: ";
const char code403[] = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: ";
const char code413[] = "HTTP/1.1 413 Content Too Large\r\nContent-Type: text/html\r\nContent-Length: ";
const char code501[] = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\nContent-Length: ";
const char code505[] = "HTTP/1.1 505 HTTP Version Not Supported\r\nContent-Type: text/html\r\nContent-Length: ";


const char pathNotFound[] = "www/01-pages/404.html";
const char pathForbidden[] = "www/01-pages/403.html";
const char notImplemented[] = "www/01-pages/501.html";
