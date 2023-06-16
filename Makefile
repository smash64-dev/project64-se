
QT_BASE_DIR   ?= QT
QT_BASE_URL   ?= https://github.com/gonetz/GLideN64/releases/download/qt_build
QT_BUILD_NAME ?= qt-5_15-x86-msvc2017-static

.PHONY: clean gliden64 hidapi pj64raphnetraw project64 qt

clean:
	MSBuild.exe project64/Project64.sln /target:Clean

gliden64:
	export QTDIR="$(shell pwd)/${QT_BASE_DIR}/${QT_BUILD_NAME}"
	MSBuild.exe GLideN64/projects/msvc/GLideNUI.vcxproj /p:Configuration=Release /p:Platform=Win32
	MSBuild.exe GLideN64/projects/msvc/GLideN64.sln /p:Configuration=Release /p:Platform=Win32

hidapi:
	MSBuild.exe hidapi/windows/hidapi.sln /p:Configuration=Release /p:Platform=Win32

	# keeps pj64raphnetraw compatibility
	cp -rf hidapi/windows/Release hidapi/windows/.libs

pj64raphnetraw:
	mingw32-make.exe -C pj64raphnetraw/src/

project64:
	MSBuild.exe project64/Project64.sln /property:Configuration=Release /property:Platform=Win32

qt:
	mkdir -p ${QT_BASE_DIR}
	curl -L -o ${QT_BASE_DIR}/${QT_BUILD_NAME}.7z ${QT_BASE_URL}/${QT_BUILD_NAME}.7z
	7z x -o${QT_BASE_DIR}/${QT_BUILD_NAME} ${QT_BASE_DIR}/${QT_BUILD_NAME}.7z
