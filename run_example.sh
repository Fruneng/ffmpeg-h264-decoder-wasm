#!/bin/bash

docker run -it --rm --name some-nginx -p 8080:80 -v $PWD/dist:/usr/share/nginx/html:ro nginx