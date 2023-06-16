
.PHONY: clean project64

clean:
	msbuild project64/Project64.sln /target:Clean

project64:
	msbuild project64/Project64.sln /property:Configuration=Release /property:Platform=Win32
