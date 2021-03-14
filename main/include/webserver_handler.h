//webserver_handler.h
#ifndef WEB_SRV_H
#define WEB_SRV_H

// static headers for HTTP responses
const static char http_html_hdr[] = "HTTP/1.1 200 OK\nContent-type: text/html\n\n";
const static char http_404_hdr[] = "HTTP/1.1 404 NOT FOUND\n\n";

//external function prototype declaration
void vTaskHttpServer(void *pvParameters);

#endif //WEB_SRV_H
//EOF