# 基于 Ubuntu 18.04 的基础镜像
FROM --platform=linux/amd64 ubuntu:20.04

# 设置环境变量
ENV DEBIAN_FRONTEND=noninteractive

# 更新包列表并安装必要的工具
RUN apt-get update && apt-get install -y \
    software-properties-common \
    wget \
    && add-apt-repository ppa:ubuntu-toolchain-r/test -y \
    && apt-get update \
    && apt-get install -y \
    g++ \
    cmake \
    build-essential \
    libreadline-dev \
    libicu-dev \
    zlib1g-dev \
    libproj-dev proj-bin \
    libgeos-dev \
    libgdal-dev \
    libjson-c-dev libprotobuf-c-dev \
    protobuf-c-compiler libprotobuf-c-dev \
    flex \
    bison \
    sudo \
    python3 \
    python3-pip \
    python3-dev \
    libpq-dev \
    gdb \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN useradd -u 1004 chen && \
    echo "chen:528491" | chpasswd && \
    usermod -aG sudo chen && \
    echo "chen ALL=(ALL:ALL) NOPASSWD:ALL" >> /etc/sudoers

# 下载并安装 cmake 3.22.0
RUN wget https://github.com/Kitware/CMake/releases/download/v3.22.0/cmake-3.22.0-linux-x86_64.tar.gz \
    && tar -zxvf cmake-3.22.0-linux-x86_64.tar.gz \
    && cp -r cmake-3.22.0-linux-x86_64/* /usr/ \
    && rm -rf cmake-3.22.0-linux-x86_64 cmake-3.22.0-linux-x86_64.tar.gz

RUN wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add - \
    && echo "deb http://apt.postgresql.org/pub/repos/apt/ focal-pgdg main" > /etc/apt/sources.list.d/pgdg.list \
    && apt-get update \
    && apt-get install -y postgresql-server-dev-17 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /app \
    && chown -R 1004:1004 /app

RUN pip3 install --no-cache-dir numpy psycopg2

# 设置时区为 Asia/Hong_Kong
RUN apt-get update \
    && apt-get install -y tzdata \
    && ln -sf /usr/share/zoneinfo/Asia/Hong_Kong /etc/localtime \
    && echo "Asia/Hong_Kong" > /etc/timezone \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /app

# 默认命令
CMD ["bash"]