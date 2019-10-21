# docker

Run in docker

## build

    docker build -t ccmix .

## run

Make sure you have a `` in the working directory else add one using `-v` to `/usr/share/ccmix`

    docker run --rm -v $(pwd):$(pwd) -it ccmix
