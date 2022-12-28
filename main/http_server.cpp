#include <esp_event.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_tls_crypto.h"
#include "segment_frame_generator.h"
#include <esp_http_server.h>

// todo: refactor into file
auto resp = "<html lang=en><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>haxxclock</title><link crossorigin=anonymous href=https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css rel=stylesheet><script src=https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js crossorigin=anonymous></script><script src=https://code.jquery.com/jquery-3.6.3.min.js crossorigin=anonymous></script><script src=https://cdn.jsdelivr.net/npm/@jaames/iro@5></script><style>body{background:#f0f0f0}.container{max-width:100%!important}.btn-color{width:100%;height:75px;margin:0;border:2px solid #d0d0d0}.col{padding:0!important;margin:0 1px}.color-picker{margin:auto;display:table}</style><div class=accordion><div class=accordion-item><h2 class=accordion-header id=headingOne><button class=accordion-button data-bs-target=#accOne data-bs-toggle=collapse type=button>Fixed Color Grid</button></h2><div class=\"accordion-collapse collapse show\"id=accOne data-bs-parent=#accordionExample><div class=accordion-body><div class=container id=color-grid></div></div></div></div><div class=accordion-item><h2 class=accordion-header id=headingTwo><button class=accordion-button data-bs-target=#accTwo data-bs-toggle=collapse type=button>Color Picker</button></h2><div class=\"accordion-collapse collapse show\"id=accTwo data-bs-parent=#accordionExample><div class=accordion-body><div class=row><div class=col><div class=color-picker id=picker></div></div></div></div></div></div></div><script>let rgb2hex = (rgb) => `#${rgb.match(/^rgb\\((\\d+),\\s*(\\d+),\\s*(\\d+)\\)$/).slice(1).map(n => parseInt(n, 10).toString(16).padStart(2, '0')).join('')}`\n"
            "    let regex = /(rgb\\(\\d+,[ ]*\\d+,[ ]*\\d+\\))/gm;\n"
            "\n"
            "    $(document).ready(function () {\n"
            "        let colors = [\n"
            "            \"#000000\",\n"
            "            \"#ff0000\",\n"
            "            \"#00ff00\",\n"
            "            \"#0000ff\",\n"
            "            \"#ffffff\",\n"
            "            \"#ff8000\",\n"
            "            \"#ffff00\",\n"
            "            \"#00ffff\",\n"
            "            \"#ff00ff\",\n"
            "            \"#ff007f\"\n"
            "\n"
            "        ];\n"
            "\n"
            "        for (let i = 0; i < (colors.length / 5); ++i) {\n"
            "            let row = $('<div id=\"color-grid-one\" class=\"row\"></div>')\n"
            "            $(\"#color-grid\").append(row)\n"
            "\n"
            "            for (let j = 0; j < 5; ++j) {\n"
            "                row.append(`<div class=\"col\"><button type=\"button\" class=\"btn btn-color\" style=\"background: ${colors[5 * i + j]};\" /></div>`);\n"
            "            }\n"
            "        }\n"
            "\n"
            "        let colorPicker = new iro.ColorPicker('#picker');\n"
            "        colorPicker.on('color:change', function (color) {\n"
            "            $.post(\"\", {color: color.hexString.slice(1)});\n"
            "        });\n"
            "\n"
            "        $(\".btn-color\").click(function () {\n"
            "            let rgbColor = $(this).css(\"background\");\n"
            "            let hexColor = rgb2hex(rgbColor.match(regex)[0]);\n"
            "            $.post(\"\", {color: hexColor.slice(1)});\n"
            "        });\n"
            "    });</script>";

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

    printf("Received: %s", content);
    char color[7];
    memcpy(color, &content[6], 6); // todo: change to 16 bit
    color[6] = '\0';
    printf("Color: %s", color);
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
