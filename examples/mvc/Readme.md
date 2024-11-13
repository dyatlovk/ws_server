# MVC Example

## Backend setup
Make sure you've [builded server](/README.md) core library.

**Example config for NGINX. (Dev mode)**
In this case I use my test domain (srv.local). Of course,
you should change it to your own.
```bash
server {
    listen 443 ssl;
    http2 on;
    server_name srv.local;
    merge_slashes on;
    access_log /var/log/nginx/srv_access.log;
    error_log /var/log/nginx/srv_errors.log;

    location / {
            add_header 'Access-Control-Allow-Origin' '*' always;
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_pass http://localhost:3044/;
    }
}

server {
    listen 127.0.0.1:443 ssl;
    http2 on;
    server_name *.srv.local;
    include snippets/self-signed.conf;
}

server {
    listen 127.0.0.1:80;
    server_name .srv.local;
    location / {
        return 301 https://$host$request_uri;
    }
}
```
Check nginx setup:
```bash
sudo nginx -t
```

Restart nginx:
```bash
sudo systemctl restart nginx.service
```
If you hate systemd:
```bash
sudo rc-service nginx restart
```

Run server executable


## Frontend setup
Change current dir:
```bash
cd <path_to_frontend_dir>
```

Install dependencies:
```bash
npm install
```

Check .env file and update backend host

Start server in watch mode:
```bash
    npm run dev
```
