#!/bin/sh

sudo apt-get update && \
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    gcovr