FROM advantechiiot/imx-yocto-sdk:kirkstone-qt6.4

WORKDIR /src
ENTRYPOINT ["/sdk_setenv.sh"]