#!/bin/bash

docker pull ghcr.io/nopesir/pdsproject-server:latest
docker run --rm -d  -p 63505:63505/tcp ghcr.io/nopesir/pdsproject-server:latest