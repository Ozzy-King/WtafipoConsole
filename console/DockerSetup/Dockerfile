# Use Ubuntu as the base image
FROM ubuntu:22.04

# Set environment variables for non-interactive installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt update && apt install -y \
	build-essential \
	python3 \
    python3-pip \
    cmake \
	gcc-arm-none-eabi \
	libnewlib-arm-none-eabi \
	libstdc++-arm-none-eabi-newlib \
	git 
	

# Clone the Pico SDK
WORKDIR /app
COPY . .

# Set up environment variables for the SDK
ENV PICO_SDK_PATH=/app/pico-sdk



# Set up entry point
CMD ["bash", "/app/build.sh"]