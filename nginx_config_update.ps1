$configPath = "$env:APPDATA\Local\run\router\nginx\conf\route.appampxco.local.conf"

$newConfig = @"
server {
        server_name appampxco.local *.appampxco.local;

        include server-block.conf;

        location / {
                proxy_pass http://127.0.0.1:10023;
                include location-block.conf;
        }
}

server {
        server_name appampxco.local *.appampxco.local;

        ssl_certificate      ../certs/appampxco.local.crt;
        ssl_certificate_key  ../certs/appampxco.local.key;

        include server-block-ssl.conf;

        location / {
                proxy_pass http://127.0.0.1:10023;
                include location-block.conf;
        }
}

# New server block for ESP32 access via IP address
server {
        listen 192.168.2.121:10023;
        server_name 192.168.2.121;

        location / {
                proxy_pass http://127.0.0.1:10023;
                include location-block.conf;
        }
}
"@

Set-Content $configPath $newConfig
Write-Host "Configuration updated! Nginx will now accept connections on 192.168.2.121:10023"
Write-Host "You need to restart LocalWP for changes to take effect." 