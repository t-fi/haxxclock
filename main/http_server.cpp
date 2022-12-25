#include <esp_event.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_tls_crypto.h"
#include "segment_frame_generator.h"
#include <esp_http_server.h>

// todo: refactor into file
auto resp = "<html lang=en><title>haxxclock</title><link crossorigin=anonymous href=https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css integrity=sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC rel=stylesheet><script crossorigin=anonymous integrity=sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM src=https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js></script><script crossorigin=anonymous integrity=\"sha256-pvPw+upLPUjgMXY0G+8O0xUf+/Im1MZjXxxgOcBQBXU=\"src=https://code.jquery.com/jquery-3.6.3.min.js></script><style>body{background:#f0f0f0}.container{max-width:100%!important}.btn-color{width:100%;height:75px;margin:0;border:2px solid #d0d0d0}.col{padding:0!important;margin:0 1 0 1}.btn-color-blk{background:#000}.btn-color-red{background:red}.btn-color-green{background:#0f0}.btn-color-blue{background:#00f}.btn-color-white{background:#fff}</style><div class=accordion id=accordionExample><div class=accordion-item><h2 class=accordion-header id=headingOne><button class=accordion-button type=button data-bs-target=#accOne data-bs-toggle=collapse>Fixed Color Grid</button></h2><div class=\"accordion-collapse collapse show\"id=accOne aria-labelledby=headingOne data-bs-parent=#accordionExample><div class=accordion-body><div class=container><div class=row><div class=col><button class=\"btn btn-color btn-color-blk\"type=button id=blk></div><div class=col><button class=\"btn btn-color btn-color-red\"type=button id=red></div><div class=col><button class=\"btn btn-color btn-color-green\"type=button id=green></div><div class=col><button class=\"btn btn-color btn-color-blue\"type=button id=blue></div><div class=col><button class=\"btn btn-color btn-color-white\"type=button id=white></div></div></div></div></div></div><div class=accordion-item><h2 class=accordion-header id=headingTwo><button class=\"accordion-button collapsed\"type=button data-bs-target=#accTwo data-bs-toggle=collapse aria-controls=accTwo aria-expanded=false>Color Picker</button></h2><div class=\"accordion-collapse collapse\"id=accTwo aria-labelledby=headingTwo data-bs-parent=#accordionExample><div class=accordion-body></div></div></div></div><script>const rgb2hex = (rgb) => `#${rgb.match(/^rgb\\((\\d+),\\s*(\\d+),\\s*(\\d+)\\)$/).slice(1).map(n => parseInt(n, 10).toString(16).padStart(2, '0')).join('')}`\n"
            "            const regex = /(rgb\\(\\d+,[ ]*\\d+,[ ]*\\d+\\))/gm;\n"
            "\n"
            "            $(document).ready(function() {\n"
            "                $(\".btn-color\").click(function() {\n"
            "                    let rgbColor = $(this).css(\"background\");\n"
            "                    let hexColor = rgb2hex(rgbColor.match(regex)[0]);\n"
            "                    $.post(\"\", { color: hexColor  });\n"
            "                });\n"
            "            });</script>";

esp_err_t get_handler(httpd_req_t *req)
{
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t post_handler(httpd_req_t *req)
{
    char content[100];

    size_t recv_size = MIN(req->content_len, sizeof(content));
    int ret = httpd_req_recv(req, content, recv_size);

    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }

        return ESP_FAIL;
    }

    char color[16];
    memcpy(color, &content[9], 6); // todo: change to 16 bit
    color[6] = '\0';
    set_color(color);

    httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_uri_t uri_get = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = get_handler,
        .user_ctx = NULL
};

httpd_uri_t uri_post = {
        .uri      = "/",
        .method   = HTTP_POST,
        .handler  = post_handler,
        .user_ctx = NULL
};

httpd_handle_t start_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }

    return server;
}

void stop_webserver(httpd_handle_t server)
{
    if (server) {
        httpd_stop(server);
    }
}
