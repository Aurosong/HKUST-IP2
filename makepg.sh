#!/bin/bash

# 设置变量
POSTGRESQL_VERSION="17.0" # 替换为你要编译的版本
SOURCE_DIR="/app/pg_src" # 源码目录
BUILD_DIR="$SOURCE_DIR/build" # 构建目录
INSTALL_DIR="/app/pgsql" # 安装目录

# 创建构建目录
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 配置编译选项
echo "配置编译选项..."
export CFLAGS="-g3 -O0 -fno-omit-frame-pointer"
$SOURCE_DIR/configure --prefix=$INSTALL_DIR \
                      --enable-debug \
                      --without-icu \

# 编译
echo "开始编译..."
make

# 安装
echo "安装 PostgreSQL..."
make install

# 清理构建目录
echo "清理构建目录..."
cd ..
rm -rf $BUILD_DIR

echo "PostgreSQL $POSTGRESQL_VERSION 编译和安装完成！"