#include <esp_event.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_tls_crypto.h"
#include "segment_frame_generator.h"
#include <esp_http_server.h>
#include <string>
#include <vector>
#include <sstream>

// todo: refactor into file
auto resp = "<html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>haxxclock</title><link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css\" crossorigin=\"anonymous\"><script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js\" crossorigin=\"anonymous\"></script><script src=\"https://code.jquery.com/jquery-3.6.3.min.js\" crossorigin=\"anonymous\"></script><script src=\"https://cdn.jsdelivr.net/npm/@jaames/iro@5\"></script><style>body{background:#f0f0f0}.container{max-width:100%!important}.col{padding:0!important;margin:0 1px}.centered{margin:auto;display:table}</style></head><body class=\"bg-body\"><div class=\"container\"><div class=\"row p-2 bg-dark border-bottom\" style=\"--bs-bg-opacity:.08\"><div class=\"col text-center\"><h1 class=\"display-6\">Haxx Clock Color</h1></div></div><div class=\"row p-2 bg-dark\" style=\"--bs-bg-opacity:.05\"><div class=\"col\"><div id=\"picker\" class=\"centered\"></div><input type=\"range\" class=\"p-4 form-range centered\" style=\"width:320px\" id=\"slider\" min=\"0\" max=\"1\" step=\"0.001\"><button type=\"button\" class=\"p-2 btn btn-outline-dark centered\" style=\"width:275px\" id=\"toggle\">Toggle</button></div></div></div><script>let regex = /(rgb\\(\\d+,[ ]*\\d+,[ ]*\\d+\\))/gm;"
            "    let colorMem = \"ffffff\";"
            "    let isOn = true;"
            "    function setColor(color, brightness) {"
            "        $.post(\"/color\", {"
            "            color: color,"
            "            brightness: brightness"
            "        });"
            "        colorMem = color;"
            "    }"
            "    function toggle() {"
            "        $.post(\"/toggle\", {});"
            "        isOn = !isOn;"
            "    }"
            "    $(document).ready(function () {"
            "        let colorPicker = new iro.ColorPicker('#picker', {"
            "            layout: [{"
            "                component: iro.ui.Wheel"
            "            }],"
            "            borderWidth: 3,"
            "            options: {"
            "                borderColor: \"#ffffff\""
            "            }"
            "        });"
            "        colorPicker.on('color:change', function (color) {"
            "            setColor(color.hexString.slice(1), $(\"#slider\").val());"
            "        });"
            "        $(\"#slider\").change(function () {"
            "            setColor(colorMem, $(\"#slider\").val());"
            "        });"
            "        $(\"#toggle\").on(\"click\", function () {"
            "            toggle();"
            "        });"
            "        $(\"#slider\").val(1);"
            "    });</script></body></html>";

esp_err_t get_handler(httpd_req_t *req)
{
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t post_color_handler(httpd_req_t *req)
{
    std::array<char, 100> content = {0};

    size_t recv_size = MIN(req->content_len, sizeof(content));
    int ret = httpd_req_recv(req, content.data(), recv_size);

    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }

        return ESP_FAIL;
    }

    printf("Received: %s\n", content.data());

    char color[7];
    memcpy(color, &content.data()[6], 6);
    color[6] = '\0';
    printf("Color: %s\n", color);

    char brightness[6];
    memcpy(brightness, &content.data()[24], 6);
    printf("Brightness: %s\n", brightness);

    set_color(color, std::atof(brightness));

    httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t post_toggle_handler(httpd_req_t *req)
{
    std::array<char, 16> content = {0};

    size_t recv_size = MIN(req->content_len, sizeof(content));
    int ret = httpd_req_recv(req, content.data(), recv_size);

    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }

        return ESP_FAIL;
    }

    printf("Received: %s\n", content.data());
    toggle_clock();

    httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_uri_t uri_get = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = get_handler,
        .user_ctx = NULL
};

httpd_uri_t uri_post_color = {
        .uri      = "/color",
        .method   = HTTP_POST,
        .handler  = post_color_handler,
        .user_ctx = NULL
};

httpd_uri_t uri_post_toggle = {
        .uri      = "/toggle",
        .method   = HTTP_POST,
        .handler  = post_toggle_handler,
        .user_ctx = NULL
};

httpd_handle_t start_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post_color);
        httpd_register_uri_handler(server, &uri_post_toggle);
    }

    return server;
}

void stop_webserver(httpd_handle_t server)
{
    if (server) {
        httpd_stop(server);
    }
}
