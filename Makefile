all: build

YOCTO_VERSION=hardknott

SRC_PATH=$(CURDIR)
OUTPUT_PATH=$(CURDIR)/output
DESTINATION_PATH=$(SRC_PATH)
VERSION=$(shell python3 ./tools/get_version.py)

DOCKERFILE=./res/kirkstone_qt_builder.Dockerfile
DOCKER_TAG_NAME=advantech/qt-builder-kirkstone

build-image:
	docker buildx build --platform linux/arm64 -t $(DOCKER_TAG_NAME) -f $(DOCKERFILE) .

build: build-image generate-makefile
	@echo "build in docker"
	docker run --rm --platform linux/arm64 -v $(SRC_PATH):/src $(DOCKER_TAG_NAME) make linux-build

linux-build: clean
	@echo "make start"
	$(MAKE) -f Makefile.qt

generate_project:
	@echo "generate qt project file in development environment"
	qmake -project -o settings.pro

linux-generate-makefile:
	@echo "generate qt makefile in builder environment"
	qmake -makefile -config release -o Makefile.qt CONFIG+=configSettings settings.pro

linux-generate-debug-makefile:
	@echo "generate debug qt makefile for gdb"
	qmake -makefile -config debug -o Makefile.qt CONFIG+=configSettings settings.pro

generate-makefile:
	@echo "generate makefile in docker"
	docker run --rm --platform linux/arm64 -v $(SRC_PATH):/src $(DOCKER_TAG_NAME) make linux-generate-makefile

linux-package-binary:
	@echo "generate package in builder environment"
	rm -f /src/output/*.tar.gz
	mkdir -p output/settings_$(VERSION)_linux-arm64
	cp res/settings_icon.png output/settings_$(VERSION)_linux-arm64
	cp res/settings_config.ini output/settings_$(VERSION)_linux-arm64
	cp tools/*.sh output/settings_$(VERSION)_linux-arm64
	cp output/settings output/settings_$(VERSION)_linux-arm64
	cd output/ && tar -czvf settings_$(VERSION)_linux-arm64.tar.gz settings_$(VERSION)_linux-arm64

package-binary: build-image
	@echo "generate makefile in docker"
	docker run --rm --platform linux/arm64 -v $(OUTPUT_PATH):/src/output $(DOCKER_TAG_NAME) make linux-package-binary

install:
	@echo "install settings DESTINATION_PATH=$(DESTINATION_PATH)"
	mkdir -p $(DESTINATION_PATH)/etc/settings
	cp output/settings $(DESTINATION_PATH)/usr/bin/
	cp tools/start_*.sh $(DESTINATION_PATH)/usr/bin/
	cp res/settings_config.ini $(DESTINATION_PATH)/etc/settings
	cp res/settings_icon.png $(DESTINATION_PATH)/usr/share/icons/hicolor/24x24/apps/

clean:
	rm -rf output/settings
	rm -f _obj/*.o
	rm -f _autogen/*
	rm -f qrc_*.cpp

create-sbom: clean-sbom
	cd output && sbom-tool generate -b . -bc . -pn settings -pv $(VERSION) -ps Advantech -nsb "https://github.com/Advantech-IIoT"
	cp output/_manifest/spdx_2.2/manifest.spdx.json $(DESTINATION_PATH)/../../scripts/out/sbom/settings.manifest.spdx.json

clean-sbom:
	cd output && rm -rf _manifest
